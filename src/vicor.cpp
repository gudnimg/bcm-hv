#include "vicor.h"

Vicor::Vicor(TwoWire *pWire, uint8_t address){
    _pWire   = pWire;
    _address = address;
    sStatusData_t     status_data;
    sStatusIOUT_t     iout_status;
    sStatusInput_t    input_status;
    sStatusCML_t      cml_status;
    sStatusSpecific_t specific_mfr_status;
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