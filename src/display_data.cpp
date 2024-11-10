// Doxygen Documentation
/*! \file 	display_data.cpp
 *  \brief  Show the data of all sensors to an LCD Panel
 *
 * This File contains all the necessary methods to display the
 * values of all sensors to an LCD Panel. Also the LCD Panel
 * can give additional information to the user.
 *
 * \author 		Matthias Werner
 * \date		11/2024
 *
 * - Prozessor:         ESP32-WROOM
 * - Hardware:          az-delivery-devkit-v4
 */

#include <display_data.h>

/// Object for I2C Bus
TwoWire WireI2C(0x3f);

/// Define the semaphore handle for the LCD Display Update
SemaphoreHandle_t xMutexLCDUpdate = NULL;

//****************************************
// Construct a new DisplayData object
DisplayData::DisplayData(AcquireData &data) : data(data), lcd(0x3F)
{
}

// Setup the LCD Panel
void DisplayData::setupLCDPanel()
{
  // Create the mutex before using it
  xMutexLCDUpdate = xSemaphoreCreateMutex();

  WireI2C.begin(21, 22);   // custom i2c port on ESP
  WireI2C.setClock(80000); // set 80kHz (PCF8574 max speed 100kHz)

  this->lcd.begin(20, 4, WireI2C);
  this->lcd.setBacklight(255);
} // setupLCDPanel

//****************************************
// Increase the current page of the LCD Panel by One
void DisplayData::increaseLcdCurrentPage()
{
  lcdCurrentPage++;
  // limit the page number
  if (lcdCurrentPage > MAX_MAIN_PAGES)
  {
    setLcdCurrentPage(WELCOME_PAGE + 1);
  }
  lcdScreenRenew = true;   // reload the screen
  updateLcdContent(false); // update the content completely

} // increaseLcdCurrentPage

//****************************************
// Set the current page for the LCD Panel
void DisplayData::setLcdCurrentPage(uint8_t page)
{
  lcdCurrentPage = page;
  lcdScreenRenew = true;   // reload the screen
  updateLcdContent(false); // update the content completely

} // setLcdCurrentPage

//****************************************
// Update the backlight of the LCD Panel
void DisplayData::updateLcdBacklight()
{
  // Set the backlight of the LCD Panel
  if (lcdBacklightDimCounter < LCD_BACKLIGHT_OFF_COUNT)
  {
    // dimmed brightness to the LCD Panel
    setBacklightFull();
  }
  else
  {
    // LCD Panel backlight is off
    setBacklightOff();
  }
  // increase counter
  lcdBacklightDimCounter++;
} // updateLcdBacklight

//****************************************
// Set the backlight of the LCD Panel to full brightness
void DisplayData::setBacklightFull()
{
  this->lcd.setBacklight(LCD_BACKLIGHT_FULL);
} // setBacklightFull

//****************************************
// Set the backlight of the LCD Panel to off
void DisplayData::setBacklightOff()
{
  this->lcd.setBacklight(LCD_BACKLIGHT_OFF);
} // setBacklightOff

//****************************************
// Update the content of all LCD pages
void DisplayData::updateLcdContent(boolean blnUpdateDataOnly)
{
  char buffer[21];
  int length;

  // count the number of alarms
  uint8_t alarmCount = this->data.getActiveWarningCount();
  // temporary buffer for alarmtext
  char bufferAlarmTxt[21];
  // number of alarm line
  uint8_t alarmLine = 0;

  // when there are alarms, then switch to the alarm page
  if (alarmCount > 0)
  {
    lcdCurrentPage = (uint8_t)PAGE_ALARM;
  }

  // Edit the content of the LCD Panel
  switch (lcdCurrentPage)
  {
  case WELCOME_PAGE:
    // ------------------------------
    // Welcome Screen
    // ------------------------------
    // fill the screen buffer with permanent text
    length = sprintf(buffer, "VolvoPenta Connect");
    strncpy(&lcdDisplay[0][0], buffer, 20);

    length = sprintf(buffer, "---  Tatooine  ---");
    strncpy(&lcdDisplay[1][0], buffer, 20);

    length = sprintf(buffer, "by Matthias Werner  ");
    strncpy(&lcdDisplay[2][0], buffer, 20);

    length = sprintf(buffer, "      Version %6s", PROJECT_VERSION);
    strncpy(&lcdDisplay[3][0], buffer, 20);

    break;

  case PAGE_ENGINE:
    // ------------------------------
    // Engine Screen
    // ------------------------------
    if (!blnUpdateDataOnly || (lcdUpdateCounter > LCD_MAX_CYCLE_COUNT_TILL_FULL_UPDATE))
    { // fill the screen buffer with permanent text
      length = sprintf(buffer, "Engine Data  %6.1fh", this->data.engSecond.getValue() / 3600);
      strncpy(&lcdDisplay[0][0], buffer, 20);

      length = sprintf(buffer, "--------------------");
      strncpy(&lcdDisplay[1][0], buffer, 20);

      length = sprintf(buffer, "  rpm GrdC  bar GrdC");
      strncpy(&lcdDisplay[2][0], buffer, 20);
    }

    // fill buffer with data
    length = sprintf(buffer, "%5d%5d%5.1f%5d", (uint16_t)this->data.nMot.getValue(), (uint16_t)this->data.tEngine.getValue(), this->data.pOil.getValue(), (int16_t)this->data.tExhaust.getValue());
    strncpy(&lcdDisplay[3][0], buffer, 20);

    break;
  case PAGE_TEMPERATURE:
    // ------------------------------
    // Temperature Screen
    // ------------------------------
    if (!blnUpdateDataOnly)
    { // fill the screen buffer with permanent text
      length = sprintf(buffer, "Temperature   [GrdC]");
      strncpy(&lcdDisplay[0][0], buffer, 20);

      length = sprintf(buffer, "--------------------");
      strncpy(&lcdDisplay[1][0], buffer, 20);

      length = sprintf(buffer, "  Eng Gear  Sea  Exh");
      strncpy(&lcdDisplay[2][0], buffer, 20);
    }

    // fill buffer with data
    length = sprintf(buffer, "%5d%5d%4.1f%5d", (int16_t)this->data.tEngine.getValue(), (int16_t)this->data.tGearbox.getValue(), this->data.tSeaOutletWall.getValue(), (int16_t)this->data.tExhaust.getValue());
    strncpy(&lcdDisplay[3][0], buffer, 20);

    break;

  case PAGE_SPEED:
    // ------------------------------
    // Speed Screen
    // ------------------------------
    if (!blnUpdateDataOnly)
    { // fill the screen buffer with permanent text
      length = sprintf(buffer, "Speed        [U/min]");
      strncpy(&lcdDisplay[0][0], buffer, 20);

      length = sprintf(buffer, "--------------------");
      strncpy(&lcdDisplay[1][0], buffer, 20);

      length = sprintf(buffer, "  Eng Gear Alt1 Alt2");
      strncpy(&lcdDisplay[2][0], buffer, 20);
    }

    // fill buffer with data
    length = sprintf(buffer, "%5d%5d%5d%5d", (uint16_t)this->data.nMot.getValue(), (uint16_t)this->data.nShaft.getValue(), (uint16_t)this->data.nAlternator1.getValue(), (uint16_t)this->data.nAlternator2.getValue());
    strncpy(&lcdDisplay[3][0], buffer, 20);

    break;

  case PAGE_ALTERNATOR:
    // ------------------------------
    // Alternator Screen
    // ------------------------------
    if (!blnUpdateDataOnly)
    { // fill the screen buffer with permanent text
      length = sprintf(buffer, "Alternator  Data    ");
      strncpy(&lcdDisplay[0][0], buffer, 20);

      length = sprintf(buffer, "--------------------");
      strncpy(&lcdDisplay[1][0], buffer, 20);

      length = sprintf(buffer, "  rpm GrdC  rpm    V");
      strncpy(&lcdDisplay[2][0], buffer, 20);
    }

    // fill buffer with data
    length = sprintf(buffer, "%5d%5d%5d%5.1f", (uint16_t)this->data.nAlternator1.getValue(), (int16_t)this->data.tAlternator.getValue(), (uint16_t)this->data.nAlternator2.getValue(), this->data.uBat.getValue());
    strncpy(&lcdDisplay[3][0], buffer, 20);

    break;

  case PAGE_VOLTAGE:
    // ------------------------------
    // Voltage Screen
    // ------------------------------
    if (!blnUpdateDataOnly || (lcdUpdateCounter > LCD_MAX_CYCLE_COUNT_TILL_FULL_UPDATE))
    { // fill the screen buffer with permanent text
      length = sprintf(buffer, "MCP3204       %5.2fV", this->data.uBat.getValue());
      strncpy(&lcdDisplay[0][0], buffer, 20);

      length = sprintf(buffer, "--------------------");
      strncpy(&lcdDisplay[1][0], buffer, 20);

      length = sprintf(buffer, "    V    V    V    V");
      strncpy(&lcdDisplay[2][0], buffer, 20);
    }

    // fill buffer with data
    length = sprintf(buffer, "%5.1f%5.1f%5.1f%5.1f", this->data.uMcp3204Ch1.getValue(), this->data.uMcp3204Ch2.getValue(), this->data.uMcp3204Ch3.getValue(), this->data.uMcp3204Ch4.getValue());
    strncpy(&lcdDisplay[3][0], buffer, 20);

    break;

  case PAGE_1WIRE_LIST:
    // ------------------------------
    // List DS18S20 Sensors Screen
    // ------------------------------
    uint8_t i;
    uint8_t deviceCount;
    DeviceAddress address;

    // locate devices on the bus
    deviceCount = oneWireSensors.getDeviceCount();
    length = sprintf(buffer, "Found %1d DS18S20.    ", deviceCount);
    strncpy(&lcdDisplay[0][0], buffer, 20);

    // Print Adresses to LCD Display
    for (int i = 0; i < 3; i++)
    {
      if (i >= deviceCount)
      {
        // clear line
        length = sprintf(buffer, "                    ");
        strncpy(&lcdDisplay[i + 1][0], buffer, 20);
      }
      else
      {
        // show the address
        oneWireSensors.getAddress(address, i);
        length = sprintf(buffer, "> 0x%02x%02x%02x%02x%02x%02x%02x%02x",
                         address[0], address[1], address[2], address[3], address[4],
                         address[5], address[6], address[7]);
        strncpy(&lcdDisplay[i + 1][0], buffer, 20);
      }
    }
    break;

  case PAGE_ALARM:
    // ------------------------------
    // Alarm Screen
    // ------------------------------

    // renew the screen always when the page is selected
    lcdScreenRenew = true;

    // count the number of alarms
    if (alarmCount == 0)
    {
      // fill the screen buffer with permanent text
      length = sprintf(buffer, "--- No Alarms ---   ");
      strncpy(&lcdDisplay[0][0], buffer, 20);

      length = sprintf(buffer, "                    ");
      strncpy(&lcdDisplay[1][0], buffer, 20);

      length = sprintf(buffer, "                    ");
      strncpy(&lcdDisplay[2][0], buffer, 20);

      length = sprintf(buffer, "                    ");
      strncpy(&lcdDisplay[3][0], buffer, 20);
    }
    else
    {
      // fill the screen buffer with permanent text
      length = sprintf(buffer, " Alarms Active !!! %d", alarmCount);
      strncpy(&lcdDisplay[0][0], buffer, 20);

      length = sprintf(buffer, "--------------------");
      strncpy(&lcdDisplay[1][0], buffer, 20);

      // create alarm text cooresponding to the active alarms
      if (this->data.currentEngineDiscreteStatus.flgHighCoolantTemp.isFlagSet())
      {
        length = sprintf(bufferAlarmTxt, "Overtemp Coolant    ");
        strncpy(&lcdDisplay[2 + alarmLine][0], bufferAlarmTxt, 20);
        alarmLine++;
      }
      if (this->data.currentEngineDiscreteStatus.flgLowOilPressure.isFlagSet())
      {
        length = sprintf(bufferAlarmTxt, "Low Oil Pressure    ");
        strncpy(&lcdDisplay[2 + alarmLine][0], bufferAlarmTxt, 20);
        alarmLine++;
      }
      if (this->data.currentEngineDiscreteStatus.flgHighExhaustTemp.isFlagSet() && (alarmCount <= 2))
      {
        length = sprintf(bufferAlarmTxt, "High Exhaust Temp   ");
        strncpy(&lcdDisplay[2 + alarmLine][0], bufferAlarmTxt, 20);
        alarmLine++;
      }
      if (this->data.currentEngineDiscreteStatus.flgHighGearboxTemp.isFlagSet() && (alarmCount <= 2))
      {
        length = sprintf(bufferAlarmTxt, "High Gearbox Temp   ");
        strncpy(&lcdDisplay[2 + alarmLine][0], bufferAlarmTxt, 20);
        alarmLine++;
      }
      if (this->data.currentEngineDiscreteStatus.flgHighAlternatorTemp.isFlagSet() && (alarmCount <= 2))
      {
        length = sprintf(bufferAlarmTxt, "High Alternator Temp");
        strncpy(&lcdDisplay[2 + alarmLine][0], bufferAlarmTxt, 20);
        alarmLine++;
      }
      if (this->data.currentEngineDiscreteStatus.flgHighSeaWaterTemp.isFlagSet() && (alarmCount <= 2))
      {
        length = sprintf(bufferAlarmTxt, "High Seawater InTemp ");
        strncpy(&lcdDisplay[2 + alarmLine][0], bufferAlarmTxt, 20);
        alarmLine++;
      }

      // more then 2 alarms
      if (alarmCount > 2)
      {
        length = sprintf(buffer, "  %1d more Alarms     ", alarmCount - 1);
        strncpy(&lcdDisplay[3][0], buffer, 20);
      }

      // when just 1 alarm is active
      if (alarmCount == 1)
      {
        length = sprintf(buffer, "                    ");
        strncpy(&lcdDisplay[3][0], buffer, 20);
      }
    }
    break;

  default:

    // ------------------------------
    // Error Screen
    // ------------------------------
    // fill the screen buffer with permanent text
    length = sprintf(buffer, "something went wrong");
    strncpy(&lcdDisplay[0][0], buffer, 20);

    length = sprintf(buffer, "--------------------");
    strncpy(&lcdDisplay[1][0], buffer, 20);

    length = sprintf(buffer, "  by Matthias Werner");
    strncpy(&lcdDisplay[2][0], buffer, 20);

    length = sprintf(buffer, "                    ");
    strncpy(&lcdDisplay[3][0], buffer, 20);

  }; // switch

  // update the LCD Panel
  updateLCDPanel();
}

//****************************************
// Update the LCD Panel
void DisplayData::updateLCDPanel()
{

  // Use the xMutexLCDUpdate to protect the LCD Panel update
  if (xSemaphoreTake(xMutexLCDUpdate, (TickType_t)pdMS_TO_TICKS(100)) == pdTRUE)
  { // Mutex is taken

    char lcdbuf[21];
    memset(lcdbuf, ' ', 20);
    lcdbuf[20] = '\0';
    // check if a full update is required
    bool fullUpdateRequired = (lcdUpdateCounter > LCD_MAX_CYCLE_COUNT_TILL_FULL_UPDATE);

    if (lcdScreenRenew || fullUpdateRequired)
    {

      // Update row 1
      this->lcd.setCursor(0, 0);
      strncpy(lcdbuf, &lcdDisplay[0][0], 20);
      lcdbuf[20] = '\0';
      this->lcd.print(lcdbuf); // print the line to screen

      // Update row 2
      this->lcd.setCursor(0, 1);
      strncpy(lcdbuf, &lcdDisplay[1][0], 20);
      lcdbuf[20] = '\0';
      this->lcd.print(lcdbuf); // print the line to screen

      // Update row 3
      this->lcd.setCursor(0, 2);
      strncpy(lcdbuf, &lcdDisplay[2][0], 20);
      lcdbuf[20] = '\0';
      this->lcd.print(lcdbuf); // print the line to screen

      // reset count
      lcdUpdateCounter = 0;
      lcdScreenRenew = false;
    }
    // Update measured values more often for good response
    // Update row 4
    this->lcd.setCursor(0, 3);
    strncpy(lcdbuf, &lcdDisplay[3][0], 20);
    lcdbuf[20] = '\0';
    this->lcd.print(lcdbuf); // print the line to screen

    // increase LCD Update counter
    lcdUpdateCounter++;

    // Give back the Mutex
    xSemaphoreGive(xMutexLCDUpdate);

  } // if

} // updateLCDPanel
