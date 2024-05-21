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
// #define DEBUG_TASK_STACK_SIZE

/// activate a certain Debuglevel (0 -> lowest, 4 -> Highest)
/// Comment out if not needed
#define DEBUG_LEVEL 1

// Definition of Simulated Test Data for TEsting without sensors
// #define USE_SIM_DATA

#define SIM_DATA_UBAT_ADC_PIN 11.7
#define SIM_DATA_MCP3201_CHN1 4.56
#define SIM_DATA_MCP3201_CHN2 1.7
#define SIM_DATA_MCP3201_CHN3 8.4
#define SIM_DATA_MCP3201_CHN4 3.9
#define SIM_DATA_EXHAUST 456
#define SIM_DATA_1WIRE_CH1 45.7
#define SIM_DATA_1WIRE_CH2 23.6
#define SIM_DATA_1WIRE_CH3 51.3
#define SIM_DATA_ENG_SPEED 2112
#define SIM_DATA_SHAFT_SPEED 358
#define SIM_DATA_ALT1_SPEED 1890
#define SIM_DATA_ALT2_SPEED 15000

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//      -----  generell Definitions ----
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/// GPIO where the LED is connected (Active High)
#define STATUS_LED_PIN 0
/// Turns on the Status LED Light
#define LED_PIN_ON 1
/// Turns off the Status LED Light
#define LED_PIN_OFF 0

/// LCD Panel value for full brightness
#define LCD_BACKLIGHT_FULL 255
/// LCD Panel value for backlight switched off
#define LCD_BACKLIGHT_OFF 0
/// LCD Panel counter for backlight switched off
#define LCD_BACKLIGHT_OFF_COUNT 2000UL // ToDo Real Value

/// GPIO where Button 1 is connected (Low Active)
#define BUTTON1_PIN 2
/// GPIO where Button 2 is connected (Low Active)
#define BUTTON2_PIN 12

/// Debounce value for the button inside of the interpretation task
#define BUTTON_DEBOUNCE 2
/// long press value for the button inside of the interpretation task
#define BUTTON_LONG_PRESS 15

/// GPIO where the WARNING is connected (Active High)
#define WARNING_OUTPUT_PIN 14

/// GPIO where the Relay Contact Output is connected (Active High)
#define RELAY_OUTPUT_PIN 4

/// GPIO where the Contact Input 1 is connected (Active Low)
#define CONTACT1_PIN 34
/// GPIO where the Contact Input 2 is connected (Active Low)
#define CONTACT2_PIN 35
/// GPIO where the Contact Input 3 is connected (Active Low)
#define CONTACT3_PIN 39

// ---------------> SPI Bus  <------------

/// GPIO where the TXB0104 is activated (Active High)
#define LT_ON_PIN 13

/// GPIO where the ChipSelect for Thermo is connected (Active Low)
#define NOT_CS_THERMO_PIN 27
/// GPIO where the ChipSelect for ADC is connected (Active Low)
#define NOT_CS_ADC_PIN 5

#define SPI_MISO_PIN 19
#define SPI_MOSI_PIN 23
#define SPI_CLK_PIN 18

// --------> OneWire <---------------------
/// GPIO where the DS18B20 is connected to
#define ONEWIRE_PIN 15
/// OneWire device address of the seawater outlet temperature sensor
#define ONEWIRE_ADR_SEAOUTLETWALL                  \
  {                                                \
    0x28, 0x75, 0x4C, 0x18, 0x0E, 0x00, 0x00, 0x75 \
  }
/// OneWire device address of the alternator temperature sensor
#define ONEWIRE_ADR_ALTERNATOR                     \
  {                                                \
    0x28, 0x67, 0x52, 0x17, 0x0E, 0x00, 0x00, 0x04 \
  }
/// OneWire device address of the gearbox temperature sensor
#define ONEWIRE_ADR_GEARBOX                        \
  {                                                \
    0x28, 0x67, 0x52, 0x17, 0x0E, 0x00, 0x00, 0x04 \
  }

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
// #define ADS1115_I2C_ADDRESS  0x48

// --------> NMEA2000 <---------------------
/// GPIO where the CAN-TX is connected
#define ESP32_CAN_TX_PIN (gpio_num_t)17
/// GPIO where the CAN-RX is connected
#define ESP32_CAN_RX_PIN (gpio_num_t)16

// --------> AD Converter <-----------------
/// Reference voltage for MCP3204
#define MCP3204_VREF (double)4.7

/// Voltage scaler for MCHP 3204 Channel 1
#define MCP3204_CH1_FAC (double)6.47706422018
/// Voltage scaler for MCHP 3204 Channel 2
#define MCP3204_CH2_FAC (double)5.51117318436
/// Voltage scaler for MCHP 3204 Channel 3
#define MCP3204_CH3_FAC (double)6.47706422018
/// Voltage scaler for MCHP 3204 Channel 4
#define MCP3204_CH4_FAC (double)5.51117318436

/// Messwert

#endif // _hardwareDef_H_
