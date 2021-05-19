#include "queue.h"
#include "accessoryDataGenerator.h"

#define DCC_PIN 4            // Arduino pin for DCC out
#define BUTTON_PIN 2         // the number of the pushbutton pin
#define POTENTIOMETER_PIN A5 // the number of the Potentiometer input
#define LED_PIN 13           // the number of the LED pin

// ISR-specific variables
#define PREAMBLE 0  // definitions for ISR state machine
#define SEPERATOR 1 // definitions for ISR state machine
#define SENDBYTE 2  // definitions for ISR state machine

//Timer frequency is 2MHz for ( /8 prescale from 16MHz )
#define TIMER_SHORT 0x8D               // 58usec pulse length 141 255-141=114
#define TIMER_LONG 0x1B                // 116usec pulse length 27 255-27 =228
unsigned char lastTimer = TIMER_SHORT; // store last timer value

unsigned char flag = 0;       // used for short or long pulse in the ISR
bool secondInterrupt = false; // pulse up or down

unsigned char state = PREAMBLE;
unsigned char preambleCount = 16;
unsigned char outbyte = 0;
unsigned char cbit = 0x80;

int DELAY_VALUE = 0;
int SPEED_VALUE = 0;

// Message / train control related variables
volatile unsigned char locoAddresses[] = {7, 11, 40}; // this is the (fixed) address of the loco
unsigned char amountOfLocoAddresses = 2;
volatile unsigned int locoAddressIndex = 0;
volatile int buttonState = 0;     // used to remember the state of the button
unsigned short emergencyStop = 0; // if true, all trains will be stopped until the button is pressed;

// light addresses mapped up starting with 12 and going down the track on the schematic
const unsigned short lightAddresses[] = {12, 52, 21, 51, 62, 22, 61, 11, 14, 42, 82, 81, 31, 32, 91, 92, 111, 112, 102, 101, 122, 121, 131, 132, 151, 152, 142, 141};

// switch addresses mapped up starting with 12 and going down the track on the schematic
const unsigned short switchAddresses[] = {221, 222, 234, 233, 224, 223, 231, 232, 242, 250, 249, 241, 243, 251, 244, 252};

// The program can store up to a certain amount of commands to send out
// if there is nothing in the queue, an idle message is sent out
// each command consists of two bytes, which are stored together as a short
struct Queue *commandQueue;

struct Message // buffer for command
{
   unsigned char data[7];
   unsigned char len;
};

#define MAXMSG 2

struct Message msg[MAXMSG] =
    {
        {{0xFF, 0, 0xFF, 0, 0, 0, 0}, 3}, // idle msg
        {{1, 0, 0, 0, 0, 0, 0}, 3}        // locoMsg with 128 speed steps
};

int msgIndex = 0;
int byteIndex = 0;

ISR(TIMER2_OVF_vect) //Timer2 overflow interrupt vector handler
{
   //Capture the current timer value TCTN2. This is how much error we have
   //due to interrupt latency and the work in this function
   //Reload the timer and correct for latency.
   unsigned char latency;
   //Serial.print("ISR got called: ");
   if (secondInterrupt)
   { // for every second interupt just toggle signal
      digitalWrite(DCC_PIN, 1);
      secondInterrupt = false;
      latency = TCNT2; // set timer to last value
      TCNT2 = latency + lastTimer;
   }
   else
   { // != every second interrupt, advance bit or state
      digitalWrite(DCC_PIN, 0);
      secondInterrupt = true;
      switch (state)
      {
      case PREAMBLE:
         flag = 1; // short pulse
         preambleCount--;
         if (preambleCount == 0)
         {
            state = SEPERATOR; // advance to next state
            msgIndex++;        // get next message
            if (msgIndex >= MAXMSG)
            {
               msgIndex = 0;
            }
            byteIndex = 0; //start msg with byte 0
         }
         break;
      case SEPERATOR:
         flag = 0;         // long pulse and then advance to next state
         state = SENDBYTE; // goto next byte ...
         outbyte = msg[msgIndex].data[byteIndex];
         cbit = 0x80; // send this bit next time first
         break;
      case SENDBYTE:
         //Serial.println(outbyte);
         if ((outbyte & cbit) != 0)
         {
            flag = 1; // send short pulse
         }
         else
         {
            flag = 0; // send long pulse
         }
         cbit = cbit >> 1;
         if (cbit == 0)
         { // last bit sent
            //Serial.print(" ");
            byteIndex++;
            if (byteIndex >= msg[msgIndex].len) // is there a next byte?
            {                                   // this was already the XOR byte then advance to preamble
               state = PREAMBLE;
               preambleCount = 16;
               //Serial.println();
            }
            else
            { // send separator and advance to next byte
               state = SEPERATOR;
            }
         }
         break;
      }

      if (flag)
      { // data = 1 short pulse
         latency = TCNT2;
         TCNT2 = latency + TIMER_SHORT;
         lastTimer = TIMER_SHORT;
         //Serial.print('1');
      }
      else
      { // data = 0 long pulse
         latency = TCNT2;
         TCNT2 = latency + TIMER_LONG;
         lastTimer = TIMER_LONG;
         // Serial.print('0');
      }
   }
   //Serial.println(latency);
}

// Variables for ensuring the button presses don't get registered multiple times
// Also known as debouncing
volatile unsigned long buttonLastDebounceTime = 0;
volatile unsigned long buttonDebounceDelay = 50;

void pin_ISR()
{
   // Get time since last bounce and return if it us less than the button bounce delay
   if ((millis() - buttonLastDebounceTime) < buttonDebounceDelay)
   {
      return;
   }
   buttonLastDebounceTime = millis();

   digitalWrite(LED_PIN, HIGH);

   if (emergencyStop == 1)
   {
      Serial.println("Resuming normal operation");
      emergencyStop = 0;
      // Start all of the trains
      for (short i = 0; i < (sizeof(locoAddresses) / sizeof(locoAddresses[0])); i++)
      {
         Serial.print("Adding a command for train address: ");
         Serial.println(locoAddresses[i]);
         enQueue(commandQueue, createSpeedCommand(locoAddresses[i]));
      }
      return;
   }

   Serial.print("Button got pressed, state: ");
   Serial.println(buttonState);

   // change all the lights according to the button state
   for (short i = 0; i < (sizeof(lightAddresses) / sizeof(lightAddresses[0])); i++)
   {
      enQueue(commandQueue, accessoryDataGenerator(lightAddresses[i], 1, buttonState));
   }

   // change all the switches according to the button state
   for (short i = 0; i < (sizeof(switchAddresses) / sizeof(switchAddresses[0])); i++)
   {
      enQueue(commandQueue, accessoryDataGenerator(switchAddresses[i], 1, buttonState));
   }

   buttonState = !buttonState;
}

unsigned short createSpeedCommand(char address)
{
   int speedValue = analogRead(POTENTIOMETER_PIN);
   // The potentiometer returns a value between 0 and 1023
   // the value is mapped to a high of 1022 to account for extra resistance in the circuit
   // The train speeds are controlled with 16 values, from 0 to 15;
   DELAY_VALUE = map(speedValue, 0, 1022, 3000, 0);
   speedValue = map(speedValue, 0, 1022, 0, 15);
   SPEED_VALUE = speedValue;

   //Serial.print("Speed value: ");
   //Serial.println(speedValue);

   // value of 96 corresponds to going forwards at speed 0
   char speedByte = 96 + speedValue;

   return (address << 8) | speedByte;
}

void setup(void)
{
   Serial.begin(9600);
   pinMode(DCC_PIN, OUTPUT);
   pinMode(LED_PIN, OUTPUT);

   // Attach an interrupt to the ISR vector for getting button input
   pinMode(BUTTON_PIN, INPUT);
   attachInterrupt(0, pin_ISR, FALLING);

   // Initialize the queue for storing commands
   commandQueue = createQueue();

   // Stop all of the trains
   for (short i = 0; i < (sizeof(locoAddresses) / sizeof(locoAddresses[0])); i++)
   {
      enQueue(commandQueue, (locoAddresses[i] << 8) | 0x61); // 0x61 - hardstop
   }

   // set all of the lights to red
   for (short i = 0; i < (sizeof(lightAddresses) / sizeof(lightAddresses[0])); i++)
   {
      enQueue(commandQueue, accessoryDataGenerator(lightAddresses[i], 1, 1));
   }

   // set all of the switches to forward
   for (short i = 0; i < (sizeof(switchAddresses) / sizeof(switchAddresses[0])); i++)
   {
      enQueue(commandQueue, accessoryDataGenerator(switchAddresses[i], 1, 1));
   }

   // Start all of the trains
   for (short i = 0; i < (sizeof(locoAddresses) / sizeof(locoAddresses[0])); i++)
   {
      enQueue(commandQueue, createSpeedCommand(locoAddresses[i]));
   }

   // Start the process of sending commands
   assembleDccMsg();

   // Start the timer for the internal ISR
   setupTimer2();
}

unsigned short loopDuration = 100; // Duration of the loop delays total, in miliseconds. Affects how often messages are assembled
unsigned short readingsPerLoop = 6;

void loop(void)
{
   //Serial.println("Loop time");
   // Read the distance twice per loop
   for (int i = 0; i < readingsPerLoop; i++)
   {
      delay(loopDuration / readingsPerLoop);
   }
   assembleDccMsg();
   digitalWrite(LED_PIN, LOW);
}

void assembleDccMsg()
{
   // there are no commands queued, sending an idle command
   if (getFirst(commandQueue) == 0)
   {
      noInterrupts();
      msg[1].data[0] = 0x01;
      msg[1].data[1] = 0x00;
      msg[1].data[2] = 0x01 ^ 0x00;
      interrupts();
      return;
   }

   //Serial.println("There is a command in the queue!");

   unsigned short command = getFirst(commandQueue);
   unsigned char byteOne = command >> 8;     // in 0x1234, this is 0x12
   unsigned char byteTwo = command & 0x00FF; // in 0x1234, this is 0x34
   //Serial.print("Train: ");
   //Serial.println((int)byteOne);
   //Serial.print("Command: ");
   //Serial.println((int)byteTwo);
   deQueue(commandQueue); // remove the command from the queue
   // Code for combining the bytes back into one short for storage in the commandQueue:
   // short testTwo = (byteOne << 8) | byteTwo;

   noInterrupts(); // make sure that only "matching" parts of the message are used in ISR
   msg[1].data[0] = byteOne;
   msg[1].data[1] = byteTwo;
   msg[1].data[2] = byteOne ^ byteTwo; // checksum

   interrupts(); // tis just enables interrupts
}

void triggerEmergencyStop()
{
   Serial.println("Emergency Stop got called. Stopping all trains indefinitely");
   emergencyStop = 1;

   for (short i = 0; i < (sizeof(locoAddresses) / sizeof(locoAddresses[0])); i++)
   {
      Serial.print("Adding a hardstop for train address: ");
      Serial.println(locoAddresses[i]);
      enQueue(commandQueue, (locoAddresses[i] << 8) | 0x61);
   }
}