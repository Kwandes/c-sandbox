#define DECODE_NEC
//#define DECODE_DENON
#include <IRremote.h>
#include "irCodes.h"

#include <stdio.h>
//#include "readFromStruct.h"
#include "definitions.h"
#include "instruction.h"
#include "write.h"
#include "writeToTrain.h"

#define ENGINE_NUMBER 11
//#define COMMAND SPEED8
volatile char COMMAND = SPEED8;

#define ACTIVE_PIN_A 4
#define ACTIVE_PIN_B 0
#define ACTIVE_PIN_C 0

//volatile char COMMAND = 0x69;

const int BUTTON_PIN = 2;     // the number of the pushbutton pin
const int IR_RECEIVER_PIN = 3; // the number of the IR receiver pin
const int LED_PIN = 13;       // the number of the LED pin

// If variable changes, put `volatile` in its decleration

void testCodes(int codeHex);
void readInstructionData(struct Instruction instruction);


struct Instruction blankInstruction =
{
        blankPreamble,      // preamble part 1
        blankPreamble,      // preamble part 2
        blankSeparator,     // -- Separating bit --
        blankEngineNumber,  // Engine Number
        blankSeparator,     // -- Separating bit --
        blankCommand,       // Command
        blankSeparator,     // -- Separating bit --
        blankInstruction.command ^ blankInstruction.engineNumber,  // Checksum
        blankEndOfMessage   // --- End of message bit ---
};

struct Instruction testInstruction =
{
        PREAMBLE,           // preamble part 1
        PREAMBLE,           // preamble part 2
        SEPARATOR,          // -- Separating bit --
        ENGINE_NUMBER,      // Engine Number
        SEPARATOR,          // -- Separating bit --
        COMMAND,            // Command
        SEPARATOR,          // -- Separating bit --
        testInstruction.command ^ testInstruction.engineNumber,  // Checksum
        END_OF_MESSAGE        // --- End of message bit ---
};

void setup()
{
  // Enable a pin as output for the train
  pinMode(ACTIVE_PIN_A,OUTPUT);
  // initialize the LED pin as an output:
  pinMode(LED_PIN, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(BUTTON_PIN, INPUT);
  Serial.begin(9600);
  IrReceiver.begin(IR_RECEIVER_PIN, ENABLE_LED_FEEDBACK);

  // Attach an interrupt to the ISR vector
  attachInterrupt(0, pin_ISR, RISING);
  attachInterrupt(1, pin_IR_ISR, FALLING);
}

void loop()
{
  //delay(1000);
    //Serial.println("\n--------------- test output ---------------\n");
    //readInstructionData(blankInstruction);
    //writeToTrain(ACTIVE_PIN_A, blankInstruction);
    //delay(1000);
    //Serial.println("\n--------------- test output ---------------\n");
    //readInstructionData(testInstruction);
    //delay(100);
    writeToTrain(ACTIVE_PIN_A, testInstruction);
    delayMicroseconds(1);
    writeToTrain(ACTIVE_PIN_A, testInstruction);
    writeToTrain(ACTIVE_PIN_A, testInstruction);
    writeToTrain(ACTIVE_PIN_A, testInstruction);
    writeToTrain(ACTIVE_PIN_A, testInstruction);
    writeToTrain(ACTIVE_PIN_A, testInstruction);
    writeToTrain(ACTIVE_PIN_A, testInstruction);
    writeToTrain(ACTIVE_PIN_A, testInstruction);
}

void pin_IR_ISR()
{
  // IR stuff
  if (IrReceiver.decode())
  {
    //Serial.println(IrReceiver.decodedIRData.command, HEX);
    testCodes(IrReceiver.decodedIRData.command);
    IrReceiver.resume();
  }
}

void pin_ISR()
{
  digitalWrite(LED_PIN, HIGH);
  Serial.println("Button got pressed");
}

void testCodes(int codeHex)
{
  switch (codeHex)
  {
  case IR_POWER:
    Serial.println("IR_POWER");
    break;

  case IR_VOL_PLUS:
    Serial.println("IR_VOL_PLUS");
    break;

  case IR_VOL_MINUS:
    Serial.println("IR_VOL_MINUS");
    break;

  case IR_FUNC_STOP:
    Serial.println("IR_FUNC_STOP");
    break;

  case IR_PREVIOUS:
    Serial.println("IR_PREVIOUS");
    break;

  case IR_PLAY_PAUSE:
    Serial.println("IR_PLAY_PAUSE");
    break;

  case IR_NEXT:
    Serial.println("IR_NEXT");
    break;

  case IR_DOWN:
    Serial.println("IR_DOWN");
    break;

  case IR_UP:
    Serial.println("IR_UP");
    break;

  case IR_EQ:
    Serial.println("IR_EQ");
    testInstruction.command = SPEED12;
    break;

  case IR_ZERO:
    Serial.println("IR_ZERO");
    testInstruction.command = HARDSTOP;
    break;

  case IR_ONE:
    Serial.println("IR_ONE");
    testInstruction.command = SPEED1;
    break;

  case IR_TWO:
    Serial.println("IR_TWO");
    testInstruction.command = SPEED2;
    break;

  case IR_THREE:
    Serial.println("IR_THREE");
    testInstruction.command = SPEED3;
    break;

  case IR_FOUR:
    Serial.println("IR_FOUR");
    testInstruction.command = SPEED4;
    break;

  case IR_FIVE:
    Serial.println("IR_FIVE");
    testInstruction.command = SPEED5;
    break;

  case IR_SIX:
    Serial.println("IR_SIX");
    testInstruction.command = SPEED6;
    break;

  case IR_SEVEN:
    Serial.println("IR_SEVEN");
    testInstruction.command = SPEED7;
    break;

  case IR_EIGHT:
    Serial.println("IR_EIGHT");
    testInstruction.command = SPEED8;
    break;

  case IR_NINE:
    Serial.println("IR_NINE");
    testInstruction.command = SPEED9;
    break;

  default:
    Serial.print("Fake button aka wrong value: 0x");
    Serial.println(codeHex);
    break;
  }
}


// TODO - Find out how to use Serial outside of main, for now, it lives here
void readInstructionData(struct Instruction instruction)
{
    char* preambleFixed = "whoops";
    int total = (instruction.preamble[0] + instruction.preamble[1]);
    if (total == 510)
    {
        preambleFixed = "0xFFFF";
    }
    if (total == 0)
    {
        preambleFixed = "0x0000";
    }

    noInterrupts();
    Serial.print("The preamble is: ");
    Serial.print(preambleFixed);
    Serial.print(" The engine number is: ");
    Serial.print(instruction.engineNumber);
    Serial.print(" The command is: ");
    Serial.print(instruction.command);
    Serial.print(" The checksum is: ");
    Serial.print(instruction.checksum); 
    Serial.println();
    interrupts();
}