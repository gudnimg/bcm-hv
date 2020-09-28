#include "vicor.h"
#include <stdio.h>

Vicor::Vicor(TwoWire *pWire, uint8_t address){
    _pWire   = pWire;
    _address = address;

    // Initalise structures
    memset(&sStatusIOUT_t, DEFAULT_STATUS_IOUT, sizeof sStatusIOUT_t);
    memset(&sStatusInput_t, DEFAULT_STATUS_INPUT, sizeof sStatusInput_t);
    memset(&sStatusTemperature_t, DEFAULT_STATUS_TEMPERATURE, sizeof sStatusTemperature_t);
    memset(&sStatusCML_t, DEFAULT_STATUS_CML, sizeof sStatusCML_t);
    memset(&sStatusSpecific_t, DEFAULT_STATUS_MFR_SPECIFIC, sizeof sStatusSpecific_t);
    memset(&sData_t, 0, sizeof sData_t);
}

int Vicor::begin() 
{
  _pWire->begin(DEFAULT_ADDRESS);
  _pWire->setClock(400000); // Set i2c clock frequency to 400kHz
  // TODO: add check here to see if device is active.
  return ERR_OK;
}

Vicor::sStatusData_t Vicor::read_status_word() {
  sStatusData_t status_word;
  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_STATUS_WORD, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_POUT
  uint16_t data = (rawData[0]<<8) | rawData[1];
  memcpy(&status_word, &data, 2);
  Serial.print("Input Fault: ");
  Serial.println(status_word.input_fault);
  Serial.print("iout_oc_fault: ");
  Serial.println(status_word.iout_oc_fault);
  Serial.print("iout_pout_fault: ");
  Serial.println(status_word.iout_pout_fault);
  Serial.print("pmbus_communication_event: ");
  Serial.println(status_word.pmbus_communication_event);
  Serial.print("power_good: ");
  Serial.println(status_word.power_good);
  Serial.print("status_mfr_specific: ");
  Serial.println(status_word.status_mfr_specific);
  Serial.print("temperature_fault: ");
  Serial.println(status_word.temperature_fault);
  Serial.print("unit_is_busy: ");
  Serial.println(status_word.unit_is_busy);
  Serial.print("unit_is_off: ");
  Serial.println(status_word.unit_is_off);
  Serial.print("vin_uv_fault: ");
  Serial.println(status_word.vin_uv_fault);

  return status_word;
}

float Vicor::get_READ_VIN() {
  convert_raw_READ_VIN();
  return sData_t.vin;
}

float Vicor::get_READ_IIN() {
  convert_raw_READ_IIN();
  return sData_t.iin;
}

float Vicor::get_READ_VOUT() {
  convert_raw_READ_VOUT();
  return sData_t.vout;
}

float Vicor::get_READ_IOUT() {
  convert_raw_READ_IOUT();
  return sData_t.iout;
}

float Vicor::get_READ_BCM_ROUT() {
  convert_raw_READ_BCM_ROUT();
  return sData_t.rout;
}

int16_t Vicor::get_READ_TEMPERATURE_1() {
  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_READ_TEMPERATURE_1, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_TEMPERATURE_1
  sData_t.temperature = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  return sData_t.temperature;
}

float Vicor::get_READ_K_FACTOR() {
  convert_raw_READ_K_FACTOR();
  return sData_t.kfactor;
}

float Vicor::get_TON_DELAY() {
  convert_raw_TON_DELAY();
  return sData_t.tdelay;
}

int16_t Vicor::get_READ_POUT() {
  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_READ_POUT, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_POUT

  sData_t.pout = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  return sData_t.pout;
}

int16_t Vicor::get_MFR_VIN_MIN() {
  byte rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_MFR_VIN_MIN, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_POUT

  int16_t data = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  return data;
}

int16_t Vicor::get_MFR_VIN_MAX() {
  byte rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_MFR_VIN_MAX, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_POUT

  int16_t data = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  return data;
}

int16_t Vicor::get_MFR_VOUT_MIN() {
  byte rawData[2]; // buffer for 2 bytes
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
void Vicor::write(uint8_t pBuf, size_t size, bool sendStop) {
  write(&pBuf, size, sendStop);
}

void Vicor::write(uint8_t* pBuf, size_t size, bool sendStop)
{
  if (pBuf == NULL) {
    Serial.println("pBuf ERROR!! : null pointer");
  }
  //retry_write:
  _pWire->beginTransmission(_address); // Start conditon + Device Address + Write bit
  for(size_t i = 0; i < size; ++i){
    _pWire->write(pBuf[i]);
    delay(1);
  }
  uint8_t ret = _pWire->endTransmission(sendStop); // use false to not send stop condition

  if (ret != 0)
  {
    /*Serial.print("Error ");
    Serial.print(ret);
    Serial.println(" in endTransmission().");*/
    //goto retry_write;
  }
}

void Vicor::write_PAGE(uint8_t data_byte){
  uint8_t rawData[2] = {VICOR_CMD_PAGE, data_byte};
  write(rawData, 2, true);

  // Read back PAGE byte.
  uint8_t buffer[1]; 
  write((uint8_t)VICOR_CMD_PAGE, 1, false); // Write I2C command
  readData(buffer, 1); // Read data from register READ_POUT
  if (buffer[0] != data_byte)
  {
    Serial.println("Error. Writing page data byte failed.");
  }
  
}

uint8_t Vicor::readData(uint8_t *pBuf, uint8_t size) {
  if (pBuf == nullptr) {
    Serial.println("pBuf ERROR!! : null pointer");
  }
  uint8_t len = 0;
  //_pWire->beginTransmission(_address);
  _pWire->requestFrom(_address, (uint8_t)size, (uint8_t)true);
  while(_pWire->available())  
  {
    pBuf[len++] = _pWire->read();  
  }

  return len;
}

void Vicor::convert_raw_READ_VIN() {
  uint8_t rawData[2];                           // buffer for 2 bytes
  write(VICOR_CMD_READ_VIN, 1, false);          // Write I2C command
  readData(rawData, 2);                         // Read data from register READ_VIN
  sData_t.vin = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  sData_t.vin = sData_t.vin / 10.f;             // V_actual = V_reported * 10^-1 (V)
}

void Vicor::convert_raw_READ_IIN() {
  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_READ_IIN, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_IIN
  sData_t.iin = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  sData_t.iin = sData_t.iin / 1000.f; // I_actual = I_reported * 10^-3 (A)
}

void Vicor::convert_raw_READ_VOUT() {
  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_READ_VOUT, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_VOUT
  sData_t.vout = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  sData_t.vout = sData_t.vout / 10.f; // V_actual = V_reported * 10^-1 (V)
}

void Vicor::convert_raw_READ_IOUT() {
  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_READ_IOUT, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_IOUT
  sData_t.iout = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  sData_t.iout = sData_t.iout / 100.f; // I_actual = I_reported * 10^-2 (A)
}

void Vicor::convert_raw_READ_BCM_ROUT() {
  //write_PAGE(page_zero); // Resistance is only readable if Page is 0x01.
  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_READ_BCM_ROUT, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_BCM_ROUT
  sData_t.rout = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  sData_t.rout = sData_t.rout / 100000.f; // R_actual = R_reported * 10^-5 (Ohms)
}

void Vicor::convert_raw_READ_K_FACTOR() {
  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_READ_K_FACTOR, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_K_FACTOR
  sData_t.kfactor = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  sData_t.kfactor = sData_t.kfactor / 65536.f; // K_actual = K_reported * 2^-16 (V/V)
}

void Vicor::convert_raw_TON_DELAY() {
  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_TON_DELAY, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register TON_DELAY
  sData_t.tdelay = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  sData_t.tdelay = sData_t.tdelay / 1000.f; // t_actual = t_reported * 10^-3 (seconds)
}

void Vicor::get_PMBUS_REVISION() {
  uint8_t rawData[1]; // buffer for 2 bytes
  write(VICOR_CMD_PMBUS_REVISION, 1, false); // Write I2C command
  uint8_t ret = readData(rawData, 1); // Read data from register READ_POUT
  Serial.print("PMbus Revision: 0x");
  Serial.println(rawData[0], HEX);
}

void Vicor::clear_faults() {
  write(VICOR_CMD_CLEAR_FAULTS, 1, true); // Write I2C command
}

void Vicor::operation() {
  write_PAGE(0x01); // Ensure Page is set correctly.
  uint8_t rawData[2] = {(uint8_t)VICOR_CMD_OPERATION,(uint8_t)0x80}; // buffer for 2 bytes
  write(rawData, 2, true); // Write I2C command

  // Read back Operation mode.
  uint8_t buffer[1];
  write((uint8_t)VICOR_CMD_OPERATION, 1, false); // Write I2C command
  readData(buffer, 1); // Read data from register READ_POUT
  if (bitRead(buffer[0], 0) != 1)
  {
    Serial.println(buffer[0], HEX);
    Serial.println("Error. Failed to turn the Module on.");
  }
}

void Vicor::read_CML() {
  uint8_t rawData[1]; // buffer for 2 bytes
  write(VICOR_CMD_STATUS_CML, 1, false); // Write I2C command
  readData(rawData, 1); // Read data from register READ_POUT
  Serial.print("Status CML: 0x");
  Serial.println(rawData[0], HEX);
}

void Vicor::read_status_mfr() {
  uint8_t rawData[1]; // buffer for 2 bytes
  write(VICOR_CMD_STATUS_MFR_SPECIFIC, 1, false); // Write I2C command
  readData(rawData, 1); // Read data from register READ_POUT
  Serial.print("Status MFR: 0x");
  Serial.println(rawData[0], HEX);
}

void Vicor::read_status_iout() {
  uint8_t rawData[1]; // buffer for 2 bytes
  write(VICOR_CMD_STATUS_IOUT, 1, false); // Write I2C command
  readData(rawData, 1); // Read data from register READ_POUT
  Serial.print("Status Iout: 0x");
  Serial.println(rawData[0], HEX);
}

void Vicor::read_status_input() {
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
  uint8_t rawData[1]; // buffer for 2 bytes
  write(VICOR_CMD_CAPABILITY, 1, false); // Write I2C command
  readData(rawData, 1); // Read data from register READ_POUT
  Serial.print("Capability: 0x");
  Serial.println(rawData[0], HEX);
}

void Vicor::write_time_on_delay() {
  uint8_t buffer[3] = {VICOR_CMD_TON_DELAY, 0x64, 0x00}; // buffer for 2 bytes
  write(buffer, 3, true); // Write I2C command
}

void Vicor::print_mfr_id() {
  uint8_t len = 2 + 1;
  uint8_t buffer[len];
  write(VICOR_CMD_MFR_ID, 1, false); // Write I2C command
  readData(buffer, len); // Read data from register READ_POUT
  Serial.print("MFR ID: ");
  for (int8_t i = 1; i < len; i++)
  {
    if ((char) buffer[i] != NULL) Serial.print((char) buffer[i]);
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
    if ((char) buffer[i] != NULL) Serial.print((char) buffer[i]);
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
    if ((char) buffer[i] != NULL) Serial.print((char) buffer[i]);
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
    if ((char) buffer[i] != NULL) Serial.print((char) buffer[i]);
  }
  Serial.println("");
}