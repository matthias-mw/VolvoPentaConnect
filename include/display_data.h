// Doxygen Documentation
/*! \file 	display_data.h
 *  \brief  Show the data of all sensors to an LCD Panel
 *
 * This File contains all the necessary methods to display the
 * values of all sensors to an LCD Panel. Also the LCD Panel
 * can give additional information to the user.
 *
 * \author 		Matthias Werner
 * \date		November 2024
 * - Prozessor:         ESP32-WROOM
 * - Hardware:          az-delivery-devkit-v4
 */
#ifndef DISPLAY_DATA_H
#define DISPLAY_DATA_H

#include <Arduino.h>
#include <hardwareDef.h>
#include <LiquidCrystal_PCF8574.h>
#include <acquire_data.h>
#include <Wire.h>
#include <versionInfo.h>

/// Define max Number of Main Pages
#define MAX_MAIN_PAGES 6

/// Define the Number for the Welcome Page
#define WELCOME_PAGE 0
/// Define the Number for the Engine Page
#define PAGE_ENGINE 1
/// Define the Number for the Temperature Page
#define PAGE_TEMPERATURE 2
/// Define the Number for the Speed Page
#define PAGE_SPEED 3
/// Define the Number for the Alternator Page
#define PAGE_ALTERNATOR 4
/// Define the Number for the Voltage Page
#define PAGE_VOLTAGE 5
/// Define the Number for the Alarm Page
#define PAGE_ALARM 6

/// Define the Number for the 1Wire List Page
#define PAGE_1WIRE_LIST 10



/// Define max number LCD Update cycles until fullscreen renew
#define LCD_MAX_CYCLE_COUNT_TILL_FULL_UPDATE 20

/// Object for I2C Bus
extern TwoWire WireI2C;

/// Define the semaphore handle for the LCD Display Update
extern SemaphoreHandle_t xMutexLCDUpdate;

/// \class DisplayData
/// \brief A class to manage the LCD display and show sensor data.
class DisplayData
{
public:
  /*! ************************************************************************
   * \brief Constructor for DisplayData.
   * \param data Reference to an AcquireData object.
   */
  DisplayData(AcquireData &data);

  /*! ************************************************************************
   * \brief Setup the LCD Panel
   *
   * This method sets up the LCD Panel and initializes the display.
   *
   */
  void setupLCDPanel();

  /*! ************************************************************************
   * \brief Update the Content of all LCD Pages
   *
   * This method updates all LCD pages with actual data. For saving time
   * and reducing glitches at the display, it has an option to update the
   * "changing" data characters and not the "static" text.
   *
   * \param blnUpdateDataOnly   update data fields only (default = false)
   *
   */
  void updateLcdContent(boolean blnUpdateDataOnly = false);

  /*! ************************************************************************
   * \brief Set the current page for the LCD Panel
   *
   * This method sets number of the current page of the LCD Panel and
   * validates the given number.
   * \param page number of the current page
   */
  void setLcdCurrentPage(uint8_t page);

  /*! ************************************************************************
   * \brief Get the current page of the LCD Panel
   *
   * This method returns the number of the current page of the LCD Panel.
   *
   * \return number of the current page
   */
  uint8_t getLcdCurrentPage() { return lcdCurrentPage; }

  /*! ************************************************************************
   * \brief Increase the current page of the LCD Panel by One
   *
   * This method increases the number of the current page of the LCD Panel
   * by one and limits it to in between \ref WELCOME_PAGE +1  and
   * \ref MAX_MAIN_PAGES
   */
  void increaseLcdCurrentPage();

  /*! ************************************************************************
   * \brief Update the backlight of the LCD Panel
   * This method sets the backlight of the LCD Panel
   */
  void updateLcdBacklight();

  /*! ************************************************************************
   * \brief Reset the backlight Counter of the LCD Panel
   *
   * This method sets the backlight counter of the LCD Panel.
   */
  void resetLcdBacklightCounter() { lcdBacklightDimCounter = 0; }

private:
  /// Reference to an AcquireData object with all the sensor data.
  AcquireData &data;

  /// LCD 4x20 object on I2C Bus
  LiquidCrystal_PCF8574 lcd;
  /// ID for the active page on the LCD-Panel
  uint8_t lcdCurrentPage = 0;

  /// Buffer for Crystal  LCD Display 4x20 (4 lines a 20 char)
  char lcdDisplay[4][20];

  /// LCD Panel switched to a complete new screen
  bool lcdScreenRenew = true;

  /// LCD Panel Update counter is incremented every cycle of \ref taskUpdateLCD
  uint16_t lcdUpdateCounter = 0;

  /// LCD Panel dimmer counter is used to decrease backlight during no activity
  uint32_t lcdBacklightDimCounter = 0;

  /*! ************************************************************************
   * \brief Set the backlight of the LCD Panel to full brightness
   *
   * This method sets the backlight of the LCD Panel to full brightness
   * \ref LCD_BACKLIGHT_FULL
   *
   */
  void setBacklightFull();

  /*! ************************************************************************
   * \brief Set the backlight of the LCD Panel to off
   *
   * This method sets the backlight of the LCD Panel to off
   * \ref LCD_BACKLIGHT_OFF
   *
   */
  void setBacklightOff();
  
  /*! ************************************************************************
   * \brief Update the LCD Panel
   *
   * This method copys the buffer lcdDisplay to the LCD Panel. It is used
   * This method copies the buffer lcdDisplay to the LCD Panel. It is used
   *
   */
  void updateLCDPanel();
};

#endif  // DISPLAY_DATA_H