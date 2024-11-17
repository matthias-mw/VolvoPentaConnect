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
  warningActive = false;


}

// ********************************************************
// Check for warnings
void ProcessWarnings::checkAndProcessWarnings()
{
  //When a flag is set and not acknowledged, the warning is active
  if((data.currentEngineDiscreteStatus.flgHighCoolantTemp.isFlagSet() &&
    !data.currentEngineDiscreteStatus.flgHighCoolantTemp.isFlagAcknowledged())||

    (data.currentEngineDiscreteStatus.flgHighExhaustTemp.isFlagSet() &&
    !data.currentEngineDiscreteStatus.flgHighExhaustTemp.isFlagAcknowledged())||

    (data.currentEngineDiscreteStatus.flgHighGearboxTemp.isFlagSet() &&
    !data.currentEngineDiscreteStatus.flgHighGearboxTemp.isFlagAcknowledged())||

    (data.currentEngineDiscreteStatus.flgHighAlternatorTemp.isFlagSet() &&
    !data.currentEngineDiscreteStatus.flgHighAlternatorTemp.isFlagAcknowledged())||

    (data.currentEngineDiscreteStatus.flgHighSeaWaterTemp.isFlagSet() &&
    !data.currentEngineDiscreteStatus.flgHighSeaWaterTemp.isFlagAcknowledged())||

    (data.currentEngineDiscreteStatus.flgLowOilPressure.isFlagSet() &&
    !data.currentEngineDiscreteStatus.flgLowOilPressure.isFlagAcknowledged()))
  {
    this->warningActive = true;
  }
  else
  {
    this->warningActive = false;
  }
  
  // operate the warning LED
  operateWarningLED(this->warningActive );
  
}

// ********************************************************
// Acknowledge all warnings
void ProcessWarnings::acknowledgeAllWarnings()
{
  // Acknowledge all warnings
  data.currentEngineDiscreteStatus.flgHighCoolantTemp.acknowledgeFlag();
  data.currentEngineDiscreteStatus.flgHighExhaustTemp.acknowledgeFlag();
  data.currentEngineDiscreteStatus.flgHighGearboxTemp.acknowledgeFlag();
  data.currentEngineDiscreteStatus.flgHighAlternatorTemp.acknowledgeFlag();
  data.currentEngineDiscreteStatus.flgHighSeaWaterTemp.acknowledgeFlag();
  data.currentEngineDiscreteStatus.flgLowOilPressure.acknowledgeFlag();
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
  else
  {
    // Turn off the LED
    digitalWrite(STATUS_LED_PIN, LED_PIN_OFF);
  }
}
