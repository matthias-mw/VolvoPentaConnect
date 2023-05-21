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
const unsigned long TransmitMessages[] PROGMEM={130310L,130311L,130312L,0};



struct VolvoPentaData{

  double fuel_flow = 10;
  
  double coolant_temperature = 124;

  // Kelvin
  double coolant_temperature_wall = 124;

  bool coolant_temperature_ok = true;

  // kPa * 100
  double oel_pressure = 250000;

  bool oel_pressure_ok = false;
  
  double batterie_voltage = 11.88;
  
  double engine_room_temperature = 35;

  double exhaust_temperature = 456;

  double gearbox_temperature = 100;

  double engine_speed = 1358;

  double shaft_speed = 1000;

  double alternator1_speed = 1234;

  double alternator2_speed = 3456;

  uint16_t engine_hours = 1234;



};




void setupN2K();


void SendN2kEngineParm(VolvoPentaData data);

#endif //_process_n2k_