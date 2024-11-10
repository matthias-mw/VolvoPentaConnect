// Doxygen Documentation
/*! \file 	button_interpreter.h
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
#ifndef BUTTON_INTERPRETER_H
#define BUTTON_INTERPRETER_H

#include <Arduino.h>
#include <hardwareDef.h>
#include <display_data.h>
#include <process_warnings.h>

/// GPIO pins of the buttons. The order of the pins must
/// match the order of the buttons.
extern const uint8_t buttonPins[NUM_BUTTONS];

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
    * \param lcdDisplay Reference to a DisplayData object.
    * \param processWarnings Reference to a ProcessWarnings object.
   */
  ButtonInterpreter(DisplayData &lcdDisplay, ProcessWarnings &processWarnings);

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

  /*!
   * \brief Process the state of all buttons.
   *
   * This method checks the state of all buttons and triggers
   * the corresponding actions. These actions can be different
   * for all LCD pages.
   *
   * \param currentLcdPage The current LCD page.
   *
   */
  void processAllButtonState(uint8_t currentLcdPage);

private:
  /*!
   * \brief Check the state of a button.
   * \param gpioPin The GPIO pin of the button.
   * \return True if the button is pressed, false otherwise.
   */
  bool isButtonPressed(uint8_t gpioPin);

  /// Reference to the lcdDisplayData-object.
  DisplayData &lcdDisplayObject;

  /// Reference to the processWarnings-object.
  ProcessWarnings &processWarningsObject;

  /// Debounce counters for the buttons.
  uint8_t debounceCounters[NUM_BUTTONS];

  /// Short press flags for the buttons.
  bool shortPressFlags[NUM_BUTTONS];
  /// Long press flags for the buttons.
  bool longPressFlags[NUM_BUTTONS];
  ///  Long press flags are served for the buttons.
  bool longPressServed[NUM_BUTTONS];

  /// Button press start times.
  uint32_t pressStartTimes[NUM_BUTTONS];

  /*! Get the index of a button in the button array.
   * \param gpioPin The GPIO pin of the button.
   * \return The index of the button in the button array.
   */
  int getButtonIndex(uint8_t gpioPin);

  /*!
  * \brief Trigger Action for a specific button.

  * This method triggers the action for a specific button. The action
  * can be different for all LCD pages.
  *
  * \param buttonIndex The index of the button.
  * \param currentLcdPage The current LCD page.
  */
  void triggerButtonAction(uint8_t buttonIndex, uint8_t currentLcdPage);
};

#endif // BUTTON_INTERPRETER_H
