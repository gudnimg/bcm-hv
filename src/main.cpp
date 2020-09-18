/*!
 * @brief Construct the function
 * @param pWire IIC bus pointer object and construction device, can both pass or not pass parameters, Wire in default.
 * @param address Chip IIC address, two optional addresse.

*/
/*!
 * @file singleMeasurement.ino
 * @brief Read ambient temperature (C/F) and relative humidity (%RH) in single-read mode.
 * @n Experimental phenomenon: the chip defaults in this mode, we need to send instructions to enable the chip collect data,
 * which means the repeatability of the read needs to be set (the difference between the data measured by the chip under the same measurement conditions)
 * then read the temperature and humidity data and print the data in the serial port.
 * @n Single measure mode: read data as needed, power consumption is relatively low, the chip idle state only costs 0.5mA. 
 * @copyright  Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [fengli](li.feng@dfrobot.com)
 * @version  V1.0
 * @date  2019-08-21
 * @get from https://www.dfrobot.com
 * @url https://github.com/DFRobot/DFRobot_SHT3x
*/

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
  Serial.print(vicor.getTemperatureC());
  Serial.print(" C/");
  Serial.print(sht3x.getTemperatureF());
  Serial.print(" F ");
  Serial.print("Relative Humidity(%RH):");
  Serial.print(sht3x.getHumidityRH());
  Serial.println(" %RH");
  delay(1000);

}
