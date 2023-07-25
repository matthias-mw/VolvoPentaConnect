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

#include <stdint.h>
// This will automatically choose right CAN library and create suitable
// NMEA2000 object
#include <NMEA2000_CAN.h>
#include <N2kMessages.h>

// // Handle NMEA2000 Bus
 #include <process_n2k.h>

#include <datapoint.h>
#include <acquire_data.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>

#include <lookUpTable.h>


LookUpTable1D tab(AXIS_TCO_MES, MAP_TCO_MES, TCO_AXIS_LEN, TCO_MAP_PREC);

/// class that contains al measured data
AcquireData data;

/// structure that hold all data ready for N2k sending
tVolvoPentaData VolvoDataForN2k;

void setup()
{

  // Wait that all Sensors can settle
  delay(1000);

  // Start Serial Output/Input
  Serial.begin(115200);

  // Init all the PINs
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);
  pinMode(BUTTON1_PIN, INPUT);
  pinMode(BUTTON2_PIN, INPUT);

  pinMode(WARNING_OUTPUT_PIN, OUTPUT);
  digitalWrite(WARNING_OUTPUT_PIN, LOW);
  pinMode(RELAY_OUTPUT_PIN, OUTPUT);
  digitalWrite(RELAY_OUTPUT_PIN, LOW);

  pinMode(CONTACT1_PIN, INPUT);
  pinMode(CONTACT2_PIN, INPUT);
  pinMode(CONTACT3_PIN, INPUT);

  pinMode(LT_ON_PIN, OUTPUT);
  digitalWrite(LT_ON_PIN, HIGH);
  pinMode(NOT_CS_THERMO_PIN, OUTPUT);
  digitalWrite(NOT_CS_THERMO_PIN, HIGH);
  pinMode(NOT_CS_ADC_PIN, OUTPUT);
  digitalWrite(NOT_CS_ADC_PIN, HIGH);

  pinMode(ONEWIRE_PIN, INPUT);

  pinMode(ENGINE_RPM_PIN, INPUT);
  pinMode(SHAFT_RPM_PIN, INPUT);
  pinMode(ALTERNATOR1_RPM_PIN, INPUT);
  pinMode(ALTERNATOR2_RPM_PIN, INPUT);

  pinMode(ESP32_CAN_TX_PIN, OUTPUT);
  pinMode(ESP32_CAN_RX_PIN, INPUT);

  // Start I2C
  Wire.begin();

  // Start the DS18B20 sensor
  oneWireSensors.begin();

  // Setup NMEA2000 Interface
  setupN2K();

  // Setup all Measurement Channels
  data.setUpMeasurementChannels();

  //data.listOneWireDevices();
}








void loop()
{

  static double EngineRPM = 0;

  // Serial.println("Test...");

  // if (digitalRead(BUTTON2_PIN))
  // {
  //   Serial.println("Button pressed...");
  //   digitalWrite(WARNING_OUTPUT_PIN, HIGH);
  // }else{
  //   digitalWrite(WARNING_OUTPUT_PIN, LOW);
  // }

  digitalWrite(STATUS_LED_PIN, LOW);
  delay(250);
  digitalWrite(STATUS_LED_PIN, HIGH);

  SendN2kEngineParm(VolvoDataForN2k);
  NMEA2000.ParseMessages();


  

  //data.measureOnewire();
  data.measureVoltage();
  data.measureSpeed();
  data.measureExhaustTemperature();
  data.checkContacts();

  data.showDataOnTerminal();

  data.convertDataToN2k(& VolvoDataForN2k);



  delay(750);
}
