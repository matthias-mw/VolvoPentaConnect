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
#include <NMEA2000_CAN.h>  
#include <N2kMessages.h>

// OneWire Temperaturmessung
#include <measure_onewire.h>
// Handle NMEA2000 Bus
#include <process_n2k.h>

void setup() {

  // Wait that all Sensors can settle
  delay(1000);

  // Start Serial Output/Input
  Serial.begin(115200);

  // Setup the LED
  pinMode(STATUS_LED,OUTPUT);

  // Setup NMEA2000 Interface
  setupN2K();

  // List all devices an 1Wire
  listOneWireDevices(Serial);

}



void loop() {

  SendN2kEngineParm();
  NMEA2000.ParseMessages();
}
