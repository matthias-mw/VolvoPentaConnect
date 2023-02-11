// Doxygen Dokumentation
/*! \file 	measure_onewire.h
 *  \brief  Measuring all data from OneWire Devices
 *
 * All functions for using Onewire Sensors will be placed here.
 * Function will be available for:
 * - Listing all devices
 * - Configure DeviceNo to certain Data Channel
 * - Retrieving Data from Sensors
 * 
 * \author 		Matthias Werner
 * \date		02/2023
 *
 * - Prozessor:         ESP32-WROOM
 * - Hardware:          az-delivery-devkit-v4
**************************************************************/

#ifndef _MEASURE_ONEWIRE_H_
#define _MEASURE_ONEWIRE_H_

#include <Arduino.h>
#include <hardwareDef.h>
#include <OneWire.h>
#include <DallasTemperature.h>

/************************************************************************//**
 * \brief Listing of all OneWire devices at the Bus
 *
 * \param SerialStr {type} 
 */
void listOneWireDevices(HardwareSerial SerialStr);

void startDS18S20sensors ();


#endif //_MEASURE_ONEWIRE_H_