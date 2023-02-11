
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

#define HISTORY_LENGTH 10



/************************************************************************ //**
* \enum   tSensorTyp
* \brief  Specifies the typ of Sensor use for data aquisition
*/
typedef enum { 
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

    void setName (String name);
    void setUnit (String unit);
    bool getName (String &name);
    bool getUnit (String &unit);
   
    void setSensorSerial (String senSerial);
    bool getSensorSerial (String &senSerial);
    
    bool updateValue(double new_value, uint32_t new_timestamp);

    bool printDatapoint();

private:

    String signalName = "na";
    String signalUnit = "-";
    tSensorTyp sensorTyp = senType_none;
    String sensorSerial = "";
    uint32_t timestamp = 0;
    uint32_t interval = 1;
    double value_raw = 0;
    double value = 0;
    double value_mean = 0;
    double value_history [HISTORY_LENGTH];

};


#endif  //_datapoint_h_