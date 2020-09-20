#include <Arduino.h>
#include "vicor.h"

Vicor vicor;

void setup() {

  Serial.begin(9600);
  //Initialize the chip

  while (vicor.begin() != 0) {
    Serial.println("Failed to Initialize the chip, please confirm the wire connection");
    delay(1000);
  }

  //Serial.print("Chip serial number");
  //Serial.println(vicor.readSerialNumber());
}

void loop() {
  Serial.print("Ambient Temperature(°C/F):");
  //Serial.print(vicor.getTemperature());
  Serial.print(" C/");
  delay(1000);

}
