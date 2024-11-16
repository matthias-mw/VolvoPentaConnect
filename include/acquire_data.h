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
#include <OneWire.h>
#include <DallasTemperature.h>
#include <max6675.h>
#include <MCP_ADC.h>

#include <Preferences.h>
#include <adc_calib.h>
#include <datapoint.h>
#include <process_n2k.h>
#include <lookUpTable.h>
#include <errorflag.h>

// ------------------------------------------------------------------
// Calibration Data for threshold values
// ------------------------------------------------------------------

/*! Enging speed in RPM above that the engine is
  considered to be running */
#define ENGINE_RUN_RPM_THRESHOLD -100

/*! oil pressure in bar below that the engine is triggering
 an low oil pressure alarm */
#define OIL_PRESSURE_LOW_THRESHOLD 0.5

/*! coolant temperature in deg celsius above that the engine is
 triggering*/
#define COOLANT_TEMPERATURE_HIGH_THRESHOLD 100

/*! exhaust temperature in deg celsius above that an high
  exhaust temperature alarm is triggered */
#define EXHAUST_TEMPERATURE_HIGH_THRESHOLD 500

/*! gearbox temperature in deg celsius above that an high
  gearbox temperature alarm is triggered */
#define GEARBOX_TEMPERATURE_HIGH_THRESHOLD 75

/*! alternator temperature in deg celsius above that an high
  alternator temperature alarm is triggered */
#define ALTERNATOR_TEMPERATURE_HIGH_THRESHOLD 135

/*! sea water temperature in deg celsius above that an high
  sea water temperature alarm is triggered */
#define SEA_WATER_TEMPERATURE_HIGH_THRESHOLD 55

// ------------------------------------------------------------------
// ------------------------------------------------------------------

/** oneWire instance to communicate with any OneWire devices */
extern OneWire oneWire;
/** Object for our oneWire Dallas Temperature sensors */
extern DallasTemperature oneWireSensors;
/// Mutex for protecting data integrity of \ref VolvoDataForN2k
extern SemaphoreHandle_t xMutexVolvoN2kData;

/// Buffer for Crystal  LCD Display 4x20 (4 lines a 20 char)
extern char lcdDisplay[4][20];

// ------------------------------------------------------------------
// variables for engine speed calculation
// ------------------------------------------------------------------

/*! ************************************************************************
 * \struct tSpeedCalc
 * \brief Structure that handles all values to determine the time difference
 * in between of two interrupts *
 */
typedef struct tSpeedCalc
{
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

// ------------------------------------------------------------------
// ErrorHandling for the engine
// ------------------------------------------------------------------

/*! \struct tEngineStatus
 *  \brief Structure that handles all status bits for the engine
 *  The structure consists bits for low Oil pressure, High engine
 * temperature, ....
 * This will be the basis for error handling */
typedef struct tEngineStatus
{
  /** Bit for low oil pressure */
  ErrorFlag flgLowOilPressure;
  /** Bit for high coolant temperature */
  ErrorFlag flgHighCoolantTemp;
  /** Bit for high exhaust temperature */
  ErrorFlag flgHighExhaustTemp;
  /** Bit for high gearbox temperature */
  ErrorFlag flgHighGearboxTemp;
  /** Bit for high alternator temperature */
  ErrorFlag flgHighAlternatorTemp;
  /** Bit for high sea water temperature */
  ErrorFlag flgHighSeaWaterTemp;

} tEngineStatus;

/*! ************************************************************************
 * \brief Handle the interrupt triggered by the engine speed
 *
 * This method is called by the interrupt of the engine speed pin and
 * determins the duration in between 2 events.
 */
void IRAM_ATTR handleEngineSpeedInterrupt();

/*! ************************************************************************
 * \brief Handle the interrupt triggered by the shaft speed
 *
 * This method is called by the interrupt of the shaft speed pin and
 * determins the duration in between 2 events.
 */
void IRAM_ATTR handleShaftSpeedInterrupt();

/*! ************************************************************************
 * \brief Handle the interrupt triggered by the alternator 1 speed
 *
 * This method is called by the interrupt of the alternator 1  speed pin and
 * determins the duration in between 2 events.
 */
void IRAM_ATTR handleAlternator1SpeedInterrupt();

/*! ************************************************************************
 * \brief Handle the interrupt triggered by the alternator 2 speed
 *
 * This method is called by the interrupt of the alternator 2  speed pin and
 * determins the duration in between 2 events.
 */
void IRAM_ATTR handleAlternator2SpeedInterrupt();

/*! ************************************************************************
 * \class AcquireData
 * \brief A class to manage the acquisition of all sensor data
 * 
 * This class is used to acquire all sensor data and store them in
 * corresponding data points. It also calculates the engine status and
 * engine hours. It holds all the necessary methods to measure the data
 * and convert them into N2k data. The data is stored in data points from
 * the type \ref tDataPoint.
 * 
 */
class AcquireData
{
  /** max depth of the data history of each data point*/
  const uint8_t MAX_DATA_POINTS_HISTORY = 8;

public:
  /*! ************************************************************************
   * \brief Construct a new AcquireDataobject
   *
   */
  AcquireData();

  /*! ************************************************************************
   * \brief Displays all data to the terminal
   *
   */
  void showDataOnTerminal();

  /*! ************************************************************************
   * \brief List all OneWire devices
   *
   *  Shows all OneWire Devices and their address.
   */
  void listOneWireDevices();

  /*! ************************************************************************
   * \brief Measure all OneWire Sensors
   *
   * This method measures all specified OneWire Sensors and stores the
   * value in the corresponding datapoint.
   *
   * \note measuring data with the onewire sensor causes a delay of 0.5s
   *       per sensor
   */
  void measureOnewire();

  /*! ************************************************************************
   * \brief  Measure all the voltages
   *
   * This method measures all voltages and stores the value in the
   * corresponding datapoint.
   */
  void measureVoltage();

  /*! ************************************************************************
   * \brief  Measure all Speeds
   *
   * This method measures all rotational speeds and stores the value in the
   * corresponding datapoint.
   */
  void measureSpeed();

  /*! ************************************************************************
   * \brief  Measure Exhaust Temperature
   *
   * This method measures the exhaust temperature while using an NiCr-Ni
   * thermocouple and the MAX6675 Chip
   */
  void measureExhaustTemperature();

  /*! ************************************************************************
   * \brief  Calculate VolvoPenta Original Sensoren
   *
   * This method calulates all values for VolvoPenta sensoren based
   * on measures voltages and interpolation curves.
   *
   * \sa adc_calib.h
   */
  void calculateVolvoPentaSensors();

  /*! ************************************************************************
   * \brief  Checking the states of the contact
   *
   * This method checks the state of all 3 contacts
   */
  void checkContacts();

  /*! ************************************************************************
   * \brief  Setup all the measurement channels
   *
   * This method handles all setup functions necessary for the config of
   * all measurements channels.
   */
  void setUpMeasurementChannels();

  /*! ************************************************************************
   * \brief  Convert all measured data into N2kData formats
   *
   * This method converts all data measured with several sensors into data
   * which fits to the N2k standard and units.
   *
   * \param data {type}
   */
  void convertDataToN2k(tVolvoPentaData *data);

  /*! ************************************************************************
   * \brief Calculating the Engine hours in seconds
   *
   *  This method calculates the engine hours, by measuring the actual
   *  runtime and adding the stored runtime from last engine run.
   */
  void calcEngineSeconds(void);

  /*! ************************************************************************
   * \brief Initial setting of the start value of the engine run time counter
   *
   * This method sets the engine runtime counter to a certain value. This is
   * useful once after a system flash/change or similar, when the stored
   * value got lost.
   *
   * \param runtime   Initial Value for Engine runtime [min]
   */
  void initEngineHours(uint32_t runtime = 0);

  /*! ************************************************************************
   * \brief Calculate the current engine status
   *
   * This method calculates the current engine status based on the
   * measured values and stores the result in \ref currentEngineDiscreteStatus
   *
   */
  void calcEngineStatus(void);

  /*! ************************************************************************
   * \brief Storing data into the NVM flash
   *
   * This method stores dedicated values into the NVM to keep values over
   * a power cycle. The values can be restored by the corresponding
   * method \sa restoreNVMdata()
   *
   */
  void storeNVMdata(void);

  /*! ************************************************************************
   * \brief Retrieving data from NVM and restore values
   *
   * This method is reading dedicated values from the NVM and initialize
   * them after power up. The values have to be stored to NVM via
   * \sa storeNVMdata().
   *
   */
  void restoreNVMdata(void);

  /*! ************************************************************************
   * \brief  Get count of active warning flags
   * This method returns the count of active warning flags from the
   * structure \ref tEngineStatus
   *
   * \return {uint8_t} count of active warning flags
   */
  uint8_t getActiveWarningCount(void);

  /** Structure with all timer values for the engine speed calculation*/
  tSpeedCalc engSpeedCalc;

  /** Structure with all timer values for the shaft speed calculation*/
  tSpeedCalc shaftSpeedCalc;

  /** Structure with all timer values for the alternator 1 speed calculation*/
  tSpeedCalc alternator1SpeedCalc;

  /** Structure with all timer values for the alternator 2 speed calculation*/
  tSpeedCalc alternator2SpeedCalc;

  // =========================================
  // Erfasste oder berechnete Messwerte (public)
  // =========================================
  /** Temperature of the engine coolant measured via a uMcp3204Ch1
      sensor connected to the original VolvoPenta sensor */
  tDataPoint tEngine = tDataPoint(senType_adc, "tEngine", "GrdC", -99, 199);
  /** Temperature of the cooling system measure via a ds1820 sensor mounted
      at the wall of a cooling pipe*/
  tDataPoint tSeaOutletWall = tDataPoint(senType_ds1820, "tSeaOutletWall", "GrdC", -99, 199);
  /** Temperature of the balmar alternator measured via a ds1820 sensor*/
  tDataPoint tAlternator = tDataPoint(senType_ds1820, "tAlternator", "GrdC", -99, 199);
  /** Temperature of the gearbox measured via a ds1820 sensor mounted
      at the wall of the transmission */
  tDataPoint tGearbox = tDataPoint(senType_ds1820, "tGearbox", "GrdC", -99, 199);
  /** Exhaust gas temperature measured via a NiCr-Ni thermocouple */
  tDataPoint tExhaust = tDataPoint(senType_max6675, "tExhaust", "GrdC", -99, 999);

  /** Engine speed measured via an hall sensor at the crankshaft*/
  tDataPoint nMot = tDataPoint(senType_RPM, "nMot", "rpm", 0, 9999);
  /** Shaft speed measured via an hall sensor at the pro shaft */
  tDataPoint nShaft = tDataPoint(senType_RPM, "nShaft", "rpm", 0, 9999);
  /** Alternator 1 speed measured via the W signal of the alternator*/
  tDataPoint nAlternator1 = tDataPoint(senType_RPM, "nAlternator1", "rpm", 0, 19999);
  /** Alternator 2 speed measured via the W signal of the alternator*/
  tDataPoint nAlternator2 = tDataPoint(senType_RPM, "nAlternator2", "rpm", 0, 19999);

  /** Battery voltage measured at the main power source*/
  tDataPoint uBat = tDataPoint(senType_adc, "uBat", "V", 0, 99);

  /** Batterie voltage measured at the main power source*/
  tDataPoint pOil = tDataPoint(senType_adc, "pOil", "bar", 0, 99);

  /** Voltage measured via an MCP3204 on AD Channel 1*/
  tDataPoint uMcp3204Ch1 = tDataPoint(senType_adc, "uMcp3204Ch1", "V", 0, 99);
  /** Voltage measure via an mcp3204 an AD Channel 2*/
  tDataPoint uMcp3204Ch2 = tDataPoint(senType_adc, "uMcp3204Ch2", "V", 0, 99);
  /** Voltage measure via an mcp3204 an AD Channel 3*/
  tDataPoint uMcp3204Ch3 = tDataPoint(senType_adc, "uMcp3204Ch3", "V", 0, 99);
  /** Voltage measure via an mcp3204 an AD Channel 4*/
  tDataPoint uMcp3204Ch4 = tDataPoint(senType_adc, "uMcp3204Ch4", "V", 0, 99);

  /** State of contact 1 */
  tDataPoint flgContact1 = tDataPoint(senType_GPIO, "flgContact1", "-", 0, 1);
  /** State of Contact 2 */
  tDataPoint flgContact2 = tDataPoint(senType_GPIO, "flgContact2", "-", 0, 1);
  /** State of Contact 3 */
  tDataPoint flgContact3 = tDataPoint(senType_GPIO, "flgContact3", "-", 0, 1);

  /** Total run time of the diesel engine in seconds*/
  tDataPoint engSecond = tDataPoint(senType_virtual, "engSecond", "sec", 0, 360000000);

  /** Status object for the current engine status. */
  tEngineStatus currentEngineDiscreteStatus;

private:
  /** Object of the NVMe storage class of the ESP32 */
  Preferences eepromDataStorage;

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

  /** Overall run time [sec] of the engine retrieved from NVM*/
  double eepromStoredEngSeconds = 0;
  /** Timestamp for the last time runtime was read from NVM */
  uint32_t curRunTimeMilliLastRead = 0;

  /*! ************************************************************************
   * \brief Stores the data into a Datapoint
   *
   * Stores the measured value of a signal into a datapoint.
   *
   * \param db        Datapoint in which it should be stored
   * \param value     Measured Value
   * \param timestamp Timestamp in ms of the measurement
   */
  void _StoreData(tDataPoint &db, double value, uint32_t timestamp);

  /*! ************************************************************************
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

  /*! ************************************************************************
   * \brief Set up the Engine speed timer and interrupt
   *
   * The engine speed calculation is using the timer0 and and interrupt. With
   * the timer0 prescaler ( \ref TIMER_PRESCALER_FOR_1NS) adjusted, that one tick
   * is 1ns.
   * The GPIO \ref ENGINE_RPM_PIN is attached as external interrupt.
   *
   */
  void _setUpEngineSpeedInt();

  /*! ************************************************************************
   * \brief Set up the Shaft speed timer and interrupt
   *
   * The engine speed calculation is using the timer0 and and interrupt. With
   * the timer0 prescaler ( \ref TIMER_PRESCALER_FOR_1NS) adjusted, that one tick
   * is 1ns.
   * The GPIO \ref SHAFT_RPM_PIN is attached as external interrupt.
   *
   */
  void _setUpShaftSpeedInt();

  /*! ************************************************************************
   * \brief Set up the Alternator1 speed timer and interrupt
   *
   * The engine speed calculation is using the timer1 and and interrupt. With
   * the timer0 prescaler ( \ref TIMER_PRESCALER_FOR_1NS) adjusted, that one tick
   * is 1ns.
   * The GPIO \ref ALTERNATOR1_RPM_PIN is attached as external interrupt.
   *
   */
  void _setUpAlternator1SpeedInt(void);

  /*! ************************************************************************
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