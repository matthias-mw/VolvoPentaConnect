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
//      -----  DEBUG DEFINES ----
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/// activate debug of free stacksize of the tasks
#define DEBUG_TASK_STACK_SIZE



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

#define SPI_MISO_PIN  19
#define SPI_MOSI_PIN  23
#define SPI_CLK_PIN   18

// --------> OneWire <---------------------
/// GPIO where the DS18B20 is connected to
#define ONEWIRE_PIN 15
/// OneWire device address of the seawater outlet temperature sensor
#define ONEWIRE_ADR_SEAOUTLETWALL { 0x28, 0x75, 0x4C, 0x18, 0x0E, 0x00, 0x00, 0x75}
/// OneWire device address of the alternator temperature sensor
#define ONEWIRE_ADR_ALTERNATOR {0x28, 0x67, 0x52, 0x17, 0x0E, 0x00, 0x00, 0x04}
/// OneWire device address of the gearbox temperature sensor
#define ONEWIRE_ADR_GEARBOX {0x28, 0x67, 0x52, 0x17, 0x0E, 0x00, 0x00, 0x04}

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
#define ESP32_CAN_TX_PIN (gpio_num_t)17
/// GPIO where the CAN-RX is connected 
#define ESP32_CAN_RX_PIN (gpio_num_t)16

// --------> AD Converter <-----------------
/// Reference voltage for MCP3204
#define MCP3204_VREF (double)4.7

/// Voltage scaler for MCHP 3204 Channel 1
#define MCP3204_CH1_FAC (double)2.488
/// Voltage scaler for MCHP 3204 Channel 2
#define MCP3204_CH2_FAC (double)2.488
/// Voltage scaler for MCHP 3204 Channel 3
#define MCP3204_CH3_FAC (double)16.024
/// Voltage scaler for MCHP 3204 Channel 4
#define MCP3204_CH4_FAC (double)16.024


#endif // _hardwareDef_H_
