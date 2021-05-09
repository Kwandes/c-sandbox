#include "messageTypes.h"

#define DCC_PIN 4            // Arduino pin for DCC out
#define BUTTON_PIN 2         // the number of the pushbutton pin
#define POTENTIOMETER_PIN A5 // the number of the Potentiometer input
#define SONIC_PING_PIN 6     // trigger pin on the ultrasonic sensor
#define SONIC_ECHO_PIN 7     // echo pin on the ultrasonic sensor
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

// Message / train control related variables
unsigned char messageType = COMMAND_TRAIN_SPEED; // By default send train speed commands

volatile unsigned char locoAddresses[] = {11}; // this is the (fixed) address of the loco
volatile unsigned int locoAddressIndex = 0;
volatile int buttonState = 0; // used to determine the direction of the train movement

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

   buttonState = !buttonState;
   //Serial.print("Button got pressed, state: ");
   //Serial.println(buttonState);
}

void setup(void)
{
   Serial.begin(9600);
   pinMode(DCC_PIN, OUTPUT); // pin 4 this is for the DCC Signal

   // initialize the pushbutton pin as an input
   pinMode(BUTTON_PIN, INPUT);
   // initialize the LED pin as an output
   pinMode(LED_PIN, OUTPUT);

   // initialize ultrasonic sensor pins
   pinMode(SONIC_PING_PIN, OUTPUT);
   pinMode(SONIC_ECHO_PIN, INPUT);

   // Attach an interrupt to the ISR vector for getting button input
   attachInterrupt(0, pin_ISR, FALLING);

   assembleDccMsg();
   setupTimer2(); // Start the timer
}

void loop(void)
{
   //Serial.println("Loop time");
   delay(100);
   triggerUltrasonicReading();
   assembleDccMsg();
   digitalWrite(LED_PIN, LOW);
}

unsigned int ultrasonicDistanceThreshold = 50; // at which point something is marked as close vs far, in centimeters

void triggerUltrasonicReading()
{
   unsigned long duration;
   digitalWrite(SONIC_PING_PIN, LOW);
   delayMicroseconds(2);
   digitalWrite(SONIC_PING_PIN, HIGH);
   delayMicroseconds(10);
   digitalWrite(SONIC_PING_PIN, LOW);
   duration = pulseIn(SONIC_ECHO_PIN, HIGH);
   //Serial.print("Distance: ");
   Serial.println((duration / 29 / 2) < 50 );
}

void assembleDccMsg()
{

   unsigned char data, checksum;

   // Define data based on the type of the message that is supposed to be sent
   switch (messageType)
   {
   case COMMAND_TRAIN_SPEED:
   {
      int speedValue = analogRead(POTENTIOMETER_PIN);
      // The potentiometer returns a value between 0 and 1023
      // the value is mapped to a high of 1022 to account for extra resistance in the circuit
      // The train speeds are controlled with 16 values, from 0 to 15;
      speedValue = map(speedValue, 0, 1022, 0, 15);

      //Serial.print("Speed value: ");
      //Serial.println(speedValue);

      // buttonState indicates direction. 1 is forwards, 0 is backwards
      // value of 96 corresponds to going forwards at speed 0, 64 is going backwards at speed 0
      if (buttonState == 1)
      {
         data = 96 + speedValue;
      }
      else
      {
         data = 64 + speedValue;
      }
      break;
   }
   case COMMAND_TRAIN_EFFECT:
   {
      Serial.println("Train effects have not been implemented yet");
      break;
   }
   case COMMAND_SWITCH:
   {
      Serial.println("Switch commands have not been implemented yet");
      break;
   }
   default:
   {
      Serial.println("The switch got ignored");
      break;
   }
   }

   noInterrupts(); // make sure that only "matching" parts of the message are used in ISR
   msg[1].data[0] = locoAddresses[locoAddressIndex];
   msg[1].data[1] = data;
   checksum = msg[1].data[0] ^ data;
   msg[1].data[2] = checksum;

   //Serial.print("address: ");
   //Serial.println(locoAddresses[locoAddressIndex]);
   interrupts(); //QUESTION: Where does method come from - tis just enables interrupts

   if (locoAddressIndex >= (sizeof(locoAddresses) / sizeof(locoAddresses[0]) - 1))
   {
      locoAddressIndex = 0;
   }
   else
      locoAddressIndex++;
}

// control of distance between trains aka collision prevention system aka CPS
// the system only works with two trains on the track, and the track having a switch for a separate, longer route
unsigned long timeBetweenTrains = 0; // time between detections
unsigned long lastDetectionTime = 0;
unsigned long minimumDistanceBetweenTrains = 5000; // 5000 miliseconds aka 5 seconds
unsigned int isTimeBetweenTrainsDecreasing = 0;    // bool - true if the trains keep taking less time between each reading set,aka get closer to each other
unsigned int isFirstTrain = 0;                     //
unsigned int setSwitchForLongerRoute = 0;          // if true, the switch is set for the longer route
unsigned long timeBetween = 0;

void pin_soundSensor_ISR()
{
   // only triggered on the first time the trains pass the sensor
   if (lastDetectionTime == 0)
   {
      lastDetectionTime = millis();
      return;
   }

   // if the first train passes and the time between the train is decreasing, send the second train onto the longer track
   if (isFirstTrain == 1 && isTimeBetweenTrainsDecreasing == 1)
   {
      setSwitchForLongerRoute = 1;
      messageType = COMMAND_SWITCH;
   }
   if (isFirstTrain == 1 && isTimeBetweenTrainsDecreasing == 0) // send both trains on the same track
   {
      setSwitchForLongerRoute = 0;
      messageType = COMMAND_SWITCH;
   }
   else if (isFirstTrain == 0) // if the second train is passing, calculate the distance
   {
      setSwitchForLongerRoute = 0;

      // decide if the second train needs to be slowed down
      if (timeBetweenTrains < (millis() - lastDetectionTime))
      {
         isTimeBetweenTrainsDecreasing = 1; // true
      }
      else
         isTimeBetweenTrainsDecreasing = 0; //false
   }

   // flip which train will we detected by the next interrupt
   isFirstTrain = !isFirstTrain;
   // reset the timers
   timeBetweenTrains = millis() - lastDetectionTime;
   lastDetectionTime = millis();

   // if the distance between trains is too short, hardstop both (emergency)
   if (timeBetweenTrains < minimumDistanceBetweenTrains)
   {
      messageType = COMMAND_EMERGENCY_HARDSTOP;
   }
}

void decideSwitches()
{
   if (isTimeBetweenTrainsDecreasing)
   {
   }
}