// Doxygen Documentation
/*! \file 	acquire_data.cpp
 *  \brief  Acquire data from all sensors
 *
 * This File contains all the necessary methods acquire the
 * data from all sensors.
 *
 * \author 		Matthias Werner
 * \date		02/2023
 *
 * - Prozessor:         ESP32-WROOM
 * - Hardware:          az-delivery-devkit-v4
 */

#include <acquire_data.h>

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONEWIRE_PIN);
// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature oneWireSensors(&oneWire);

ADS1115_WE objAD1115 = ADS1115_WE(ADS1115_I2C_ADDRESS);


tAcquireData::tAcquireData()
{

  // Setup ADS1115


}

//****************************************
// Stores the data into a Datapoint
void tAcquireData::_store_data(tDataPoint &db, double value, uint32_t timestamp)
{
  // Update the Value of the Datapoint
  db.updateValue(value, timestamp);
}

//****************************************
// Measure all OneWire Sensors
void tAcquireData::measure_onewire()
{

  // Measure Wall Sensor Cooling 
  oneWireSensors.requestTemperaturesByAddress(oWtCoolWall);
  double temp = oneWireSensors.getTempC(oWtCoolWall);
  _store_data(tCoolWall,temp, millis());

  // Measure Engine Room Sensor
  oneWireSensors.requestTemperaturesByAddress(oWtEngRoom);
  temp = oneWireSensors.getTempC(oWtEngRoom);
  _store_data(tEngRoom,temp, millis());

  // Measure Gearbox Sensor
  oneWireSensors.requestTemperaturesByAddress(oWtGearbox);
  temp = oneWireSensors.getTempC(oWtGearbox);
  _store_data(tGearbox,temp, millis());

}

void tAcquireData::measure_voltage(){

  double voltage;

  voltage = objAD1115.getResult_V();
  this->_store_data(this->uBat,voltage,millis());
  
}



//****************************************
// 
void tAcquireData::show_data(){


  tCoolWall.printDatapointFull();
  tGearbox.printDatapointFull();
  tEngRoom.printDatapointFull();
  uBat.printDatapointFull();

}

//****************************************
// Listing of all OneWire Devices at the bus
void tAcquireData::listOneWireDevices()
{

  uint8_t i;
  uint8_t deviceCount;
  DeviceAddress address;

  // locate devices on the bus
  Serial.println("Locating devices...");
  Serial.print("Found ");
  deviceCount = oneWireSensors.getDeviceCount();
  Serial.print(deviceCount, DEC);
  Serial.println(" devices.");
  deviceCount = oneWireSensors.getDS18Count();
  Serial.print(deviceCount, DEC);
  Serial.println(" DS18S20 Sensors.");
  Serial.println("");

  Serial.println("Printing addresses...");
  for (int i = 0; i < deviceCount; i++)
  {
    Serial.print("Sensor ");
    Serial.print(i + 1);
    Serial.print(" : ");
    oneWireSensors.getAddress(address, i);
    for (uint8_t i = 0; i < 8; i++)
    {
      Serial.print("0x");
      if (address[i] < 0x10)
        Serial.print("0");
      Serial.print(address[i], HEX);
      if (i < 7)
        Serial.print(", ");
    }
    Serial.println("");
  }
}

void  tAcquireData::_setUpADS1115(void){
  
    if(!objAD1115.init()){
      Serial.print("ADS1115 not connected!");
   }
    objAD1115.setVoltageRange_mV(ADS1115_RANGE_6144);
    objAD1115.setCompareChannels(ADS1115_COMP_0_GND);
    objAD1115.setMeasureMode(ADS1115_CONTINUOUS); 


}