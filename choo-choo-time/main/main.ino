#define DECODE_NEC
//#define DECODE_DENON
#include <IRremote.h>
#include "irCodes.h"

const int BUTTON_PIN = 2;     // the number of the pushbutton pin
const int IR_RECEIVE_PIN = 3; // the number of the IR receiver pin
const int LED_PIN = 13;       // the number of the LED pin

// If variable changes, put `volatile` in its decleration

void testCodes(int codeHex);

void setup()
{
  // initialize the LED pin as an output:
  pinMode(LED_PIN, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(BUTTON_PIN, INPUT);
  Serial.begin(9600);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  // Attach an interrupt to the ISR vector
  attachInterrupt(0, pin_ISR, RISING);
  attachInterrupt(1, pin_IR_ISR, FALLING);
}

void loop()
{
  delay(10);
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
    break;

  case IR_ZERO:
    Serial.println("IR_ZERO");
    break;

  case IR_ONE:
    Serial.println("IR_ONE");
    break;

  case IR_TWO:
    Serial.println("IR_TWO");
    break;

  case IR_THREE:
    Serial.println("IR_THREE");
    break;

  case IR_FOUR:
    Serial.println("IR_FOUR");
    break;

  case IR_FIVE:
    Serial.println("IR_FIVE");
    break;

  case IR_SIX:
    Serial.println("IR_SIX");
    break;

  case IR_SEVEN:
    Serial.println("IR_SEVEN");
    break;

  case IR_EIGHT:
    Serial.println("IR_EIGHT");
    break;

  case IR_NINE:
    Serial.println("IR_NINE");
    break;

  default:
    Serial.print("Fake button aka wrong value: 0x");
    Serial.println(codeHex);
    break;
  }
}
