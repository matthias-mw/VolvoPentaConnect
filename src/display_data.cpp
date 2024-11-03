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

#include "display_data.h"

//****************************************
// Construct a new DisplayData object
DisplayData::DisplayData(AcquireData &data) : data(data)
{
} // DisplayData

//****************************************
// Update the content of all LCD pages
void DisplayData::updateLCDPage(uint8_t page, boolean blnUpdateDataOnly)
{
  char buffer[21];
  int length;

  switch (page)
  {
  case 0:
    // ------------------------------
    // Welcome Screen
    // ------------------------------
    // fill the screen buffer with permanent text
    length = sprintf(buffer, "VolvoPenta Connect");
    strncpy(&lcdDisplay[0][0], buffer, 20);

    length = sprintf(buffer, "--------------------");
    strncpy(&lcdDisplay[1][0], buffer, 20);

    length = sprintf(buffer, "  by Matthias Werner");
    strncpy(&lcdDisplay[2][0], buffer, 20);

    length = sprintf(buffer, "  Version %s", PROJECT_VERSION);
    strncpy(&lcdDisplay[3][0], buffer, 20);

    break;

  case 1:
    // ------------------------------
    // Engine Screen
    // ------------------------------
    if (!blnUpdateDataOnly)
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
  case 2:
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
    length = sprintf(buffer, "%5d%5d%5.1f%5d", (int16_t)this->data.tEngine.getValue(), (int16_t)this->data.tGearbox.getValue(), this->data.tSeaOutletWall.getValue(), (int16_t)this->data.tExhaust.getValue());
    strncpy(&lcdDisplay[3][0], buffer, 20);

    Serial.println(lcdDisplay[3]);

    break;

  case 3:
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

  case 4:
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

  case 5:
    // ------------------------------
    // Voltage Screen
    // ------------------------------
    if (!blnUpdateDataOnly)
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

  case 10:
    // ------------------------------
    // List DS18S20 Sensors Screen
    // ------------------------------
    uint8_t i;
    uint8_t deviceCount;
    DeviceAddress address;
    char buffer[20];

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
}
