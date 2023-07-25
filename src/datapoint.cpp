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

tDataPoint::tDataPoint(tSensorTyp senType, String name, String unit)
{
  uint8_t i;

  this->sensorTyp = senType;
  this->signalName = name;
  this->signalUnit = unit;

  for (i = 0; i < MAX_HISTORY_BUFFER_SIZE; i++)
  {
    this->value_history[i] = 0;
  }
}

void tDataPoint::setName(String name)
{
  if (name != "")
  {
    this->signalName = name;
  }
  else
  {
    this->signalName = "na";
  }
}
void tDataPoint::setUnit(String unit)
{
  if (unit != "")
  {
    this->signalUnit = unit;
  }
  else
  {
    this->signalUnit = "-";
  }
}

String tDataPoint::getName()
{
  return this->signalName;
}

String tDataPoint::getUnit()
{
  return this->signalUnit;
}

bool tDataPoint::updateValue(double new_value, uint32_t new_timestamp)
{
  uint8_t k;
  double mean = 0;
  double tmp = 0;

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
  }else{
    this->value_mean = new_value;
  }

  return true;
}

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