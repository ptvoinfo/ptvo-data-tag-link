/* ***********************************************************************************************
 *  Example demonstrating PTVO Data Tag Link's interface
 * *********************************************************************************************** */
#include "DataHandler.h"        // Library to handle serial commands
#include "DataDispatcherBase.h" 

/// <summary>
/// Variables that control the blink interval and keep track of when we blinked.
/// </summary>
long LastBlink = 0;     // Time we last blinked the LED
int ToggleTime = 20000; // LED will be toggled after this time [milliseconds]
uint8_t bState = 1;     // LED state

/// <summary>
/// The command handler looks after parsing and dispatching serial commands to Arduino functions
/// </summary>
DataHandler<> SerialCommandHandler;

/// <summary>
/// Called when a new value for the added variable was received.
/// </summary>
void State_Callback(PDTL::VariableData &rVariableInfo, bool bIsWrite)
{
  // set the LED state received from a link
  if (bIsWrite)
  {
    LastBlink = millis();
    digitalWrite(LED_BUILTIN, (bState) ? HIGH : LOW);
  }
}

void Default_Command_Callback(uint8_t uCommandId, uint8_t uVariableId)
{
  // nothing to do
  Serial.print(F("Command received:"));Serial.println(uCommandId);
}

void setup()
{
  Serial.begin(9600);
  Serial.println(F("Blink"));
  Serial.println(F("=========="));

  // Register a variable with a callback.
  SerialCommandHandler.AddVariable(PDTL_VARIABLE_1, bState, State_Callback);
  SerialCommandHandler.SetDefaultHandler(Default_Command_Callback);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // default ON
}

void loop()
{
  // Check for serial data and dispatch them.
  SerialCommandHandler.Process();

  // Update the LED
  uint32_t uNow = millis();
  uint8_t ledState;
  if ((uNow - LastBlink) >= ToggleTime)
  {
    bState = (bState) ? 0: 1;
    ledState = (bState) ? LOW : HIGH;
    digitalWrite(LED_BUILTIN, ledState);
    LastBlink = uNow;
    Serial.println(F("Blink"));
    SerialCommandHandler.SendVariable(PDTL_VARIABLE_1);
  }
}
