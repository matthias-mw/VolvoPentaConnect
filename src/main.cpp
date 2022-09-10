#include <Arduino.h>

void setup() {
  // put your setup code here, to run once:

  pinMode(32,OUTPUT);


  Serial.begin(115200);
  Serial.println("Hello World");
}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.print(".");
  delay(1000);
  digitalWrite(32,!digitalRead(32));

  
}