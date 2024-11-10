// Doxygen Documentation
/*! \file 	button_interpreter.cpp
 *  \brief  All Methods to interpret the buttons
 *
 * This File contains all the necessary methods to interpret the
 * buttons of the hardware.
 *
 * \author 		Matthias Werner
 * \date		November 2024
 * - Prozessor:         ESP32-WROOM
 * - Hardware:          az-delivery-devkit-v4
 */

#include "button_interpreter.h"

// ********************************************************
// GPIO pins of the buttons. The order of the pins must
// match the order of the buttons.
const uint8_t buttonPins[NUM_BUTTONS] = {BUTTON1_PIN, BUTTON2_PIN};

// ********************************************************
// ButtonInterpreter Constructor
ButtonInterpreter::ButtonInterpreter(DisplayData &lcdDisplay, ProcessWarnings &processWarnings)
    : lcdDisplayObject(lcdDisplay), processWarningsObject(processWarnings) //
{
  // Initialize the Button Interpreter
  for (int i = 0; i < NUM_BUTTONS; ++i)
  {
    debounceCounters[i] = 0;
    longPressFlags[i] = false;
    pressStartTimes[i] = 0;
  }
}

// ********************************************************
// Initialize the buttons
void ButtonInterpreter::initializeButtons()
{
  for (int i = 0; i < NUM_BUTTONS; ++i)
  {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
}

// ********************************************************
// Get the index of a button
int ButtonInterpreter::getButtonIndex(uint8_t gpioPin)
{
  for (int i = 0; i < NUM_BUTTONS; ++i)
  {
    if (buttonPins[i] == gpioPin)
    {
      return i;
    }
  }
  return -1;
}

// ********************************************************
// Check the state of a button
bool ButtonInterpreter::isButtonPressed(uint8_t gpioPin)
{
  return digitalRead(gpioPin) == HIGH;
}

// ********************************************************
// Get status of a button short pressed
bool ButtonInterpreter::getButtonShortPress(uint8_t buttonIndex)
{
  if (buttonIndex < NUM_BUTTONS)
  {
    bool wasPressed = shortPressFlags[buttonIndex];
    shortPressFlags[buttonIndex] = false; // Reset flag after reading
    return wasPressed;
  }
  return false;
}

// ********************************************************
// Get status of a button long pressed
bool ButtonInterpreter::getButtonLongPress(uint8_t buttonIndex)
{
  if (buttonIndex < NUM_BUTTONS)
  {
    bool wasPressed = longPressFlags[buttonIndex];
    longPressFlags[buttonIndex] = false; // Reset flag after reading
    return wasPressed;
  }
  return false;
}

// ********************************************************
// Update the state of a specific button
void ButtonInterpreter::updateButtonState(uint8_t gpioPin)
{
  int buttonIndex = getButtonIndex(gpioPin);
  if (buttonIndex < 0)
    return;

  // Check if the button is currently pressed
  if (isButtonPressed(gpioPin))
  {
    // Debounce the button
    debounceCounters[buttonIndex]++;

    // Check if the button has been pressed long enough to be considered a long press
    if (debounceCounters[buttonIndex] >= BUTTON_LONG_PRESS)
    {

      // *********** Long Press ***********
      if (longPressServed[buttonIndex] == false)
      {
        // Long press event detected
        longPressFlags[buttonIndex] = true;

        // Mark long press event as served
        longPressServed[buttonIndex] = true;
      }

// Debugging
#ifdef DEBUG_LEVEL
      if (DEBUG_LEVEL > 2)
      {
        Serial.println("Button 1 -> Long press action...");
      }

#endif // DEBUG_LEVEL
    }

  } // Check if the button was shortly pressed on release
  else
  {

    // *********** Short Press ***********
    // Detect short press event on button release
    if ((debounceCounters[buttonIndex] >= BUTTON_DEBOUNCE) && (debounceCounters[buttonIndex] < BUTTON_LONG_PRESS))
    {
      // *********** Short Press ***********
      // Short press has been detected
      shortPressFlags[buttonIndex] = true;

// Debugging
#ifdef DEBUG_LEVEL
      if (DEBUG_LEVEL > 2)
      {
        Serial.println("Button 1 -> Short press action...");
      }

#endif // DEBUG_LEVEL
    }

    // Reset the debounce counter
    debounceCounters[buttonIndex] = 0;
    // Reset long press event
    longPressServed[buttonIndex] = false;
  }

} // End of updateButtonState

// ********************************************************
// Process the state of all buttons
void ButtonInterpreter::processAllButtonState(uint8_t currentLcdPage)
{
  // Update the Button State
  for (int i = 0; i < NUM_BUTTONS; ++i)
  {
    // Update the state of the button
    updateButtonState(buttonPins[i]);

    // Trigger the corresponding action for Button
    triggerButtonAction(i, currentLcdPage);
  }

} // End of processAllButtonState

// ********************************************************
// Trigger Action for a specific button
void ButtonInterpreter::triggerButtonAction(uint8_t buttonIndex, uint8_t currentLcdPage)
{
  // =============================================
  // All actions that can be triggered by button 1 (Up)
  // =============================================
  if (buttonIndex == 0)
  {
    // Action if button 1 is pressed shortly
    if (getButtonShortPress(buttonIndex))
    {
      // show the previous LCD panel page
      lcdDisplayObject.setLcdCurrentPage(PAGE_ALARM);

      // Reset the backlight counter of the LCD Panel
      lcdDisplayObject.resetLcdBacklightCounter();
      lcdDisplayObject.updateLcdBacklight();
    }

    // Action if button 1 is pressed long
    if (getButtonLongPress(buttonIndex))
    {
      // switch the action depending on the current LCD page
      switch (currentLcdPage)
      {
        // Separate the actions for the different LCD pages
        // Uncomment cases wher no specefic action is needed
        // case WELCOME_PAGE:
        //   // do something
        //   break;

        // case PAGE_ENGINE:
        //   // do something
        //   break;

        // case PAGE_TEMPERATURE:
        //   // do something
        //   break;

        // case PAGE_VOLTAGE:
        //   // do something
        //   break;

        // case PAGE_SPEED:
        //   // do something
        //   break;

        // case PAGE_ALTERNATOR:
        //   // do something
        //   break;

        // case PAGE_1WIRE_LIST:
        //   // do something
        //   break;

      default:

        // acknowledge all warnings
        processWarningsObject.acknowledgeAllWarnings();

        break;
      }
    }
  }
  // =============================================
  // All actions that can be triggered by button 2 (Down)
  // =============================================
  if (buttonIndex == 1)
  {

    // Action if button 1 is pressed shortly
    if (getButtonShortPress(buttonIndex))
    {
      // show the next LCD panel page
      lcdDisplayObject.increaseLcdCurrentPage();

      // Reset the backlight counter of the LCD Panel
      lcdDisplayObject.resetLcdBacklightCounter();
      lcdDisplayObject.updateLcdBacklight();
    }

    // Action if button 1 is pressed long
    if (getButtonLongPress(buttonIndex))
    {
      // switch the action depending on the current LCD page
      switch (currentLcdPage)
      {
        // Separate the actions for the different LCD pages
        // Uncomment cases wher no specefic action is needed
        // case WELCOME_PAGE:
        //   // do something
        //   break;

        // case PAGE_ENGINE:
        //   // do something
        //   break;

      case PAGE_TEMPERATURE:

        // show LCD panel page with the list of 1Wire devices
        lcdDisplayObject.setLcdCurrentPage(PAGE_1WIRE_LIST);

        break;

        // case PAGE_VOLTAGE:
        //   // do something
        //   break;

        // case PAGE_SPEED:
        //   // do something
        //   break;

        // case PAGE_ALTERNATOR:
        //   // do something
        //   break;

        // case PAGE_1WIRE_LIST:
        //   // do something
        //   break;

      default:
        // do something

        break;
      }
    }
  }

} // End of triggerButtonAction
