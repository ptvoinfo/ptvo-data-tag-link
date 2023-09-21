/* ***********************************************************************************************
 *  Example demonstrating PTVO Data Tag Link's interface
 *  with an IoT device with the PTVO firmware inside
 * *********************************************************************************************** */
#include "DataHandler.h" // Library to handle serial commands
#include "DataDispatcherBase.h"
#include "extensions/PtvoUartSensor.h"

// Arduino pin that supports PWM
// Arduino Nano - 11
#define PWM_PIN 11
#define ANALOG_INPUT_PIN A0

#define ENDPOINT_ON_OFF ENDPOINT_2
#define ENDPOINT_ANALOG_VALUE ENDPOINT_3
#define ENDPOINT_PWM ENDPOINT_4
#define ENDPOINT_CLICKS ENDPOINT_5

/// <summary>
/// The On/Off state of the PWM output below. If the state is Off, the LED is Off
// If the state is On, the LED brightness depends on the PWM level.
/// </summary>
uint8_t bOnOffState = 1; // LED state
uint8_t bOnOffStatePrevios = 1;

long uAdcLastRead = 0;        // ADC last read time
int uAdcReadInterval = 20000; // read an analog input every N milliseconds
float fAnalogValue = -1;      // Analog sensor (-1 - undefined)

// Pulse width modulation (PWM) of of an output (LED)
// Default: 255 (full width, always ON)
uint8_t uPwmLevel = 255;
uint8_t uPwmLevelBeforeOff = 255;

// The pin that the button is connected to. The other side of the button
// should be connected to ground.
const int ButtonPin = 3;

/// <summary>
/// The command handler looks after parsing and dispatching serial commands to Arduino functions
// This demo project uses only 4 variables (one variable per endpoint)
/// </summary>
PDTL::PtvoUartSensor<4, 4> SerialCommandHandler;

/// <summary>
/// Called when a new value for the added variable was received.
/// </summary>
void OnOffWrite_Callback(PDTL::VariableData &rVariableInfo, bool bIsWrite)
{
  if (!bIsWrite)
  {
    return;
  }
  // set the LED state received from a link
  if (bOnOffState == bOnOffStatePrevios)
  {
    return;
  }
  bOnOffStatePrevios = bOnOffState;
  if (bOnOffState)
  {
    uPwmLevel = uPwmLevelBeforeOff;
    analogWrite(PWM_PIN, uPwmLevel);
    return;
  }

  uPwmLevelBeforeOff = uPwmLevel;
  analogWrite(PWM_PIN, 0);
}

/// <summary>
/// Called when a new value for the added variable was received.
/// </summary>
void PwmState_Callback(PDTL::VariableData &rVariableInfo, bool bIsWrite)
{
  if (!bIsWrite)
  {
    return;
  }
  if (bOnOffState)
  {
    analogWrite(PWM_PIN, uPwmLevel);
  }
  uPwmLevelBeforeOff = uPwmLevel;
}

void Default_Command_Callback(uint8_t uCommandId, uint8_t uVariableId)
{
  if (uVariableId == PDTL_ID_SYS)
  {
    if (uCommandId >= PDTL::StatusHold )
    {
      Serial.print(F("Zigbee network status:"));
      Serial.println(uCommandId);
    }
  }
}

void setup()
{
  Serial.begin(9600);
  Serial.println(F("UartSensor"));
  Serial.println(F("=========="));

  // Register a variable with a callback.
  SerialCommandHandler.AddVariable(ENDPOINT_ON_OFF, bOnOffState, OnOffWrite_Callback);
  // The analog value does not use a callback. We read it asynchronously
  SerialCommandHandler.AddVariable(ENDPOINT_ANALOG_VALUE, fAnalogValue);
  SerialCommandHandler.AddVariable(ENDPOINT_PWM, uPwmLevel, PwmState_Callback);
  // ENDPOINT_5 is used to report button clicks
  SerialCommandHandler.SetDefaultHandler(Default_Command_Callback);

  pinMode(PWM_PIN, OUTPUT);
  analogWrite(PWM_PIN, uPwmLevel); // default ON

  pinMode(ButtonPin, INPUT_PULLUP);
}

void loop()
{
  // Check for serial data and dispatch them.
  SerialCommandHandler.Process();

  // Update the LED
  uint32_t uNow = millis();
  uint8_t ledState;
  if ((uNow - uAdcLastRead) >= uAdcReadInterval)
  {
    uint16_t ADCValue = analogRead(ANALOG_INPUT_PIN);
    float fNewValue = ADCValue;
    if (fNewValue != fAnalogValue)
    {
      Serial.println(F("Analog value sent"));
      // report the changed value
      fAnalogValue = fNewValue;
      SerialCommandHandler.SendVariable(ENDPOINT_ANALOG_VALUE);
      uAdcLastRead = uNow;
    }
  }

  bool ButtonPressed = digitalRead(ButtonPin) == 0;
  static bool WasPressed = false;

  if (ButtonPressed)
  {
    if (!WasPressed)
    {
      Serial.println(F("Button clicked"));
      WasPressed = true;
      SerialCommandHandler.SendUInt8(ENDPOINT_CLICKS, PTVO_CLICK_SINGLE);
    }
  }
  else
  {
    WasPressed = false;
  }
}
