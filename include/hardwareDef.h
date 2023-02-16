// Doxygen Dokumentation
/*! \file 	hardwareDef.h
 *  \brief  Definition of Hardware Interfaces
 *  
 *  Test
 *
 * \author 		Matthias Werner
 * \date		10 Sep 2022
 *
 * - Hardware:          az-delivery-devkit-v4
 * - Prozessor:         ESP32-WROOM
 *
 * ************************************************************
 * \version
 *	1.0		
 *
 *************************************************************/

#ifndef _hardwareDef_H_
#define _hardwareDef_H_

#include <Arduino.h>


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//      -----  generell Definitions ----
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// GPIO where the LED ist connected to
#define STATUS_LED  32

// GPIO where the LED ist connected to
#define BUTTON1  23


// --------> OneWire <---------------------
// GPIO where the DS18B20 is connected to
#define ONEWIRE_PIN 27

// ------------> I2C <---------------------
#define ADS1115_I2C_ADDRESS  0x48


// --------> NMEA2000 <---------------------
#define ESP32_CAN_TX_PIN GPIO_NUM_16
#define ESP32_CAN_RX_PIN GPIO_NUM_4




#endif // _hardwareDef_H_
