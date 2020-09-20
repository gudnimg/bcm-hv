#include "vicor.h"

Vicor::Vicor(TwoWire *pWire, uint8_t address){
    _pWire   = pWire;
    _address = address;
}

int Vicor::begin() 
{
  _pWire->begin();
  /*uint8_t data[2];
  if(readSerialNumber() == 0){
    Serial.println("bus data access error");
    return ERR_DATA_BUS;
   }*/
  return ERR_OK;
}

uint32_t Vicor::readSerialNumber()
{
    uint32_t result = 0 ;
    uint8_t serialNumber1[3];
    uint8_t serialNumber2[3];
    uint8_t rawData[6];
    writeCommand(VICOR_CMD_MFR_SERIAL, 2);
    delay(1);
    readData(rawData,6);
    memcpy(serialNumber1,rawData,3);
    memcpy(serialNumber2,rawData+3,3);
    result = serialNumber1[0];
    result = (result << 8) | serialNumber1[1];
    result = (result << 8) | serialNumber2[0];
    result = (result << 8) | serialNumber2[1];
  return result;
}

uint16_t Vicor::get_READ_VIN() {
  convert_raw_READ_VIN();
  return sData_t.vin;
}

uint16_t Vicor::get_READ_IIN() {
  convert_raw_READ_IIN();
  return sData_t.iin;
}

void Vicor::writeCommand(uint16_t cmd, size_t size)
{
  uint8_t _pBuf[2];
  _pBuf[0] = cmd >> 8;
  _pBuf[1] = cmd & 0xFF;
  delay(1);
  write(_pBuf,2);
}

void Vicor::write(const void* pBuf,size_t size)
{
  if (pBuf == NULL) {
    Serial.println("pBuf ERROR!! : null pointer");
  }
  uint8_t * _pBuf = (uint8_t *)pBuf;
  _pWire->beginTransmission(_address);
  for (uint8_t i = 0; i < size; i++) {
    _pWire->write(_pBuf[i]);
  }
  _pWire->endTransmission();
}

uint8_t Vicor::readData(void *pBuf, size_t size) {
  if (pBuf == NULL) {
    Serial.println("pBuf ERROR!! : null pointer");
  }
  uint8_t * _pBuf = (uint8_t *)pBuf;
  _pWire->requestFrom(_address, size);
  uint8_t len = 0;
  for (uint8_t i = 0 ; i < size; i++) {
    _pBuf[i] = _pWire->read();
    len++;
  }
  _pWire->endTransmission();
  return len;
}

void Vicor::convert_raw_READ_VIN() {
  uint8_t rawData[2]; // buffer for 2 bytes
  writeCommand(VICOR_CMD_READ_VIN, 2); // Write I2C command
  delay(1);
  readData(rawData, 2); // Read data from register READ_VIN
  sData_t.vin = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  sData_t.vin = sData_t.vin / 10; // V_actual = V_reported * 10^-1 (V)
}

void Vicor::convert_raw_READ_IIN() {
  uint8_t rawData[2]; // buffer for 2 bytes
  writeCommand(VICOR_CMD_READ_IIN, 2); // Write I2C command
  delay(1);
  readData(rawData, 2); // Read data from register READ_IIN
  sData_t.iin = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  sData_t.iin = sData_t.iin / 1000; // I_actual = I_reported * 10^-3 (A)
}

void Vicor::convert_raw_READ_VOUT() {
  uint8_t rawData[2]; // buffer for 2 bytes
  writeCommand(VICOR_CMD_READ_VOUT, 2); // Write I2C command
  delay(1);
  readData(rawData, 2); // Read data from register READ_VOUT
  sData_t.vout = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  sData_t.vout = sData_t.vout / 10; // V_actual = V_reported * 10^-1 (V)
}

void Vicor::convert_raw_READ_IOUT() {
  uint8_t rawData[2]; // buffer for 2 bytes
  writeCommand(VICOR_CMD_READ_IOUT, 2); // Write I2C command
  delay(1);
  readData(rawData, 2); // Read data from register READ_IOUT
  sData_t.iout = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  sData_t.iout = sData_t.iout / 100; // I_actual = I_reported * 10^-2 (A)
}

void Vicor::convert_raw_READ_BCM_ROUT() {
  uint8_t rawData[2]; // buffer for 2 bytes
  writeCommand(VICOR_CMD_READ_BCM_ROUT, 2); // Write I2C command
  delay(1);
  readData(rawData, 2); // Read data from register READ_BCM_ROUT
  sData_t.rout = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  sData_t.rout = sData_t.rout / 100000; // R_actual = R_reported * 10^-5 (Ohms)
}

void Vicor::convert_raw_READ_K_FACTOR() {
  uint8_t rawData[2]; // buffer for 2 bytes
  writeCommand(VICOR_CMD_READ_K_FACTOR, 2); // Write I2C command
  delay(1);
  readData(rawData, 2); // Read data from register READ_BCM_ROUT
  sData_t.kfactor = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  sData_t.kfactor = sData_t.kfactor / 65536; // K_actual = K_reported * 2^-16 (V/V)
}

void Vicor::convert_raw_TON_DELAY() {
  uint8_t rawData[2]; // buffer for 2 bytes
  writeCommand(VICOR_CMD_TON_DELAY, 2); // Write I2C command
  delay(1);
  readData(rawData, 2); // Read data from register READ_BCM_ROUT
  sData_t.tdelay = (rawData[0] << 8) | rawData[1]; // Combine high byte and low byte.
  sData_t.tdelay = sData_t.tdelay / 1000; // t_actual = t_reported * 10^-3 (seconds)
}