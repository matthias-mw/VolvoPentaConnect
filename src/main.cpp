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

#include <Arduino.h>
#include <hardware.Def.h>


void setup() {
  // put your setup code here, to run once:

  pinMode(STATUS_LED,OUTPUT);


  Serial.begin(115200);
  Serial.println("Hello World");
}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.print(".");
  delay(1000);
  digitalWrite(STATUS_LED,!digitalRead(32));

  
}
