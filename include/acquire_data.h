// Doxygen Documentation
/*! \file 	acquire_data.h
 *  \brief  Acquire data from all sensors
 *
 * This File contains all the necessary methods to acquire the
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

#include <adc_calib.h>
#include <datapoint.h>
#include <max6675.h>
#include <MCP_ADC.h>
#include "process_n2k.h"

/** oneWire instance to communicate with any OneWire devices */
extern OneWire oneWire;
/** Object for our oneWire Dallas Temperature sensors */
extern DallasTemperature oneWireSensors;
/// Mutex for protecting data integrity of \ref VolvoDataForN2k
extern SemaphoreHandle_t xMutexVolvoN2kData;

// ------------------------------------------------------------------
// variables for engine speed calculation
// ------------------------------------------------------------------

/***********************************************************************//**
 * \struct tSpeedCalc
 * \brief Structure that handles all values to determine the time difference
 * in between of two interrupts *
 */
typedef struct speedCalc{
    /** Start value for tick counts in between two interrupt events on GPIO pin.  */
  volatile uint64_t StartValue = 0;                   
  /** Period duration in between two interrupt events of the  GPIO pin. 
   * As the corresponding timer is set with a prescaler, one count 
   * represents 1ns  */
  volatile uint64_t PeriodCount = 0;                  
  /** Timestamp for the last interrupt   */
  volatile unsigned long TimestampLastInt = 0;           
  /** Pointer for the timer of of the signal */
  hw_timer_t *Timer = NULL;                           
  /** mux to lock/unlock engine speed timer interrupt 
   * \sa https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos_idf.html?highlight=portmux_type
  */
  portMUX_TYPE muxTMR = portMUX_INITIALIZER_UNLOCKED;

} tSpeedCalc;



/************************************************************************//**
 * \brief Handle the interrupt triggered by the engine speed
 *
 * This method is called by the interrupt of the engine speed pin and 
 * determins the duration in between 2 events.
 */
void IRAM_ATTR handleEngineSpeedInterrupt();

/************************************************************************//**
 * \brief Handle the interrupt triggered by the shaft speed
 *
 * This method is called by the interrupt of the shaft speed pin and 
 * determins the duration in between 2 events.
 */
void IRAM_ATTR handleShaftSpeedInterrupt();

/************************************************************************//**
 * \brief Handle the interrupt triggered by the alternator 1 speed
 *
 * This method is called by the interrupt of the alternator 1  speed pin and 
 * determins the duration in between 2 events.
 */
void IRAM_ATTR handleAlternator1SpeedInterrupt();

/************************************************************************//**
 * \brief Handle the interrupt triggered by the alternator 2 speed
 *
 * This method is called by the interrupt of the alternator 2  speed pin and 
 * determins the duration in between 2 events.
 */
void IRAM_ATTR handleAlternator2SpeedInterrupt();

class AcquireData
{
  /** max depth of the data history of each data point*/
  const uint8_t _MAX_DATA_POINTS_HISTORY = 8;

public:

  /************************************************************************//**
   * \brief Construct a new AcquireDataobject
   *
   */
  AcquireData();


  /************************************************************************//**
   * \brief Displays all data to the terminal 
   *
   */
  void showDataOnTerminal();
  
  /************************************************************************//**
   * \brief List all OneWire devices
   *
   *  Shows all OneWire Devices and there address.
   */
  void listOneWireDevices();

  /************************************************************************//**
   * \brief Measure all OneWire Sensors
   *
   * This method measures all specified OneWire Sensors and stores the 
   * value in the corresponding datapoint.
   * 
   * \note measuring data with the onewire sensor causes a delay of 0.5s 
   *       per sensor 
   */
  void measureOnewire();


  /************************************************************************//**
   * \brief  Measure all the voltages
   *
   * This method measures all voltages and stores the value in the 
   * corresponding datapoint.
   */
  void measureVoltage();

  /************************************************************************//**
   * \brief  Measure all Speeds
   *
   * This method measures all rotational speeds and stores the value in the 
   * corresponding datapoint.
   */
  void measureSpeed();

  /************************************************************************//**
   * \brief  Measure Exhaust Temperature
   *
   * This method measures the exhaust temperature while using an NiCr-Ni
   * thermocouple and the MAX6675 Chip
   */
  void measureExhaustTemperature();

  /************************************************************************//**
   * \brief  Checking the states of the contact
   *
   * This method checks the state of all 3 contacts
   */
  void checkContacts();

  /************************************************************************//**
   * \brief  Setup all the measurement channels
   *
   * This method handles all setup functions necessary for the config of
   * all measurements channels.
   */
  void setUpMeasurementChannels();

  /************************************************************************//**
   * \brief  Convert all measured data into N2kData formats
   *
   * This method converts all data measured with several sensors into data
   * which fits to the N2k standard and units.
   * 
   * \param data {type} 
   */
  void convertDataToN2k(tVolvoPentaData * data);



  /** Structure with all timer values for the engine speed calculation*/
  tSpeedCalc engSpeedCalc;
 
   /** Structure with all timer values for the shaft speed calculation*/
  tSpeedCalc shaftSpeedCalc;

  /** Structure with all timer values for the alternator 1 speed calculation*/
  tSpeedCalc alternator1SpeedCalc;

  /** Structure with all timer values for the alternator 2 speed calculation*/
  tSpeedCalc alternator2SpeedCalc;

private:

  /** Object of the NiCr-Ni Thermocouple that handles all functions */
  MAX6675 thermoNiCr_Ni = MAX6675(SPI_CLK_PIN, NOT_CS_THERMO_PIN, SPI_MISO_PIN);

  /** Object of the Microchip 3204 10bit AD-Converter that handles all 
   * functions */
  MCP3204 mcp3204 = MCP3204(SPI_MISO_PIN, SPI_MOSI_PIN, SPI_CLK_PIN);  

  /// OneWire Sensor Address for cooling system
  DeviceAddress oWtSeaOutletWall = ONEWIRE_ADR_SEAOUTLETWALL;
  /// OneWire Sensor Address for engine room system
  DeviceAddress oWtAlternator = ONEWIRE_ADR_ALTERNATOR;
  /// OneWire Sensor Address for gearbox system
  DeviceAddress oWtGearbox = ONEWIRE_ADR_GEARBOX;

  /** Temperature of the cooling system measure via a ds1820 sensor mounted 
      at the wall of a cooling pipe*/
  tDataPoint tSeaOutletWall = tDataPoint(senType_ds1820, "tSeaOutletWall", "GrdC");
  /** Temperature of the balmar alternator measured via a ds1820 sensor*/
  tDataPoint tAlternator = tDataPoint(senType_ds1820, "tAlternator", "GrdC");
  /** Temperature of the gearbox measured via a ds1820 sensor mounted 
      at the wall of the transmission */
  tDataPoint tGearbox = tDataPoint(senType_ds1820, "tGearbox", "GrdC");
  /** Batterie voltage measured at the main power source*/
  tDataPoint uBat = tDataPoint(senType_adc, "uBat", "V");
  /** Engine speed measured via an hall sensor at the crankshaft*/
  tDataPoint nMot = tDataPoint(senType_RPM, "nMot", "rpm");
  /** Shaft speed measured via an hall sensor at the pro shaft */
  tDataPoint nShaft = tDataPoint(senType_RPM, "nShaft", "rpm");
  /** Alternator 1 speed measured via the W signal of the alternator*/
  tDataPoint nAlternator1 = tDataPoint(senType_RPM, "nAlternator1", "rpm");
  /** Alternator 2 speed measured via the W signal of the alternator*/
  tDataPoint nAlternator2 = tDataPoint(senType_RPM, "nAlternator2", "rpm");
  /** Exhaust gas temperature measured via a NiCr-Ni thermocouple */
  tDataPoint tExhaust = tDataPoint(senType_max6675, "tExhaust", "GrdC");
  /** Voltage measure via an mcp3204 an AD Channel 1*/
  tDataPoint uMcp3204Ch1 = tDataPoint(senType_adc, "uMcp3204Ch1", "V");
  /** Voltage measure via an mcp3204 an AD Channel 2*/
  tDataPoint uMcp3204Ch2 = tDataPoint(senType_adc, "uMcp3204Ch2", "V");
  /** Voltage measure via an mcp3204 an AD Channel 3*/
  tDataPoint uMcp3204Ch3 = tDataPoint(senType_adc, "uMcp3204Ch3", "V");
  /** Voltage measure via an mcp3204 an AD Channel 4*/
  tDataPoint uMcp3204Ch4 = tDataPoint(senType_adc, "uMcp3204Ch4", "V");
  /** State of Contact 1 */
  tDataPoint flgContact1 = tDataPoint(senType_GPIO, "flgContact1", "-");
  /** State of Contact 2 */
  tDataPoint flgContact2 = tDataPoint(senType_GPIO, "flgContact2", "-");
  /** State of Contact 3 */
  tDataPoint flgContact3 = tDataPoint(senType_GPIO, "flgContact3", "-");

  /************************************************************************//**
   * \brief Stores the data into a Datapoint
   *
   * Stores the measured value of a signal into a datapoint.
   * 
   * \param db        Datapoint in which it should be stored
   * \param value     Measured Value
   * \param timestamp Timestamp in ms of the measurement
   */
  void _StoreData(tDataPoint &db, double value, uint32_t timestamp);

  /************************************************************************//**
   * \brief  Calculate Revolutions per Minute
   * 
   * This method calculates the revolutions per minute out of the measured
   * timer values
   * 
   * \param tmrValues Pointer to timer values \ref tSpeedCalc
   * 
   * \return res [1/min]
   *
   */
  double _calcNumberOfRevs(tSpeedCalc *tmrValues);

    /************************************************************************//**
   * \brief Set up the Engine speed timer and interrupt
   *
   * The engine speed calculation is using the timer0 and and interrupt. With
   * the timer0 prescaler ( \ref TIMER_PRESCALER_FOR_1NS) adjusted, that one tick 
   * is 1ns.
   * The GPIO \ref ENGINE_RPM_PIN is attached as external interrupt.
   * 
   */
  void _setUpEngineSpeedInt();

  /************************************************************************//**
   * \brief Set up the Shaft speed timer and interrupt
   *
   * The engine speed calculation is using the timer0 and and interrupt. With
   * the timer0 prescaler ( \ref TIMER_PRESCALER_FOR_1NS) adjusted, that one tick 
   * is 1ns.
   * The GPIO \ref SHAFT_RPM_PIN is attached as external interrupt.
   * 
   */
  void _setUpShaftSpeedInt();

  /************************************************************************//**
   * \brief Set up the Alternator1 speed timer and interrupt
   *
   * The engine speed calculation is using the timer1 and and interrupt. With
   * the timer0 prescaler ( \ref TIMER_PRESCALER_FOR_1NS) adjusted, that one tick 
   * is 1ns.
   * The GPIO \ref ALTERNATOR1_RPM_PIN is attached as external interrupt.
   * 
   */
  void _setUpAlternator1SpeedInt(void);

  /************************************************************************//**
   * \brief Set up the Alternator2 speed timer and interrupt
   *
   * The engine speed calculation is using the timer2 and and interrupt. With
   * the timer0 prescaler ( \ref TIMER_PRESCALER_FOR_1NS) adjusted, that one tick 
   * is 1ns.
   * The GPIO \ref ALTERNATOR2_RPM_PIN is attached as external interrupt.
   * 
   */
  void _setUpAlternator2SpeedInt(void);
  

};



#endif //_acquire_data_h_