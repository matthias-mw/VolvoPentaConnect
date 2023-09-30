// Doxygen Dokumentation
/*! \file 	datapoint.cpp
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

#include <datapoint.h>

// *****************************************************************************
// Constructor
// *****************************************************************************
tDataPoint::tDataPoint(tSensorTyp senType, String name, String unit)
{
  uint8_t i;

  // initialize Values
  this->sensorTyp = senType;
  this->signalName = name;
  this->signalUnit = unit;

  // create a mutex object to protect data for synchronized handling
  this->xMutexDataLock = xSemaphoreCreateMutex(); 

  // initialize history
  for (i = 0; i < MAX_HISTORY_BUFFER_SIZE; i++)
  {
    this->value_history[i] = 0;
  }
}

// *****************************************************************************
// Get the Name of this Datapoint
// *****************************************************************************
String tDataPoint::getName()
{
  return this->signalName;
}

// *****************************************************************************
// Get the Unit of this Datapoint
// *****************************************************************************
String tDataPoint::getUnit()
{
  return this->signalUnit;
}

// *****************************************************************************
// Update the value of the Datapoint
// *****************************************************************************
bool tDataPoint::updateValue(double new_value, uint32_t new_timestamp)
{
  uint8_t k;
  double mean = 0;
  double tmp = 0;

  // Check if the Semaphore used for Dataprotection is initialzed
  if (this->xMutexDataLock != NULL)
  {
    // See if we can obtain the semaphore. If the semaphore is not
    // available wait 10ms to see if it becomes free.
    if (xSemaphoreTake(this->xMutexDataLock, (TickType_t)1) == pdTRUE)
    {
      // We were able to obtain the semaphore and can now use 
      // the protected data 

      // Move History to next point
      for (k = (MAX_HISTORY_BUFFER_SIZE - 1); k > 0; k--)
      {
        this->value_history[k] = this->value_history[k - 1];
      }

      // save the value and timestamp
      this->value = new_value;
      this->value_history[0] = new_value;
      this->timestamp = new_timestamp;

      // don't calculate mean for boolean values
      if (this->sensorTyp != senType_GPIO)
      {
        // Calculate mean value
        for (k = 0; k < this->mean_cnt; k++)
        {
          mean = mean + this->value_history[k];
        }
        this->value_mean = mean / this->mean_cnt;
      }
      else
      {
        this->value_mean = new_value;
      }
      // We have finished accessing the shared resource.  Release the
      // semaphore. 
      xSemaphoreGive(this->xMutexDataLock);

    }
    else
    {
      /* We could not obtain the semaphore and can therefore not access
      the shared resource safely. */
      Serial.print(millis());
      Serial.println("No Semaphore obtained. Data Resource has been blocked for to long, no update possible");
    }
  }

  return true;
}

// *****************************************************************************
// Get the Value of the datapoint object
// *****************************************************************************
double tDataPoint::getValue(){

  double tmpValue = 0;

  // Check if the Semaphore used for Data protection is initialzed
  if (this->xMutexDataLock != NULL)
  {
    // See if we can obtain the semaphore. If the semaphore is not
    // available wait 10ms to see if it becomes free.
    if (xSemaphoreTake(this->xMutexDataLock, (TickType_t)1) == pdTRUE)
    {
      tmpValue = this->value;
      
      // We have finished accessing the shared resource.  Release the
      // semaphore. 
      xSemaphoreGive(this->xMutexDataLock);
    }
    else
    {
      /* We could not obtain the semaphore and can therefore not access
      the shared resource safely. */
      Serial.print(millis());
      Serial.println("No Semaphore obtained. Data Resource has been blocked for to long, no Read possible");

    }
  }

  return tmpValue;

}

// *****************************************************************************
// Get the Value Mean of the Datapoint object
// *****************************************************************************
double tDataPoint::getValueMean(){

  double tmpValue = 0;

  // Check if the Semaphore used for Data protection is initialzed
  if (this->xMutexDataLock != NULL)
  {
    // See if we can obtain the semaphore. If the semaphore is not
    // available wait 10ms to see if it becomes free.
    if (xSemaphoreTake(this->xMutexDataLock, (TickType_t)1) == pdTRUE)
    {
      tmpValue = this->value_mean;
      
      // We have finished accessing the shared resource.  Release the
      // semaphore. 
      xSemaphoreGive(this->xMutexDataLock);
    }
    else
    {
      /* We could not obtain the semaphore and can therefore not access
      the shared resource safely. */
      Serial.print(millis());
      Serial.println("No Semaphore obtained. Data Resource has been blocked for to long, no ReadMean possible");
    }
  }

  return tmpValue;

}

// *****************************************************************************
// Printing all the Infos of a Datapoint
// *****************************************************************************
bool tDataPoint::printDatapointFull()
{

  uint8_t k;
  String str = "";

  // Add Timestamp
  str = str + String(this->timestamp) + " -> ";

  // Add Name
  str = str + "Name: " + this->signalName;
  // Add Sensortype
  switch (this->sensorTyp)
  {
  case senType_ds1820:
    str = str + " - DS18S20";
    break;
  case senType_virtual:
    str = str + " - virtual";
    break;

  default:
    str = str + " ------";
    break;
  }

  // Add Value
  str = str + " -> Value: " + String(this->value, 3U) + " " + this->signalUnit;

  // Add Mean
  str = str + " -> Mean: " + String(this->value_mean) + " " + this->signalUnit;

  // Add Mean
  str = str + " -> History: ";

  for (k = 0; k < MAX_HISTORY_BUFFER_SIZE; k++)
  {

    str = str + String(this->value_history[k]) + " ";
  }

  // print
  Serial.println(str);

  return true;
}

// *****************************************************************************
// Printing Short Infos of a Datapoint
// *****************************************************************************
bool tDataPoint::printDatapointShort()
{

  uint8_t k;
  String str = "";

  // Add Name
  str = this->signalName + ": ";

  // Add Value
  str = str + String(this->value, 3U) + " " + this->signalUnit + "; ";

  // print
  Serial.print(str);

  return true;
}