// Doxygen Dokumentation
/*! \file 	measure_onewire.cpp
 *  \brief  Measuring all data from OneWire Devices
 *
 * \author 		Matthias Werner
 * \date		02/2023
 *
 * - Prozessor:         ESP32-WROOM
 * - Hardware:          az-delivery-devkit-v4
**************************************************************/

#include <measure_onewire.h>


// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

// Define specific sensors
DeviceAddress sensor1 = { 0x28, 0xE8, 0xF7, 0xAA, 0x03, 0x00, 0x00, 0x37 };
DeviceAddress sensor2 = { 0x28, 0xBF, 0xCA, 0xAA, 0x03, 0x00, 0x00, 0xF4 };

void startDS18S20sensors (){

  // Start the DS18B20 sensor
  sensors.begin();
}

// Listing of all OneWire Devices at the bus
void listOneWireDevices(HardwareSerial SerialStr){

  uint8_t i;
  uint8_t addr[8];
  uint8_t k;
  bool blnDone = false;

  SerialStr.println("+++++++++++++  List OneWire Devices  +++++++++++++");
  SerialStr.print("Number of Devices on OneWire Bus: ");
  k = sensors.getDeviceCount();
  SerialStr.println(k);
  SerialStr.print("Number of Ds18S20 Devices Bus:  ");
  k = sensors.getDS18Count();
  SerialStr.println(k);

  //Loop true all OneWire Sensors
  while (!blnDone){
    if (!oneWire.search(addr)) {
      SerialStr.println(" No more addresses.");
      SerialStr.println();
      oneWire.reset_search();
      delay(250);
      blnDone = true;
      break;
    }
    SerialStr.print(" ROM =");
    for (i = 0; i < 8; i++) {
      SerialStr.write(' ');
      SerialStr.print(addr[i], HEX);
    }
    SerialStr.print("\n");
  }

}