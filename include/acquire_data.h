// Doxygen Documentation
/*! \file 	acquire_data.h
 *  \brief  Acquire data from all sensors
 *
 * This File contains all the necessary methods acquire the
 * data from all sensors.
 *
 * \author 		Matthias Werner
 * \date		02/2023
 *
 * - Prozessor:         ESP32-WROOM
 * - Hardware:          az-delivery-devkit-v4
 */

#ifndef _acquire_data_h_
#define _acquire_data_h_

#include <Arduino.h>
#include <hardwareDef.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <datapoint.h>
#include <ADS1115_WE.h>


// Setup a oneWire instance to communicate with any OneWire devices
extern OneWire oneWire;
// Pass our oneWire reference to Dallas Temperature sensor
extern DallasTemperature oneWireSensors;

extern ADS1115_WE objAD1115;

class tAcquireData
{

  const uint8_t _MAX_DATA_POINTS_HISTORY = 8;

public:
  tAcquireData();
  void show_data();
  
  

  /************************************************************************//**
   * \brief List all OneWire devices
   *
   *  Shows all OneWire Devices and there address.
   */
  void listOneWireDevices();

  /************************************************************************//**
   * \brief Measure all OneWire Sensors
   *
   * This Method measures all specified OneWire Sensors and stores the 
   * value in the corresponding Datapoint.
   */
  void measure_onewire();


  void measure_voltage();
  void _setUpADS1115(void);

private:

  // Define specific OneWire sensors
  DeviceAddress oWtCoolWall = {0x28, 0xE8, 0xF7, 0xAA, 0x03, 0x00, 0x00, 0x37};
  DeviceAddress oWtEngRoom = {0x28, 0xBF, 0xCA, 0xAA, 0x03, 0x00, 0x00, 0xF4};
  DeviceAddress oWtGearbox = {0x28, 0xBF, 0xCA, 0xAA, 0x03, 0x00, 0x00, 0xF4};

  tDataPoint tCoolWall = tDataPoint(senType_ds1820, "tCoolWall", "GrdC");
  tDataPoint tEngRoom = tDataPoint(senType_ds1820, "tEngRoom", "GrdC");
  tDataPoint tGearbox = tDataPoint(senType_ds1820, "tGearbox", "GrdC");
  tDataPoint uBat = tDataPoint(senType_ads1115, "Ubat", "V");





  /************************************************************************//**
   * \brief Stores the data into a Datapoint
   *
   * Stores the measured value of a signal into a datapoint.
   * 
   * \param db        Datapoint in which it should be stored
   * \param value     Measured Value
   * \param timestamp Timestamp in ms of the measurement
   */
  void _store_data(tDataPoint &db, double value, uint32_t timestamp);


  

};

#endif //_acquire_data_h_