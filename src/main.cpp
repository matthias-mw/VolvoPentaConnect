// Doxygen Dokumentation
/*! \file 	main.cpp
 *  \brief  Hauptprogramm des VolvoPentaConnect Projektes
 *
 *  Mit dem VolvoPentaConnect Projekt sollen aktuelle Messwerte
 *  und Daten von dem VolvoPenta Diesel Motor über NMEA2000
 *  CAN-BUS direkt in das Netzwerk im Boot eingespielt werden.
 *
 * \author 		Matthias Werner
 * \date		10 Sep 2022
 *
 * - Prozessor:         ESP32-WROOM
 * - Hardware:          az-delivery-devkit-v4
 **************************************************************/

#include <hardwareDef.h>

// This will automatically choose right CAN library and create suitable
// NMEA2000 object
// #include <NMEA2000_CAN.h>
// #include <N2kMessages.h>

// // Handle NMEA2000 Bus
// #include <process_n2k.h>

#include <datapoint.h>
#include <acquire_data.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ADS1115_WE.h>
#include <Wire.h>


tAcquireData data;

void setup()
{

  // Wait that all Sensors can settle
  delay(1000);

  // Start Serial Output/Input
  Serial.begin(115200);

  // Setup the LED
  pinMode(STATUS_LED,OUTPUT);
  pinMode(BUTTON1,INPUT);

  // Start I2C 
  Wire.begin();

  // Start the DS18B20 sensor
  oneWireSensors.begin();

  // Start ADS1115
  data.setUpADS1115();

  // // Setup NMEA2000 Interface
  // setupN2K();

}

tDataPoint dp = tDataPoint(senType_ds1820, "Channel 1", "s");
uint32_t k = 0;

void loop()
{

  Serial.println("Test...");

  if(!digitalRead(BUTTON1)){
    Serial.println("Button pressed...");
  }

  // SendN2kEngineParm();
  // NMEA2000.ParseMessages();

  //dp.updateValue(k, millis());
  //dp.printDatapoint();

  data.listOneWireDevices();

  data.measureOnewire();
  data.measureVoltage();
  data.showDataOnTerminal();


  k++;
  //delay(1000);
  

  
}



