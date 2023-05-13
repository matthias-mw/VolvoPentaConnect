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
#include <ADS1115_WE.h>

#include <adc_calib.h>
#include <datapoint.h>



/** oneWire instance to communicate with any OneWire devices */
extern OneWire oneWire;
/** Object for our oneWire Dallas Temperature sensors */
extern DallasTemperature oneWireSensors;
/** Object for the AD1115 voltage sensor */
extern ADS1115_WE objAD1115;



 // ------------------------------------------------------------------
  // variables for engine speed calculation
  // ------------------------------------------------------------------
/************************************************************************//**
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

class tAcquireData
{
  /** max depth of the data history of each data point*/
  const uint8_t _MAX_DATA_POINTS_HISTORY = 8;

public:

  /************************************************************************//**
   * \brief Construct a new tAcquireDataobject
   *
   */
  tAcquireData();


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
   * \brief Setup the the ADS 1115 sensor on the i2c bus
   *
   * This method has to be run once in the setup routine to configure all
   * necessary options on the ADS1115 chip
   */
  void setUpADS1115(void);

  /************************************************************************//**
   * \brief  Measure all the voltages
   *
   * This method measures all voltages and stores the value in the 
   * corresponding datapoint.
   */
  void measureVoltage();

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
  double calcNumberOfRevs(tSpeedCalc *tmrValues);

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

  /** Structure with all timer values for the engine speed calculation*/
  tSpeedCalc engSpeedCalc;
 
  /** Structure with all timer values for the alternator 1 speed calculation*/
  tSpeedCalc alternator1SpeedCalc;

  /** Structure with all timer values for the alternator 2 speed calculation*/
  tSpeedCalc alternator2SpeedCalc;

private:

  // Define specific OneWire sensors
  DeviceAddress oWtCoolWall = {0x28, 0xE8, 0xF7, 0xAA, 0x03, 0x00, 0x00, 0x37};
  DeviceAddress oWtEngRoom = {0x28, 0xBF, 0xCA, 0xAA, 0x03, 0x00, 0x00, 0xF4};
  DeviceAddress oWtGearbox = {0x28, 0xBF, 0xCA, 0xAA, 0x03, 0x00, 0x00, 0xF4};

  tDataPoint tCoolWall = tDataPoint(senType_ds1820, "tCoolWall", "GrdC");
  tDataPoint tEngRoom = tDataPoint(senType_ds1820, "tEngRoom", "GrdC");
  tDataPoint tGearbox = tDataPoint(senType_ds1820, "tGearbox", "GrdC");
  tDataPoint uBat = tDataPoint(senType_ads1115, "Ubat", "V");
  tDataPoint uTest = tDataPoint(senType_ads1115, "U_Test", "V");
  tDataPoint uTest2 = tDataPoint(senType_ads1115, "U_Test2", "V");



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


  

};


#endif //_acquire_data_h_