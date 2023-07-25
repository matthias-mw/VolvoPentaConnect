// Doxygen Dokumentation
/*! \file 	process_n2k.h
 *  \brief  Processing all N2K Task on High Level
 *
 * This File contains all the necessary high level methods to handle the 
 * NMEA2000 connectivity and all Data from and to the bus.
 *
 * \author 		Matthias Werner
 * \date		02/2023
 *
 * - Prozessor:         ESP32-WROOM
 * - Hardware:          az-delivery-devkit-v4
*/


#ifndef _process_n2k_
#define _process_n2k_


#include <Arduino.h>
#include <hardwareDef.h>
#include <NMEA2000.h>
#include <N2kMessages.h>

// Publish the extern NMEA2000 Object from <NMEA2000_CAN.h>
extern tNMEA2000 &NMEA2000;

// List here messages your device will transmit.
const unsigned long TransmitMessages[] PROGMEM={127493L,127489L,127488L,130316L,0};

/************************************************************************//**
 * \struct  VolvoPentaData
 * \brief   Structure contains all data ready to be send to N2K bus
 *
 * The data in this struct has been collected and converted (units etc...)
 * to be directly used inside the N2K functions.
 */
typedef struct VolvoPentaData{

  /// engine coolant flow rate in l/h
  double engine_coolant_flow = 0;
  /// engine coolant temperature in kelvin
  double engine_coolant_temperature = 0;
  /// engine coolant temperature at the pipe in kelvin
  double engine_coolant_temperature_wall = 0;
  /// status that the coolant temperature has not exceeded limits
  bool flg_coolant_temperature_ok = false;
  /// engine oel pressure in Pascal
  double engine_oel_pressure = 0;
  /// status engine oel pressure is ok
  bool flg_engine_oel_pressure_ok = false;
  /// voltage of the starter batterie in volt
  double batterie_voltage = 0;
  /// engine room temperature in kelvin
  double engine_room_temperature = 0;
  /// exhaust gat temperature in kelvin
  double exhaust_temperature = 0;
  /// gearbox temperature in kelvin
  double gearbox_temperature = 0;
  /// engine speed in rev per minute
  double engine_speed = 0;
  /// prop shaft speed in rev per minute
  double shaft_speed = 0;
  /// alternator 1 speed in rev per minute
  double alternator1_speed = 0;
  /// alternator 2 speed in rev per minute
  double alternator2_speed = 0;
  /// engine hours run in seconds
  uint16_t engine_hours = 0;

}tVolvoPentaData;




void setupN2K();


void SendN2kEngineParm(VolvoPentaData data);

#endif //_process_n2k_