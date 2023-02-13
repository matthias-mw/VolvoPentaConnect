
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

/************************************************************************ //**
 * \enum   tSensorTyp
 * \brief  Specifies the typ of Sensor use for data aquisition
 */
typedef enum
{
    /** xxxx */
    senType_ds1820,

    /** xxxx. */
    senType_ads1115,

    /** */
    senType_virtual,
    /**  */
    senType_none
} tSensorTyp;

/************************************************************************ //**
 * \class tDataPoint
 * \brief This gives an object for a Datapoint
 */
class tDataPoint
{
public:
    tDataPoint(tSensorTyp senType, String name = "na", String unit = "-");

    void setName(String name);
    void setUnit(String unit);
    String getName();
    String getUnit();

    /********************************************************************* //**
     * \brief Update the value of the datapoint
     * 
     * This method updates the value and the timestamp of an datapoint and
     * calculates all other values (eg history and mean) inside the datapoint.
     *
     * \param new_value {type}
     * \param new_timestamp {type}
     * \return true
     * \return false
     */
    bool updateValue(double new_value, uint32_t new_timestamp);

    /******************************************************************** //**
     * \brief Printing all the Infos of a Datapoint
     *
     * \return true
     * \return false
     */
    bool printDatapointFull();

private:
    String signalName = "na";
    String signalUnit = "-";
    tSensorTyp sensorTyp = senType_none;
    uint32_t timestamp = 0;
    uint32_t mean_cnt = 3;
    double value = 0;
    double value_mean = 0;
    double value_history[MAX_HISTORY_BUFFER_SIZE];
};

#endif //_datapoint_h_