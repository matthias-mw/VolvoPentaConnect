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

  for (i = 0; i < HISTORY_LENGTH; i++)
  {
    this->value_history[i] = 0xFFFF;
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

bool tDataPoint::getName(String &name)
{
  if (this->signalName == "na")
  {
    return false;
  }
  else
  {
    name = this->signalName;
    return true;
  }
}

bool tDataPoint::getUnit(String &unit)
{
  {
    if (this->signalUnit == "-")
    {
      return false;
    }
    else
    {
      unit = this->signalUnit;
      return true;
    }
  }
}

bool tDataPoint::updateValue(double new_value, uint32_t new_timestamp)
{
  // save the value
  this->value = new_value;
  this->timestamp = new_timestamp;
  return true;
}

bool tDataPoint::printDatapoint()
{

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
  case senType_ads1115:
    str = str + " - ADS1115";
    break;

  default:
    str = str + " ------";
    break;
  }

  // Add Serial
  str = str + " - " + this->sensorSerial;
  // Add Value
  str = str + "     Value: " + String(this->value) + " " + this->signalUnit;

  // print
  Serial.println(str);

  return true;
}