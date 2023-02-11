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


void setupN2K();


void SendN2kEngineParm();

#endif //_process_n2k_