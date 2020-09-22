#include <Arduino.h>
#include "vicor.h"

Vicor vicor;

void setup() {

  Serial.begin(9600);
  //Initialize the chip

  while (!Serial) {
  };
  
  while (vicor.begin() != 0) {
    Serial.println("Failed to Initialize the chip, please confirm the wire connection");
    delay(1000);
  }

  
  //Serial.println(vicor.readSerialNumber());
}

void loop() {
  Serial.print("Input voltage (V):");
  Serial.println(vicor.get_READ_VIN());

  Serial.print("Input current (A):");
  Serial.println(vicor.get_READ_IIN());

  Serial.print("Output voltage (V):");
  Serial.println(vicor.get_READ_VOUT());

  Serial.print("Output current (A):");
  Serial.println(vicor.get_READ_IOUT());

  Serial.print("Output resistance (Ohms):");
  Serial.println(vicor.get_READ_BCM_ROUT());

  Serial.print("Internal temperature (°C):");
  Serial.println(vicor.get_READ_TEMPERATURE_1());

  Serial.print("K factor (V/V):");
  Serial.println(vicor.get_READ_K_FACTOR());

  Serial.print("Time on delay (s):");
  Serial.println(vicor.get_TON_DELAY());

  Serial.print("Output power (W):");
  Serial.println(vicor.get_READ_POUT());

  Serial.print("Minimum rated high side voltage: ");
  Serial.println(vicor.get_MFR_VIN_MIN());

  vicor.write_PAGE();
  
  vicor.read_status_word();
  Serial.print("\n");

  delay(1000);

}
