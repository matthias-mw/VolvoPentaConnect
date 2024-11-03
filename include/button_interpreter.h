// Doxygen Documentation
/*! \file 	button.h
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
#ifndef _button_interpreter_h_
#define _button_interpreter_h_

#include <Arduino.h>
#include <hardwareDef.h>


/// GPIO pins of the buttons. The order of the pins must
/// match the order of the buttons.
const uint8_t buttonPins[NUM_BUTTONS] = {BUTTON1_PIN, BUTTON2_PIN};

/*! \class ButtonInterpreter
 *  \brief Class to interpret the buttons.
 *
 * This class contains all the necessary methods to interpret the
 * buttons of the hardware.
 */
class ButtonInterpreter
{
public:
  /*!
   * \brief Constructor for ButtonInterpreter.
   */
  ButtonInterpreter();

  /*!
   * \brief Initialize the buttons.
   */
  void initializeButtons();

  /*! \brief Get status of a button short press.
   * \param buttonIndex The index of the button.
   * \return True if the button is pressed, false otherwise.
   */
  bool getButtonShortPress(uint8_t buttonIndex);

  /*! \brief Get status of a button long press.
   * \param buttonIndex The index of the button.
   * \return True if the button is pressed, false otherwise.
   */
  bool getButtonLongPress(uint8_t buttonIndex);

  /*!
   * \brief Update the state of a button.
   * 
   * This method updates the state of a button and checks for long 
   * presses and debounces the button.
   * 
   * \param gpioPin The GPIO pin of the button.
   */
  void updateButtonState(uint8_t gpioPin);


private:
  /*!
   * \brief Check the state of a button.
   * \param gpioPin The GPIO pin of the button.
   * \return True if the button is pressed, false otherwise.
   */
  bool isButtonPressed(uint8_t gpioPin);

  /// Debounce counters for the buttons.
  uint8_t debounceCounters[NUM_BUTTONS];

  /// Short press flags for the buttons.
  bool shortPressFlags[NUM_BUTTONS];  
  /// Long press flags for the buttons.
  bool longPressFlags[NUM_BUTTONS];
  ///  Long press flags for the buttons.
  bool longPressServed[NUM_BUTTONS];

  /// Button press start times.
  uint32_t pressStartTimes[NUM_BUTTONS];

  /*! Get the index of a button in the button array.
   * \param gpioPin The GPIO pin of the button.
   * \return The index of the button in the button array.
   */
  int getButtonIndex(uint8_t gpioPin);
};

#endif // _button_interpreter_h_
