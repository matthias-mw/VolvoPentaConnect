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
  pinMode(STATUS_LED, OUTPUT);
  pinMode(BUTTON1, INPUT);

  // Start I2C
  Wire.begin();

  // Start the DS18B20 sensor
  oneWireSensors.begin();

  // Start ADS1115
  data.setUpADS1115();

  // // Setup NMEA2000 Interface
  // setupN2K();

  data._setUpEngineSpeedInt();
  data._setUpAlternator1SpeedInt();
  data._setUpAlternator2SpeedInt();

  

  //*****************************************************************************
  // Only for frequency simulation in loop()
  ledcAttachPin(26, 1); // sets GPIO26 as signal output (for simulation only)
  //*****************************************************************************

}

tDataPoint dp = tDataPoint(senType_ds1820, "Channel 1", "s");
uint32_t k = 0;

void loop()
{

  static double EngineRPM = 0;

  //Serial.println("Test...");

  if (!digitalRead(BUTTON1))
  {
    Serial.println("Button pressed...");
  }

  // SendN2kEngineParm();
  // NMEA2000.ParseMessages();

  // dp.updateValue(k, millis());
  // dp.printDatapoint();

  //data.listOneWireDevices();

  //data.measureOnewire();
  data.measureVoltage();
  //data.showDataOnTerminal();

  EngineRPM = data.calcNumberOfRevs(&data.engSpeedCalc);
  Serial.printf("Engine RPM  :%4.0f rev/min \n", EngineRPM);

  
  EngineRPM = data.calcNumberOfRevs(&data.alternator1SpeedCalc);
  Serial.printf("Alternator1 RPM  :%4.0f rev/min \n", EngineRPM);

  EngineRPM = data.calcNumberOfRevs(&data.alternator2SpeedCalc);
  Serial.printf("Alternator2 RPM  :%4.0f rev/min \n", EngineRPM);


  // For frequency simulation only
  //**********************************
  static unsigned long timer = 0;
  if (millis() > timer  + 100) {
    timer = millis();
    ledcSetup(1, analogRead(34), 7);
    ledcWrite(1, 64);
  }
  //***********************************

  k++;
   delay(100);
}
