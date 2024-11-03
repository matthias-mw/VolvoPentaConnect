// Doxygen Documentation
/*! \file 	display_data.h
 *  \brief  Show the data of all sensors to an LCD Panel
 *
 * This File contains all the necessary methods to display the
 * values of all sensors to an LCD Panel. Also the LCD Panel 
 * can give additional information to the user.
 *
 * \author 		Matthias Werner
 * \date		11/2024
 * - Prozessor:         ESP32-WROOM
 * - Hardware:          az-delivery-devkit-v4
 */
// USE DOXYGEN DOCUMENTATION Style for all Code
#ifndef _display_data_h_
#define _display_data_h_

#include <Arduino.h>
#include <hardwareDef.h>
#include <LiquidCrystal_PCF8574.h>
#include <acquire_data.h>
#include <version.h>  // Include the version header

/// \class DisplayData
/// \brief A class to manage the LCD display and show sensor data.
class DisplayData
{
public:
  /*!
   * \brief Constructor for DisplayData.
   * \param data Reference to an AcquireData object.
   */
  DisplayData(AcquireData &data);

  /************************************************************************//**
   * \brief Update the Content of all LCD Pages
   * 
   * This method updates all LCD pages with actual data. For saving time
   * and reducing glitches at the display, it has an option to update the 
   * "changing" data characters and not the "static" text.
   * 
   * @param page                number of lcd page to be updated
   * @param blnUpdateDataOnly   update data fields only (default = false)
   * 
   */
  void updateLCDPage(uint8_t page, boolean blnUpdateDataOnly = false);  

private:
  /// Reference to an AcquireData object with all the sensor data.
  AcquireData &data; 
};

#endif  // _display_data_h_