
// Doxygen Dokumentation
/*! \file 	datapoint.h
 *  \brief  Storing the measured values into datapoints
 *
 * This File contains all the necessary methods to handle measured
 * datapoints.
 *
 * \author 		Matthias Werner
 * \date		02/2023
 *
 * - Prozessor:         ESP32-WROOM
 * - Hardware:          az-delivery-devkit-v4
 */

#ifndef _datapoint_h_
#define _datapoint_h_

#include <Arduino.h>

#define MAX_HISTORY_BUFFER_SIZE 10

/*! ************************************************************************
 * \enum   tSensorTyp
 * \brief  Specifies the typ of Sensor use for data acquisition
 */
typedef enum
{
    /** Sensortyp DS18S20 - oneWire Thermocouple */
    senType_ds1820,

    /** Sensortyp Drehzahlmessung */
    senType_RPM,

    /** Sensortyp MAX6675 - SPI device for NiCr-Ni Thermocouples */
    senType_max6675,

    /** Sensortype MCP3204 - SPI Analog-in-Digital-Wandler 4Channel */
    senType_mcp3204,

    /** Sensortyp on chip AD-Wandler at the esp32. */
    senType_adc,

    /** Sensortyp General IO of the esp32  */
    senType_GPIO,

    /** Sensortyp virtual - simulated or calculated signal*/
    senType_virtual,

    /** default no Sensortyp defined */
    senType_none
} tSensorTyp;

/*! ************************************************************************
 * \class DataPoint
 * \brief This gives an object for a Datapoint
 * 
 * This object is used to store all the necessary information for a datapoint
 * like name, unit, value, history and mean value. It also provides methods
 * to update the value and calculate the mean value. The mean value is
 * calculated over a defined number \ref MAX_HISTORY_BUFFER_SIZE of values 
 * in the history buffer.
 * 
 */
class DataPoint
{
public:
    /*! ************************************************************************
     * \brief Construct a new t Data Point object
     *
     * \param senType   Type of Sensor used (\ref tSensorTyp)
     * \param name      Datapoint name (eG. batteryvoltage)
     * \param unit      Datapoint unit (eG. V)
     * \param min_value Lower limit of the value
     * \param max_value Upper limit of the value
     */
    DataPoint(tSensorTyp senType, String name = "na", String unit = "-", double min_value = -9999, double max_value = 9999);

    /*! ************************************************************************
     * \brief Get the Name of this Datapoint
     *
     * \return String name of datapoint
     */
    String getName();

    /*! ************************************************************************
     * \brief Get the Unit of this Datapoint
     *
     * \return String unit of datapoint
     */
    String getUnit();

    /*! ************************************************************************
     * \brief Get the Value of the datapoint object
     * 
     * This method returns the current value of the datapoint.
     * 
     * The method is protected by a semaphore to make 
     * sure that reading and writing of the datapoint is consistent in
     * multithreading operations
     *
     * \return double 
     */double getValue();

    /*! ************************************************************************
     * \brief Get the Value Mean of the Datapoint object
     * 
     * This method returns the mean value of the datapoint.
     * 
     * The method is protected by a semaphore to make 
     * sure that reading and writing of the datapoint is consistent in
     * multithreading operations
     *
     * \return double 
     */
    double getValueMean();
    
    /*! ************************************************************************
     * \brief Update the value of the Datapoint
     * 
     * This method updates the value and the timestamp of an datapoint and
     * calculates all other values (eg history and mean) inside the datapoint.
     * 
     * The method is protected by a semaphore to make 
     * sure that reading and writing of the datapoint is consistent in
     * multithreading operations
     *
     * \param new_value         new value for updating
     * \param new_timestamp     corresponding timestamp
     * \return true
     * \return false
     */
    bool updateValue(double new_value, uint32_t new_timestamp);

    /*! ************************************************************************
     * \brief Printing all the Infos of a Datapoint
     * 
     * This Method prints all informations of the Datapoint to the serial
     * stream.
     *
     * \return true
     * \return false
     */
    bool prinDataPointFull();

    /*! ************************************************************************
     * \brief Printing Short Infos of a Datapoint
     * 
     * This Method prints a short version of the informations of the 
     * Datapoint to the serialstream.
     * 
     * \return true
     * \return false
     */
    bool prinDataPointShort();

private:

    /// Signal name of the datapoint
    String signalName = "na";
    /// unit of the datapoint
    String signalUnit = "-";
    /// Typ of sensor used for this signal
    tSensorTyp sensorTyp = senType_none;
    /// timestamp for den data acquisition of this datapoint
    uint32_t timestamp = 0;
    /* number of values from the signals history that is used to calculate 
        the moving average */
    uint32_t mean_cnt = 3;
    /// current value of the datapoint
    double value = 0;
    /// current mean value of the datapoint
    double value_mean = 0;
    /// history of the signals values 
    double value_history[MAX_HISTORY_BUFFER_SIZE];
    /// lower limit of the value
    double value_limit_min = -9999;
    /// upper limit of the value
    double value_limit_max = 9999;

    /// semaphore handle to ensure data consistency while reading and 
    /// writing in parallel tasks
    SemaphoreHandle_t xMutexDataLock = NULL;  // Create a mutex object
    
};


#endif //_datapoint_h_