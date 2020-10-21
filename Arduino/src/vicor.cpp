#include "vicor.h"
#include <stdio.h>

Vicor::Vicor(uint8_t address) {
  begin(&Wire, address);
}

Vicor::Vicor(TwoWire *pWire, uint8_t address){
  begin(pWire, address);
}

int Vicor::begin(TwoWire *pWire, uint8_t address) 
{
  _pWire   = pWire;
  _address = address;
  _page    = DEFAULT_PAGE;

  _pWire->begin(_address);
  _pWire->setClock(400000); // Set i2c clock frequency to 400kHz
  // TODO: add check here to see if device is active.
  
  // Initalise structures
  memset(&status_iout, DEFAULT_STATUS_IOUT, sizeof(status_iout));
  memset(&status_input, DEFAULT_STATUS_INPUT, sizeof status_input);
  memset(&status_temperature, DEFAULT_STATUS_TEMPERATURE, sizeof(status_temperature));
  memset(&status_cml, DEFAULT_STATUS_CML, sizeof status_cml);
  memset(&status_specific, DEFAULT_STATUS_MFR_SPECIFIC, sizeof status_specific);
  measurement_data = {0};
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
  // Note: cannot write data to this register.
  // Page must be 0

  if (_page != 0x00) {
    uint8_t error = write_PAGE(0x00); // Ensures we can both Read and Write.
    if (error != 1) return 0;
  }

  uint8_t len = 2;
  uint8_t rawData[len]; // buffer for 2 bytes
  write(VICOR_CMD_READ_TEMPERATURE_1, 1, false); // Write I2C command
  readData(rawData, len); // Read data from register READ_TEMPERATURE_1

  measurement_data.temperature = (rawData[1] << 8) | rawData[0]; // Combine high byte and low byte.
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
  // Note: cannot write data to this register.
  // Page must be 0

  if (_page != 0x00) {
    uint8_t error = write_PAGE(0x00); // Ensures we can both Read and Write.
    if (error != 1) return 0;
  }

  uint8_t len = 2;
  uint8_t rawData[len]; // buffer for 2 bytes
  write(VICOR_CMD_READ_POUT, 1, false); // Write I2C command
  readData(rawData, len); // Read data from register READ_POUT

  measurement_data.pout = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  return measurement_data.pout;
}

int16_t Vicor::get_MFR_VIN_MIN() {
  // Note: cannot write data to this register.
  // Page must be 0

  if (_page != 0x00) {
    uint8_t error = write_PAGE(0x00); // Ensures we can both Read and Write.
    if (error != 1) return 0;
  }

  byte rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_MFR_VIN_MIN, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_POUT

  int16_t data = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  return data;
}

int16_t Vicor::get_MFR_VIN_MAX() {
  // Note: cannot write data to this register.
  // Page must be 0

  if (_page != 0x00) {
    uint8_t error = write_PAGE(0x00); // Ensures we can both Read and Write.
    if (error != 1) return 0;
  }

  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_MFR_VIN_MAX, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_POUT

  int16_t data = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  return data;
}

int16_t Vicor::get_MFR_VOUT_MIN() {
  // Note: cannot write data to this register.
  // Page must be 0

  if (_page != 0x00) {
    uint8_t error = write_PAGE(0x00); // Ensures we can both Read and Write.
    if (error != 1) return 0;
  }

  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_MFR_VOUT_MIN, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_POUT

  int16_t data = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  return data;
}

int16_t Vicor::get_MFR_VOUT_MAX() {
  // Note: cannot write data to this register.
  // Page must be 0

  if (_page != 0x00) {
    uint8_t error = write_PAGE(0x00); // Ensures we can both Read and Write.
    if (error != 1) return 0;
  }

  uint8_t len = 2;
  byte rawData[len]; // buffer for 2 bytes
  write(VICOR_CMD_MFR_VOUT_MAX, 1, false); // Write I2C command
  readData(rawData, len); // Read data from register READ_POUT

  int16_t data = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  return data;
}

int16_t Vicor::get_MFR_IOUT_MAX() {
  // Note: cannot write data to this register.
  // Page must be 0

  if (_page != 0x00) {
    uint8_t error = write_PAGE(0x00); // Ensures we can both Read and Write.
    if (error != 1) return 0;
  }

  uint8_t len = 2;
  uint8_t rawData[len]; // buffer for 2 bytes
  write(VICOR_CMD_MFR_IOUT_MAX, 1, false); // Write I2C command
  readData(rawData, len); // Read data from register READ_POUT

  int16_t data = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  return data;
}

int16_t Vicor::get_MFR_POUT_MAX() {
  // Note: cannot write data to this register.
  // Page must be 0

  if (_page != 0x00) {
    uint8_t error = write_PAGE(0x00); // Ensures we can both Read and Write.
    if (error != 1) return 0;
  }

  uint8_t len = 2;
  uint8_t rawData[len]; // buffer for 2 bytes
  write(VICOR_CMD_MFR_POUT_MAX, 1, false); // Write I2C command
  readData(rawData, len); // Read data from register READ_POUT

  int16_t data = (rawData[1] << 8) | rawData[0]; // Combine high byte and low byte.
  return data;
}

// This overload allows a single byte as input for pBuf.
uint8_t Vicor::write(uint8_t pBuf, size_t size, bool sendStop) {
  return write(&pBuf, size, sendStop);
}

uint8_t Vicor::write(uint8_t* pBuf, size_t size, bool sendStop)
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
    delay(1000);
    return false;
  }

  _page = data_byte; // Update page data byte.
  return true;
}

uint8_t Vicor::readData(uint8_t *pBuf, uint8_t size) {
  uint8_t len = 0;
  
  _pWire->requestFrom(_address, size, (uint8_t)true);
  while(_pWire->available())  
  {
    pBuf[len++] = _pWire->read();  
  }

  return len;
}

void Vicor::convert_raw_READ_VIN() {
  if (_page != 0x01) {
    uint8_t error = write_PAGE(0x01); // VIN can only be read when page is 0x01.
    if (error != 1) return;
  }

  uint8_t len = 2;
  uint8_t rawData[len];                           // buffer for 2 bytes
  write(VICOR_CMD_READ_VIN, 1, false);          // Write I2C command
  readData(rawData, len);                         // Read data from register READ_VIN
  measurement_data.vin = (rawData[1] << 8) | rawData[0]; // Combine high byte and low byte.
  measurement_data.vin /= 10.f;             // V_actual = V_reported * 10^-1 (V)
}

void Vicor::convert_raw_READ_IIN() {
  // Note: cannot write data to this register.
  // Page must be 0

  if (_page != 0x00) {
    uint8_t error = write_PAGE(0x00); // Ensures we can both Read and Write.
    if (error != 1) return;
  }

  uint8_t len = 2;
  uint8_t rawData[len]; // buffer for 2 bytes
  write(VICOR_CMD_READ_IIN, 1, false); // Write I2C command
  readData(rawData, len); // Read data from register READ_IIN
  measurement_data.iin = (rawData[1] << 8) | rawData[0]; // Combine high byte and low byte.
  measurement_data.iin /= 1000.f; // I_actual = I_reported * 10^-3 (A)
}

void Vicor::convert_raw_READ_VOUT() {
  if (_page != 0x01) {
    uint8_t error = write_PAGE(0x01); // VOUT can only be read when page is 0x01.
    if (error != 1) return;
  }

  uint8_t len = 2;
  uint8_t rawData[len]; // buffer for 2 bytes
  write(VICOR_CMD_READ_VOUT, 1, false); // Write I2C command
  readData(rawData, len); // Read data from register READ_VOUT
  measurement_data.vout = (rawData[1] << 8) | rawData[0]; // Combine high byte and low byte.
  measurement_data.vout /= 10.f; // V_actual = V_reported * 10^-1 (V)
}

void Vicor::convert_raw_READ_IOUT() {
  // Note: cannot write data to this register.
  // Page must be 0

  if (_page != 0x00) {
    uint8_t error = write_PAGE(0x00); // Ensures we can both Read and Write.
    if (error != 1) return;
  }

  uint8_t len = 2;
  uint8_t rawData[len]; // buffer for 2 bytes
  write(VICOR_CMD_READ_IOUT, 1, false); // Write I2C command
  readData(rawData, len); // Read data from register READ_IOUT
  measurement_data.iout = ((rawData[0] << 8) | rawData[1]); // Combine high byte and low byte.
  measurement_data.iout /= 100.f; // I_actual = I_reported * 10^-2 (A)
}

void Vicor::convert_raw_READ_BCM_ROUT() {
  if (_page != 0x01) {
    uint8_t error = write_PAGE(0x01); // Ensures we can both Read and Write.
    if (error != 1) return;
  }

  uint8_t len = 2;
  uint8_t rawData[len]; // buffer for 2 bytes
  uint8_t error = write(VICOR_CMD_READ_BCM_ROUT, 1, false); // Write I2C command

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
  }

  readData(rawData, len); // Read data from register READ_BCM_ROUT
  measurement_data.rout = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  measurement_data.rout /= 100000.f; // R_actual = R_reported * 10^-5 (Ohms)
}

void Vicor::convert_raw_READ_K_FACTOR() {

  if (_page != 0x01) {
    uint8_t error = write_PAGE(0x01); // Ensures we can both Read and Write.
    if (error != 1) return;
  }

  uint8_t len = 2;
  uint8_t rawData[len]; // buffer for 2 bytes
  write(VICOR_CMD_READ_K_FACTOR, 1, false); // Write I2C command
  readData(rawData, len); // Read data from register READ_K_FACTOR
  measurement_data.kfactor = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  measurement_data.kfactor /= 65536.f; // K_actual = K_reported * 2^-16 (V/V)
}

void Vicor::convert_raw_TON_DELAY() {

  if (_page != 0x01) {
    uint8_t error = write_PAGE(0x01); // Ensures we can both Read and Write.
    if (error != 1) return;
  }

  uint8_t len = 2;
  uint8_t rawData[len]; // buffer for 2 bytes
  write(VICOR_CMD_TON_DELAY, 1, false); // Write I2C command
  readData(rawData, len); // Read data from register TON_DELAY
  measurement_data.tdelay = (rawData[1] << 8) | rawData[0]; // Combine high byte and low byte.
  measurement_data.tdelay /= 1000.f; // t_actual = t_reported * 10^-3 (seconds)
}

void Vicor::get_PMBUS_REVISION() {

  if (_page != 0x00) write_PAGE(0x00); // Ensures we can read.

  uint8_t len = 1;
  uint8_t rawData[len]; // buffer for 2 bytes
  write(VICOR_CMD_PMBUS_REVISION, 1, false); // Write I2C command
  readData(rawData, len); // Read data from register READ_POUT
  Serial.print("PMbus Revision: 0x");
  Serial.println(rawData[0], HEX);
}

void Vicor::clear_faults() {
  uint8_t error = write(VICOR_CMD_CLEAR_FAULTS, 1, true); // Write I2C command

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
  }
}

void Vicor::operation() {

  if (_page != 0x01) {
    uint8_t error = write_PAGE(0x01); // Ensures we can both Read and Write.
    if (error != 1) return;
  }

  // Read back Operation mode.
  uint8_t buffer[2];
  write(VICOR_CMD_OPERATION, 1, false); // Write I2C command
  readData(buffer, 1); // Read data from register READ_POUT*/
  Serial.println(buffer[0], HEX);
  Serial.println(buffer[0] & 0x80, HEX);
  if ((buffer[0] & 0x80) != 0x80) {
    Serial.println("Module is off.");
  } else {
    return;
  }

  /*uint8_t rawData[2] = {VICOR_CMD_OPERATION, 0x80}; // buffer for 2 bytes
  uint8_t error = write(rawData, 2, true); // Write I2C command

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
  }

  // Read back Operation mode.
  write(VICOR_CMD_OPERATION, 1, false); // Write I2C command
  readData(buffer, 1); // Read data from register READ_POUT
  Serial.println(buffer[0], HEX);
  Serial.println(buffer[0] & 0x80, HEX);
  if ((buffer[0] & 0x80) != 0x80)
  {
    Serial.println("Error. Failed to turn the Module on.");
  }*/
}

void Vicor::read_CML() {

  if (_page != 0x00) {
    uint8_t error = write_PAGE(0x00); // Ensures we can both Read and Write.
    if (error != 1) return;
  }

  uint8_t len = 1;
  uint8_t buffer[len];
  uint8_t error = write(VICOR_CMD_STATUS_CML, 1, false); // Write I2C command

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
  }

  readData(buffer, len); // Read data from register READ_POUT
  Serial.print("Status CML: 0x");
  Serial.println(buffer[0], HEX);

  if (buffer[0] & 0x02) Serial.println("CML error: Other Communication Faults.");
  if (buffer[0] & 0x40) Serial.println("CML error: Invalid or Unsupported Data Received.");
  if (buffer[0] & 0x80) Serial.println("CML error: Invalid or Unsupported Command Received.");  
}

void Vicor::read_status_mfr() {
  // Note: cannot write data to this register.
  // Page must be 0

  if (_page != 0x00) {
    uint8_t error = write_PAGE(0x00); // Ensures we can both Read and Write.
    if (error != 1) return;
  }

  uint8_t len = 1;
  uint8_t rawData[len]; // buffer for 2 bytes
  write(VICOR_CMD_STATUS_MFR_SPECIFIC, 1, false); // Write I2C command
  readData(rawData, len); // Read data from register READ_POUT
  Serial.print("Status MFR: 0x");
  Serial.println(rawData[0], HEX);
}

void Vicor::read_status_iout() {
  // Note: cannot write data to this register.
  // Page must be 0

  if (_page != 0x00) {
    uint8_t error = write_PAGE(0x00); // Ensures we can both Read and Write.
    if (error != 1) return;
  }

  uint8_t len = 1;
  uint8_t rawData[len]; // buffer for 2 bytes
  write(VICOR_CMD_STATUS_IOUT, 1, false); // Write I2C command
  readData(rawData, len); // Read data from register READ_POUT
  Serial.print("Status Iout: 0x");
  Serial.println(rawData[0], HEX);
}

void Vicor::read_status_input() {
  // Note: cannot write data to this register.
  // Page must be 0

  if (_page != 0x00) {
    uint8_t error = write_PAGE(0x00); // Ensures we can both Read and Write.
    if (error != 1) return;
  }

  uint8_t len = 1;
  uint8_t rawData[len]; // buffer for 2 bytes
  write(VICOR_CMD_STATUS_INPUT, 1, false); // Write I2C command
  readData(rawData, len); // Read data from register READ_POUT
  Serial.print("Status Input: 0x");
  Serial.println(rawData[0], HEX);
}

void Vicor::read_status_temperature() {
  if (_page != 0x00) {
    uint8_t error = write_PAGE(0x00); // Ensures we can both Read and Write.
    if (error != 1) return;
  }

  uint8_t len = 1;
  uint8_t rawData[len]; // buffer for 2 bytes
  write(VICOR_CMD_STATUS_TEMPERATURE, 1, false); // Write I2C command
  readData(rawData, len); // Read data from register READ_POUT
  Serial.print("Status Temperature: 0x");
  Serial.println(rawData[0], HEX);
}

void Vicor::print_status_word() {

  if (_page != 0x00) {
    uint8_t error = write_PAGE(0x00); // Ensures we can both Read and Write.
    if (error != 1) return;
  }

  uint8_t len = 2;
  uint8_t rawData[len]; // buffer for 2 bytes
  write(VICOR_CMD_STATUS_WORD, 1, false); // Write I2C command
  readData(rawData, len); // Read data from register READ_POUT
  uint16_t data = (rawData[1] << 8) | rawData[0];
  Serial.print("Status Word: 0x");
  Serial.println(data, HEX);
}

void Vicor::print_status_byte() {

  if (_page != 0x00) {
    uint8_t error = write_PAGE(0x00); // Ensures we can both Read and Write.
    if (error != 1) return;
  }

  uint8_t len = 1;
  uint8_t rawData[len]; // buffer for 2 bytes
  uint8_t error = write(VICOR_CMD_STATUS_BYTE, 1, false); // Write I2C command

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
  }

  readData(rawData, len); // Read data from register READ_POUT
  uint8_t data = rawData[0];
  Serial.print("Status byte: 0x");
  Serial.println(data, HEX);
}

void Vicor::read_capability() {

  if (_page != 0x00) {
    uint8_t error = write_PAGE(0x00); // Ensures we can both Read and Write.
    if (error != 1) return;
  }

  uint8_t len = 1;
  uint8_t rawData[len]; // buffer for 2 bytes
  write(VICOR_CMD_CAPABILITY, 1, false); // Write I2C command
  readData(rawData, len); // Read data from register READ_POUT
  Serial.print("Capability: 0x");
  Serial.println(rawData[0], HEX);
}

void Vicor::print_mfr_id() {

  if (_page != 0x00) {
    uint8_t error = write_PAGE(0x00); // Ensures we can both Read and Write.
    if (error != 1) return;
  }

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

  if (_page != 0x00) {
    uint8_t error = write_PAGE(0x00); // Ensures we can both Read and Write.
    if (error != 1) return;
  }

  uint8_t len = 18 + 1;
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

  if (_page != 0x00) {
    uint8_t error = write_PAGE(0x00); // Ensures we can both Read and Write.
    if (error != 1) return;
  }

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

  if (_page != 0x00) {
    uint8_t error = write_PAGE(0x00); // Ensures we can both Read and Write.
    if (error != 1) return;
  }

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

  if (_page != 0x00) {
    uint8_t error = write_PAGE(0x00); // Ensures we can both Read and Write.
    if (error != 1) return;
  }

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

  if (_page != 0x00) {
    uint8_t error = write_PAGE(0x00); // Ensures we can both Read and Write.
    if (error != 1) return;
  }

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
  if (_page != 0x00) {
    uint8_t error = write_PAGE(0x00); // Ensures we can both Read and Write.
    if (error != 1) return;
  }

  uint8_t len = 2;
  uint8_t buffer[len];

  write(0x58, 1, false); // Write I2C command
  delay(200);
  readData(buffer, len); // Read data from register READ_POUT
  Serial.println("VIN OV FAULT: ");
  uint16_t data = (buffer[1] << 8) | buffer[0];
  Serial.println(data, HEX); // Combine high byte and low byte.
}

void Vicor::disable_faults() {

  if (_page != 0x01) {
    uint8_t error = write_PAGE(0x01); // Ensures we can both Read and Write.
    if (error != 1) return;
  }

  uint8_t write_buffer[3] = {DISABLE_FAULT, 0x00};
  write(write_buffer, 3, true); // Write I2C command
}

void Vicor::set_all_thresholds() {

  if (_page != 0x01) {
    uint8_t error = write_PAGE(0x01); // Ensures we can both Read and Write.
    if (error != 1) return;
  }

  uint8_t write_buffer[7] = {SET_ALL_THRESHOLDS, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64};
  write(write_buffer, 7, true); // Write I2C command
}