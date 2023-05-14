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


/// GPIO where the LED is connected (Active High)
#define STATUS_LED_PIN  0

/// GPIO where Button 1 is connected (Low Active)
#define BUTTON1_PIN  2
/// GPIO where Button 2 is connected (Low Active)
#define BUTTON2_PIN  12

/// GPIO where the WARNING is connected (Active High)
#define WARNING_OUTPUT_PIN  14

/// GPIO where the Relay Contact Output is connected (Active High)
#define RELAY_OUTPUT_PIN  4

/// GPIO where the Contact Input 1 is connected (Active Low)
#define CONTACT1_PIN  34
/// GPIO where the Contact Input 2 is connected (Active Low)
#define CONTACT2_PIN  35
/// GPIO where the Contact Input 3 is connected (Active Low)
#define CONTACT3_PIN  39


// ---------------> SPI Bus  <------------

/// GPIO where the TXB0104 is activated (Active High)
#define LT_ON_PIN 13

/// GPIO where the ChipSelect for Thermo is connected (Active Low)
#define NOT_CS_THERMO_PIN 27
/// GPIO where the ChipSelect for ADC is connected (Active Low)
#define NOT_CS_ADC_PIN 5

#define MAX6675_DO_PIN 19

#define MAX6675_CLK_PIN 18


// --------> OneWire <---------------------
/// GPIO where the DS18B20 is connected to
#define ONEWIRE_PIN 13

// --------> Analog PINs <-----------------
/// Analog Channel for Battery Voltage
#define UBAT_ADC_PIN 36

// ---------> Speed Counter <--------------

#define TIMER_PRESCALER_FOR_1NS 80
/// GPIO where the Engine Speed Signal is connected (Counter)
#define ENGINE_RPM_PIN 25
/// GPIO where the SHAFT Speed Signal is connected (Counter)
#define SHAFT_RPM_PIN 26
/// GPIO where the Alternator1 Speed Signal is connected (Counter)
#define ALTERNATOR1_RPM_PIN 32
/// GPIO where the Alternator2 Speed Signal is connected (Counter)
#define ALTERNATOR2_RPM_PIN 33

// ------------> I2C <---------------------
//#define ADS1115_I2C_ADDRESS  0x48


// --------> NMEA2000 <---------------------
/// GPIO where the CAN-TX is connected 
#define ESP32_CAN_TX_PIN 17
/// GPIO where the CAN-RX is connected 
#define ESP32_CAN_RX_PIN 16




#endif // _hardwareDef_H_
