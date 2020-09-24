#include "vicor.h"

Vicor::Vicor(TwoWire *pWire, uint8_t address){
    _pWire   = pWire;
    _address = address;
}

int Vicor::begin() 
{
  _pWire->begin(DEFAULT_ADDRESS);
  _pWire->setClock(400000); // Set i2c clock frequency to 400kHz
  /*uint8_t data[2];
  if(readSerialNumber() == 0){
    Serial.println("bus data access error");
    return ERR_DATA_BUS;
   }*/
  return ERR_OK;
}

Vicor::sStatusData_t Vicor::read_status_word() {
  sStatusData_t status_word;
  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_STATUS_WORD, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_POUT
  uint16_t data = (rawData[1]<<8) | rawData[0];
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

uint16_t Vicor::get_READ_VIN() {
  convert_raw_READ_VIN();
  return sData_t.vin;
}

uint16_t Vicor::get_READ_IIN() {
  convert_raw_READ_IIN();
  return sData_t.iin;
}

uint16_t Vicor::get_READ_VOUT() {
  convert_raw_READ_VOUT();
  return sData_t.vout;
}

uint16_t Vicor::get_READ_IOUT() {
  convert_raw_READ_IOUT();
  return sData_t.iout;
}

uint16_t Vicor::get_READ_BCM_ROUT() {
  convert_raw_READ_BCM_ROUT();
  return sData_t.rout;
}

uint16_t Vicor::get_READ_TEMPERATURE_1() {
  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_READ_TEMPERATURE_1, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_TEMPERATURE_1
  sData_t.temperature = (rawData[1] << 8) | rawData[0]; // Combine high byte and low byte.
  return sData_t.temperature;
}

uint16_t Vicor::get_READ_K_FACTOR() {
  convert_raw_READ_K_FACTOR();
  return sData_t.kfactor;
}

uint16_t Vicor::get_TON_DELAY() {
  convert_raw_TON_DELAY();
  return sData_t.tdelay;
}

uint16_t Vicor::get_READ_POUT() {
  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_READ_POUT, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_POUT

  sData_t.pout = (rawData[1] << 8) | rawData[0]; // Combine high byte and low byte.
  return sData_t.pout;
}

uint16_t Vicor::get_MFR_VIN_MIN() {
  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_MFR_VIN_MIN, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_POUT

  uint16_t data = (rawData[1] << 8) | rawData[0]; // Combine high byte and low byte.
  return data;
}

void Vicor::write_PAGE(uint8_t data_byte){
  uint8_t rawData[2] = {VICOR_CMD_PAGE, data_byte};
  write(rawData, 2, true);
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
  _pWire->beginTransmission(_address); // Start conditon + Device Address + Write bit
  _pWire->write(pBuf, size);
  uint8_t ret = _pWire->endTransmission(sendStop); // use false to not send stop condition

  if (ret != 0)
  {
    Serial.println("Error in endTransmission().");
  }
}

uint8_t Vicor::readData(void *pBuf, size_t size) {
  if (pBuf == nullptr) {
    Serial.println("pBuf ERROR!! : null pointer");
  }
  uint8_t * _pBuf = (uint8_t *)pBuf;
  _pWire->requestFrom(_address, (uint8_t)size, (uint8_t)true);

  uint8_t len = size;
  for (uint8_t i = 0 ; i < size; i++) {
    _pBuf[i] = _pWire->read();
    len--;
  }

  return len;
}

void Vicor::convert_raw_READ_VIN() {
  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_READ_VIN, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_VIN
  sData_t.vin = (rawData[1] << 8) | rawData[0]; // Combine high byte and low byte.
  sData_t.vin = sData_t.vin / 10; // V_actual = V_reported * 10^-1 (V)
}

void Vicor::convert_raw_READ_IIN() {
  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_READ_IIN, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_IIN
  sData_t.iin = (rawData[1] << 8) | rawData[0]; // Combine high byte and low byte.
  sData_t.iin = sData_t.iin / 1000; // I_actual = I_reported * 10^-3 (A)
}

void Vicor::convert_raw_READ_VOUT() {
  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_READ_VOUT, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_VOUT
  sData_t.vout = (rawData[1] << 8) | rawData[0]; // Combine high byte and low byte.
  sData_t.vout = sData_t.vout / 10; // V_actual = V_reported * 10^-1 (V)
}

void Vicor::convert_raw_READ_IOUT() {
  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_READ_IOUT, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_IOUT
  sData_t.iout = (rawData[1] << 8) | rawData[0]; // Combine high byte and low byte.
  sData_t.iout = sData_t.iout / 100; // I_actual = I_reported * 10^-2 (A)
}

void Vicor::convert_raw_READ_BCM_ROUT() {
  //write_PAGE(page_zero); // Resistance is only readable if Page is 0x01.
  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_READ_BCM_ROUT, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_BCM_ROUT
  sData_t.rout = (rawData[1] << 8) | rawData[0]; // Combine high byte and low byte.
  sData_t.rout = sData_t.rout / 100000; // R_actual = R_reported * 10^-5 (Ohms)
}

void Vicor::convert_raw_READ_K_FACTOR() {
  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_READ_K_FACTOR, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register READ_K_FACTOR
  sData_t.kfactor = (rawData[1] << 8) | rawData[0]; // Combine high byte and low byte.
  sData_t.kfactor = sData_t.kfactor / 65536; // K_actual = K_reported * 2^-16 (V/V)
}

void Vicor::convert_raw_TON_DELAY() {
  uint8_t rawData[2]; // buffer for 2 bytes
  write(VICOR_CMD_TON_DELAY, 1, false); // Write I2C command
  readData(rawData, 2); // Read data from register TON_DELAY
  sData_t.tdelay = (rawData[1] << 8) | rawData[0]; // Combine high byte and low byte.
  sData_t.tdelay = sData_t.tdelay / 1000; // t_actual = t_reported * 10^-3 (seconds)
}

void Vicor::get_PMBUS_REVISION() {
  uint8_t rawData[1]; // buffer for 2 bytes
  write(VICOR_CMD_PMBUS_REVISION, 1, false); // Write I2C command
  uint8_t ret = readData(rawData, 1); // Read data from register READ_POUT
  Serial.println(rawData[0], HEX);
}