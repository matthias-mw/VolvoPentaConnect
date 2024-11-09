// Doxygen Documentation
/*! \file 	process_warnings.cpp
 *  \brief  Implementation file for processing warnings
 *
 * This file contains the implementations for functions and classes
 * used to process warnings in the system.
 *
 * \author 		Matthias Werner
 * \date		November 2024
 * - Prozessor:         ESP32-WROOM
 * - Hardware:          az-delivery-devkit-v4
 */

#include <process_warnings.h>

// ********************************************************
// Constructor
ProcessWarnings::ProcessWarnings(AcquireData &data) : data(data)
{
  // Initialization code


}

// ********************************************************
// Check for warnings
void ProcessWarnings::checkWarnings()
{
  // Code to check for warnings

  // operate the warning LED
  operateWarningLED(true);
}

// ********************************************************
// Operate the warning LED
void ProcessWarnings::operateWarningLED(bool warningActive)
{
  // Code to operate the warning LED
  uint32_t currentMillis = millis();

  if (warningActive)
  {
    // Toggle the LED when it is time to do so
    if (currentMillis - warningLedCounter >= WARNING_LED_DELAY)
    {
      // Toggle the LED
      warningLedState = !warningLedState;
      if (warningLedState)
      {
        // turn LED on
        digitalWrite(STATUS_LED_PIN, LED_PIN_ON);
      }
      else
      {
        // turn LED off
        digitalWrite(STATUS_LED_PIN, LED_PIN_OFF);
      }

      // save the last time the LED was toggled
      warningLedCounter = currentMillis;
    }
  }
}
