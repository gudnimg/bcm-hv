#include "vicor.h"
#include <stdio.h>

Vicor::Vicor(TwoWire *pWire, uint8_t address){
    _pWire   = pWire;
    _address = address;
    _page    = DEFAULT_PAGE;
    
    // Initalise structures
    memset(&status_iout, DEFAULT_STATUS_IOUT, sizeof(status_iout));
    memset(&status_input, DEFAULT_STATUS_INPUT, sizeof status_input);
    memset(&status_temperature, DEFAULT_STATUS_TEMPERATURE, sizeof(status_temperature));
    memset(&status_cml, DEFAULT_STATUS_CML, sizeof status_cml);
    memset(&status_specific, DEFAULT_STATUS_MFR_SPECIFIC, sizeof status_specific);
    measurement_data = {0};
}

int Vicor::begin() 
{
  _pWire->begin(_address);
  _pWire->setClock(400000); // Set i2c clock frequency to 400kHz
  // TODO: add check here to see if device is active.
  return ERR_OK;
}

float Vicor::get_READ_VIN() {
  convert_raw_READ_VIN();
  return measurement_data.vin;
}

float Vicor::get_READ_IIN() {
  convert_raw_READ_IIN();
  return measurement_data.iin;
}

float Vicor::get_READ_VOUT() {
  convert_raw_READ_VOUT();
  return measurement_data.vout;
}

float Vicor::get_READ_IOUT() {
  convert_raw_READ_IOUT();
  return measurement_data.iout;
}

float Vicor::get_READ_BCM_ROUT() {
  convert_raw_READ_BCM_ROUT();
  return measurement_data.rout;
}

int16_t Vicor::get_READ_TEMPERATURE_1() {
  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_READ_TEMPERATURE_1, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_TEMPERATURE_1

  measurement_data.temperature = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  return measurement_data.temperature;
}

float Vicor::get_READ_K_FACTOR() {
  convert_raw_READ_K_FACTOR();
  return measurement_data.kfactor;
}

float Vicor::get_TON_DELAY() {
  convert_raw_TON_DELAY();
  return measurement_data.tdelay;
}

int16_t Vicor::get_READ_POUT() {
  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_READ_POUT, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_POUT

  measurement_data.pout = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  return measurement_data.pout;
}

int16_t Vicor::get_MFR_VIN_MIN() {
  byte rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_MFR_VIN_MIN, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_POUT

  int16_t data = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  return data;
}

int16_t Vicor::get_MFR_VIN_MAX() {
  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_MFR_VIN_MAX, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_POUT

  int16_t data = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  return data;
}

int16_t Vicor::get_MFR_VOUT_MIN() {
  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_MFR_VOUT_MIN, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_POUT

  int16_t data = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  return data;
}

int16_t Vicor::get_MFR_VOUT_MAX() {
  uint8_t len = 2 + 1;
  byte rawData[len]; // buffer for 2 bytes
  write(VICOR_CMD_MFR_VOUT_MAX, 1, false); // Write I2C command
  readData(rawData, len); // Read data from register READ_POUT

  int16_t data = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  return data;
}

int16_t Vicor::get_MFR_IOUT_MAX() {
  byte rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_MFR_IOUT_MAX, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_POUT

  int16_t data = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  return data;
}

int16_t Vicor::get_MFR_POUT_MAX() {
  byte rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_MFR_POUT_MAX, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_POUT

  int16_t data = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  return data;
}

// This overload allows a single byte as input for pBuf.
bool Vicor::write(uint8_t pBuf, size_t size, bool sendStop) {
  return write(&pBuf, size, sendStop);
}

bool Vicor::write(uint8_t* pBuf, size_t size, bool sendStop)
{
  if (pBuf == nullptr) {
    Serial.println("pBuf ERROR!! : null pointer");
  }

  _pWire->beginTransmission(_address); // Start conditon + Device Address + Write bit
  for(size_t i = 0; i < size; ++i){
    _pWire->write(pBuf[i]);
  }
  uint8_t ret = _pWire->endTransmission(sendStop); // use false to not send stop condition

  return ret;
}

bool Vicor::write_PAGE(uint8_t data_byte){

  uint8_t rawData[2] = {VICOR_CMD_PAGE, data_byte};
  uint8_t error = write(rawData, 2, true);

  read_CML();

  if (error != 0)
  {
    switch (error)
    {
    case 1:
      Serial.println("Length too long for buffer.");
      break;
    case 2:
      Serial.println("Address send, NACK received.");
      break;
    case 3:
      Serial.println("Data send, NACK received.");
      break;
    case 4:
      Serial.println("Other twi error.");
      break;
    }
    return false;
  }
  
  // Read back PAGE byte.
  uint8_t buffer[1]; 
  write((uint8_t)VICOR_CMD_PAGE, 1, false); // Write I2C command
  readData(buffer, 1); // Read data from register READ_POUT
  if (buffer[0] != data_byte)
  {
    Serial.println("Error. Writing page data byte failed.");
    //Serial.println(buffer[0]);
    delay(1000);
    //goto retry_again;
  }

  _page = data_byte; // Update page data byte.
}

uint8_t Vicor::readData(uint8_t *pBuf, uint8_t size) {
  uint8_t len = 0;
  
  _pWire->requestFrom(_address, (uint8_t)size, (uint8_t)true);
  while(_pWire->available())  
  {
    pBuf[len++] = _pWire->read();  
  }

  return len;
}

void Vicor::convert_raw_READ_VIN() {
  uint8_t error = 0;
  if (_page != 0x01) {
    error = write_PAGE(0x01); // VIN can only be read when page is 0x01.
    if (error != 0) return;
  }

  uint8_t rawData[2];                           // buffer for 2 bytes
  write(VICOR_CMD_READ_VIN, 1, false);          // Write I2C command
  readData(rawData, 2);                         // Read data from register READ_VIN
  measurement_data.vin = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  measurement_data.vin /= 10.f;             // V_actual = V_reported * 10^-1 (V)
}

void Vicor::convert_raw_READ_IIN() {
  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_READ_IIN, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_IIN
  measurement_data.iin = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  measurement_data.iin /= 1000.f; // I_actual = I_reported * 10^-3 (A)
}

void Vicor::convert_raw_READ_VOUT() {
  uint8_t error = 0;
  if (_page != 0x01) {
    write_PAGE(0x01); // VOUT can only be read when page is 0x01.
    if (error != 0) return;
  }

  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_READ_VOUT, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_VOUT
  measurement_data.vout = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  measurement_data.vout /= 10.f; // V_actual = V_reported * 10^-1 (V)
}

void Vicor::convert_raw_READ_IOUT() {
  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_READ_IOUT, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_IOUT
  measurement_data.iout = ((rawData[0] << 8) | rawData[1]); // Combine high byte and low byte.
  measurement_data.iout /= 100.f; // I_actual = I_reported * 10^-2 (A)
}

void Vicor::convert_raw_READ_BCM_ROUT() {

  if (_page != 0x00) write_PAGE(0x00); // ROUT can only be read when page is 0x00.

  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_READ_BCM_ROUT, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_BCM_ROUT
  measurement_data.rout = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  measurement_data.rout /= 100000.f; // R_actual = R_reported * 10^-5 (Ohms)
}

void Vicor::convert_raw_READ_K_FACTOR() {

  if (_page != 0x00) write_PAGE(0x00); // K factor can only be read when page is 0x00.

  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_READ_K_FACTOR, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_K_FACTOR
  measurement_data.kfactor = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  measurement_data.kfactor /= 65536.f; // K_actual = K_reported * 2^-16 (V/V)
}

void Vicor::convert_raw_TON_DELAY() {

  if (_page != 0x01) write_PAGE(0x01); // Ton delay can only be read when page is 0x01.

  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_TON_DELAY, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register TON_DELAY
  measurement_data.tdelay = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  measurement_data.tdelay /= 1000.f; // t_actual = t_reported * 10^-3 (seconds)
}

void Vicor::get_PMBUS_REVISION() {

  if (_page != 0x00) write_PAGE(0x00); // Ensures we can read.

  uint8_t rawData[1]; // buffer for 2 bytes
  write(VICOR_CMD_PMBUS_REVISION, 1, false); // Write I2C command
  readData(rawData, 1); // Read data from register READ_POUT
  Serial.print("PMbus Revision: 0x");
  Serial.println(rawData[0], HEX);
}

void Vicor::clear_faults() {
  write(VICOR_CMD_CLEAR_FAULTS, 1, true); // Write I2C command
}

void Vicor::operation() {

  clear_faults();
  if (_page != 0x01) {
    uint8_t error = write_PAGE(0x01); // Ensures we can both Read and Write.
    if (error != 0) return;
  }

  uint8_t rawData[2] = {(uint8_t)VICOR_CMD_OPERATION,(uint8_t)0x80}; // buffer for 2 bytes
  write(rawData, 2, true); // Write I2C command

  // Read back Operation mode.
  uint8_t buffer[1];
  write((uint8_t)VICOR_CMD_OPERATION, 1, false); // Write I2C command
  readData(buffer, 1); // Read data from register READ_POUT*/
  if (bitRead(buffer[0], 7) != 1)
  {
    Serial.println("Error. Failed to turn the Module on.");
  }
}

void Vicor::read_CML() {

  //if (_page != 0x00) write_PAGE(0x00); // Ensures we can both Read and Write.
  uint8_t write_buffer[2] = {VICOR_CMD_STATUS_CML, 0x7F};
  write(write_buffer, 2, true); // Write I2C command

  uint8_t buffer[1];
  write(VICOR_CMD_STATUS_CML, 1, false); // Write I2C command
  readData(buffer, 1); // Read data from register READ_POUT
  Serial.print("Status CML: 0x");
  Serial.println(buffer[0], HEX);

  if (buffer[0] & 0x02) Serial.println("CML error: Other Communication Faults.");
  if (buffer[0] & 0x40) Serial.println("CML error: Invalid or Unsupported Data Received.");
  if (buffer[0] & 0x80) Serial.println("CML error: Invalid or Unsupported Command Received.");  
}

void Vicor::read_status_mfr() {
  uint8_t rawData[1]; // buffer for 2 bytes
  write(VICOR_CMD_STATUS_MFR_SPECIFIC, 1, false); // Write I2C command
  readData(rawData, 1); // Read data from register READ_POUT
  Serial.print("Status MFR: 0x");
  Serial.println(rawData[0], HEX);
}

void Vicor::read_status_iout() {
  if (_page != 0x01) write_PAGE(0x01); // Ensures we can read.
  uint8_t write_buffer[2] = {VICOR_CMD_STATUS_IOUT, 0x00};
  write(write_buffer, 2, true); // Write I2C command

  read_CML();

  uint8_t rawData[1]; // buffer for 2 bytes
  write(VICOR_CMD_STATUS_IOUT, 1, false); // Write I2C command
  readData(rawData, 1); // Read data from register READ_POUT
  Serial.print("Status Iout: 0x");
  Serial.println(rawData[0], HEX);

  read_CML();
}

void Vicor::read_status_input() {
  uint8_t write_buffer[2] = {VICOR_CMD_STATUS_INPUT, 0x00};
  write(write_buffer, 2, true); // Write I2C command
  uint8_t rawData[1]; // buffer for 2 bytes
  write(VICOR_CMD_STATUS_INPUT, 1, false); // Write I2C command
  readData(rawData, 1); // Read data from register READ_POUT
  Serial.print("Status Input: 0x");
  Serial.println(rawData[0], HEX);
}

void Vicor::read_status_temperature() {
  uint8_t rawData[1]; // buffer for 2 bytes
  write(VICOR_CMD_STATUS_TEMPERATURE, 1, false); // Write I2C command
  readData(rawData, 1); // Read data from register READ_POUT
  Serial.print("Status Temperature: 0x");
  Serial.println(rawData[0], HEX);
}

void Vicor::print_status_word() {
  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_STATUS_WORD, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_POUT
  uint16_t data = (rawData[0] << 8) | rawData[1];
  Serial.print("Status Word: 0x");
  Serial.println(data, HEX);
}

void Vicor::read_capability() {

  if (_page != 0x00) write_PAGE(0x00); // Ensures we can read.

  uint8_t rawData[1]; // buffer for 2 bytes
  write(VICOR_CMD_CAPABILITY, 1, false); // Write I2C command
  readData(rawData, 1); // Read data from register READ_POUT
  Serial.print("Capability: 0x");
  Serial.println(rawData[0], HEX);
}

void Vicor::write_time_on_delay() {
  if (_page != 0x01) write_PAGE(0x01); // Ensures we can write.
  uint8_t buffer[3] = {VICOR_CMD_TON_DELAY, 0x64, 0x00}; // buffer for 2 bytes
  write(buffer, 3, true); // Write I2C command
}

void Vicor::print_mfr_id() {

  if (_page != 0x00) write_PAGE(0x00); // Ensures we can read.

  uint8_t len = 2 + 1;
  uint8_t buffer[len];
  write(VICOR_CMD_MFR_ID, 1, false); // Write I2C command
  readData(buffer, len); // Read data from register READ_POUT
  Serial.print("MFR ID: ");
  for (int8_t i = 1; i < len; i++)
  {
    if ((char) buffer[i] != '\0') Serial.print((char) buffer[i]);
  }
  Serial.println("");
}

void Vicor::print_mfr_revision() {
  uint8_t len = 18;
  uint8_t buffer[len];
  write(VICOR_CMD_MFR_REVISION, 1, false); // Write I2C command
  readData(buffer, len); // Read data from register READ_POUT
  Serial.print("MFR Revision: ");
  for (int8_t i = 1; i < len; i++)
  {
    if ((char) buffer[i] != '\0') Serial.print((char) buffer[i]);
  }
  Serial.println("");
}

void Vicor::print_mfr_model() {
  uint8_t len = 18 + 1;
  uint8_t buffer[len];
  write(VICOR_CMD_MFR_MODEL, 1, false); // Write I2C command
  readData(buffer, len); // Read data from register READ_POUT
  Serial.print("Part Number: ");
  for (int8_t i = 1; i < len; i++)
  {
    if ((char) buffer[i] != '\0') Serial.print((char) buffer[i]);
  }
  Serial.println("");
}

void Vicor::print_mfr_location() {
  uint8_t len = 2 + 1;
  uint8_t buffer[len];
  write(VICOR_CMD_MFR_LOCATION, 1, false); // Write I2C command
  readData(buffer, len); // Read data from register READ_POUT
  Serial.print("Factory Location (AP): ");
  for (int8_t i = 1; i < len; i++)
  {
    Serial.print((char)buffer[i]);
  }
  Serial.println("");
}

void Vicor::print_mfr_date() {
  uint8_t len = 4 + 1;
  uint8_t buffer[len];
  write(VICOR_CMD_MFR_DATE, 1, false); // Write I2C command
  readData(buffer, len); // Read data from register READ_POUT
  Serial.print("Manufacturing date (YYWW): ");
  for (uint8_t i = 1; i < len; i++)
  {
    Serial.print((char)buffer[i]);
  }
  Serial.println("");
}

void Vicor::print_mfr_serial() {
  uint8_t len = 16 + 1;
  uint8_t buffer[len];
  write(VICOR_CMD_MFR_SERIAL, 1, false); // Write I2C command
  readData(buffer, len); // Read data from register READ_POUT
  Serial.print("Serial Number: ");
  for (uint8_t i = 1; i < len; i++)
  {
    if ((char) buffer[i] != '\0') Serial.print((char) buffer[i]);
  }
  Serial.println("");
}

void Vicor::print_vin_ov_fault() {
  if (_page != 0x01) write_PAGE(0x01); // Ensures we can write.
  //delay(2000);
  uint8_t len = 2;
  uint8_t buffer[len];
  /*write(VICOR_CMD_VIN_OV_FAULT_LIMIT, 1, false); // Write I2C command
  delay(3);
  readData(buffer, len); // Read data from register READ_POUT
  Serial.print("VIN OV FAULT: ");
  uint16_t data = (buffer[0] << 8) | buffer[1];
  Serial.println(data, HEX); // Combine high byte and low byte.*/

  clear_faults();
  //read_CML();

  /*if (_page != 0x01) write_PAGE(0x01); // Ensures we can write.
  uint8_t buffer2[3] = {VICOR_CMD_VIN_OV_FAULT_LIMIT, 0x05, 0x00}; // buffer for 2 bytes
  write(buffer2, 3, true); // Write I2C command*/
  write(0x57, 1, false); // Write I2C command
  delay(200);
  readData(buffer, len); // Read data from register READ_POUT
  Serial.println("VIN OV FAULT: ");
  uint16_t data = 0;
  data = (buffer[0] << 8) | buffer[1];
  Serial.println(data, HEX); // Combine high byte and low byte.
}

void Vicor::disable_faults() {

  if (_page != 0x01) write_PAGE(0x01); // Ensures we can both Read and Write.
  uint8_t write_buffer[3] = {DISABLE_FAULT, 0x00};
  write(write_buffer, 3, true); // Write I2C command
}