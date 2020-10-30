#include <stdbool.h>
#include "vicor.h"

BCM4414 bcm; // Global BCM variable.


/**
 * @brief Read an amount of data in blocking mode from a specific memory address.
 * @param hi2c Pointer to a I2C_HandleTypeDef structure that contains
 *                the configuration information for the specified I2C.
 * @param cmd Command code byte
 * @param buffer Buffer to store read data.
 * @param nbytes number of bytes to read.
 */
static bool pmbus_read(I2C_HandleTypeDef *hi2c, uint8_t cmd, uint8_t *buffer, uint8_t nbytes);

/**
 * @brief Read an amount of data in blocking mode from a specific memory address.
 * @param hi2c Pointer to a I2C_HandleTypeDef structure that contains
 *                the configuration information for the specified I2C.
 * @param cmd Command code byte
 * @param buffer Buffer which stores the data to write.
 * @param nbytes number of bytes to write.
 */
static void pmbus_write(I2C_HandleTypeDef *hi2c, uint8_t cmd, uint8_t *buffer);

/**
 * @brief helper function to debug HAL status.
 * @param error HAL_StatusTypeDef to debug.
 */
static void check_hal_error(HAL_StatusTypeDef error);

static bool pmbus_read(I2C_HandleTypeDef *hi2c, uint8_t cmd, uint8_t *buffer, uint8_t nbytes) {
    const uint8_t address = DEVICE_ADDRESS << 1;
    const uint8_t cmd_size = 1;
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(hi2c, address, cmd, cmd_size, buffer, nbytes, I2C_TIMEOUT);

    if (status != HAL_OK) {
        check_hal_error(status);
        return false;
    }

    return true;
}

static void pmbus_write(I2C_HandleTypeDef *hi2c, uint8_t cmd, uint8_t *buffer) {
    const uint8_t address = DEVICE_ADDRESS << 1;
    const uint8_t cmd_size = 1;
    const uint16_t buff_size = sizeof(buffer);
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(hi2c, address, cmd, cmd_size, buffer, buff_size, I2C_TIMEOUT);

    if (status != HAL_OK) check_hal_error(status);
}

static void check_hal_error(HAL_StatusTypeDef error) {
    char msg[16];
    switch (error)
    {
    case HAL_ERROR:
        sprintf(msg, "HAL Error.\r\n");
        break;
    case HAL_BUSY:
        sprintf(msg, "HAL Busy.\r\n");
        break;
    case HAL_TIMEOUT:
        sprintf(msg, "HAL Timeout.\r\n");
        break;

    default:
        // Keep default to ignore warning about HAL_OK case.
        break;
    }
}

void read_status_cml(I2C_HandleTypeDef *hi2c) {
    uint8_t buffer[1] = {0};
    bool status = pmbus_read(hi2c, VICOR_CMD_STATUS_CML, buffer, 1);

    if (status != false)
    {
        bcm.status_cml.all = buffer[0];
    }
}

void read_status_specific(I2C_HandleTypeDef *hi2c) {
    uint8_t buffer[1] = {0};
    bool status = pmbus_read(hi2c, VICOR_CMD_STATUS_MFR_SPECIFIC, buffer, 1);

    if (status != false)
    {
        bcm.status_specific.all = buffer[0];
    }
}

void read_status_iout(I2C_HandleTypeDef *hi2c) {
    uint8_t buffer[1] = {0};
    bool status = pmbus_read(hi2c, VICOR_CMD_STATUS_IOUT, buffer, 1);

    if (status != false)
    {
        bcm.status_iout.all = buffer[0];
    }
}

void read_status_input(I2C_HandleTypeDef *hi2c) {
    uint8_t buffer[1] = {0};
    bool status = pmbus_read(hi2c, VICOR_CMD_STATUS_INPUT, buffer, 1);

    if (status != false)
    {   
        bcm.status_input.all = buffer[0];
    }
}

void read_temperature(I2C_HandleTypeDef *hi2c) {
    uint8_t buffer[2] = {0};
    bool status = pmbus_read(hi2c, VICOR_CMD_READ_TEMPERATURE_1, buffer, 2);

    if (status != false)
    {
        bcm.measurements.temperature = (buffer[1] << 8) | buffer[0];
    }
}

void read_iout(I2C_HandleTypeDef *hi2c) {
    uint8_t buffer[2] = {0};
    bool status = pmbus_read(hi2c, VICOR_CMD_READ_IOUT, buffer, 2);

    if (status != false)
    {
        bcm.measurements.iout = (buffer[1] << 8) | buffer[0];
    }
}

void read_iin(I2C_HandleTypeDef *hi2c) {
    uint8_t buffer[2] = {0};
    bool status = pmbus_read(hi2c, VICOR_CMD_READ_IIN, buffer, 2);

    if (status != false)
    {
        bcm.measurements.iin = (buffer[1] << 8) | buffer[0];
    }
}

void read_pout(I2C_HandleTypeDef *hi2c) {
    uint8_t buffer[2] = {0};
    bool status = pmbus_read(hi2c, VICOR_CMD_READ_POUT, buffer, 2);

    if (status != false)
    {
        bcm.measurements.pout = (buffer[1] << 8) | buffer[0];
    }
}

void read_vin(I2C_HandleTypeDef *hi2c) {
    uint8_t buffer[2] = {0};
    bool status = pmbus_read(hi2c, VICOR_CMD_READ_VIN, buffer, 2);

    if (status != false)
    {
        bcm.measurements.vin = (buffer[1] << 8) | buffer[0];
    }
}

void read_ton_delay(I2C_HandleTypeDef *hi2c){
    uint8_t buffer[2] = {0};
    pmbus_read(hi2c, VICOR_CMD_TON_DELAY, buffer, 2);

    bcm.measurements.tdelay = (buffer[1] << 8) | buffer[0];
}

void read_vout(I2C_HandleTypeDef *hi2c) {
    uint8_t buffer[2] = {0};
    bool status = pmbus_read(hi2c, VICOR_CMD_READ_VOUT, buffer, 2);

    if (status != false)
    {
        bcm.measurements.vout = (buffer[1] << 8) | buffer[0];
    }
}

void read_rout(I2C_HandleTypeDef *hi2c) {
    uint8_t buffer[2] = {0};
    bool status = pmbus_read(hi2c, VICOR_CMD_READ_BCM_ROUT, buffer, 2);

    if (status != false)
    {
        bcm.measurements.rout = (buffer[1] << 8) | buffer[0];
    }
}

// This command clears all status bits that have been previously set.
// Persistent or active faults are re-asserted again once cleared.
//  All faults are latched once asserted in the BCM controller. Registered
// faults will not be cleared when shutting down the BCM
// powertrain by recycling the BCM high side voltage or sending
// the OPERATION command
void clear_faults(I2C_HandleTypeDef *hi2c) {
    uint8_t buffer[1] = {VICOR_CMD_CLEAR_FAULTS};
    const uint8_t address = DEVICE_ADDRESS << 1;
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(hi2c, address, buffer, 1, I2C_TIMEOUT);

    if (status != HAL_OK) check_hal_error(status);
}