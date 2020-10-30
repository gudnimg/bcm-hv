/*!
 * @file vicor.h
 * @brief Define the infrastructure of the vicor BCM class
 * @n This is a library for the BCM 600V DC/DC regulator intended for Team Spark.
 * @author Guðni Már Gilbert
 * @version  V0.x
 * @date  2020-11-01
 */

#ifndef VICOR_H
#define VICOR_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"
#include <stdbool.h>

extern UART_HandleTypeDef huart1; // Probably not the best place to put this.

typedef struct  {
    union {
        uint16_t all;

        struct {
            uint8_t unknown_fault       : 1; // NONE OF THE ABOVE
            uint8_t pmbus_communication_event : 1;
            uint8_t temperature_fault   : 1; // TEMPERATURE FAULT OR WARNING
            uint8_t vin_uv_fault        : 1;
            uint8_t iout_oc_fault       : 1;
            uint8_t                     : 1; // UNSUPPORTED - VOUT_OV_FAULT
            uint8_t unit_is_off         : 1;
            uint8_t unit_is_busy        : 1;
            uint8_t                     : 1; // UNSUPPORTED - UNKNOWN FAULT OR WARNING
            uint8_t                     : 1; // UNSUPPORTED - OTHER
            uint8_t                     : 1; // UNSUPPORTED - FAN FAULT OR WARNING
            uint8_t power_good          : 1; // POWER_GOOD Negated*
            uint8_t status_mfr_specific : 1; // STATUS_MFR_SPECIFIC
            uint8_t input_fault         : 1; // INPUT FAULT OR WARNING
            uint8_t iout_pout_fault     : 1; // IOUT/POUT FAULT OR WARNING
            uint8_t                     : 1; // UNSUPPORTED - VOUT FAULT OR WARNING
        };
    };
} sStatusData_t;

typedef struct {
    union {
        uint8_t all;
        
        struct {
            uint8_t                 : 1; // UNSUPPORTED - POUT_OP_WARNING
            uint8_t                 : 1; // UNSUPPORTED - POUT_OP_FAULT
            uint8_t                 : 1; // UNSUPPORTED - In Power Limiting Mode
            uint8_t                 : 1; // UNSUPPORTED - Current Share Fault
            uint8_t                 : 1; // UNSUPPORTED - IOUT_UC_FAULT
            uint8_t iout_oc_warning : 1;
            uint8_t                 : 1; // UNSUPPORTED - IOUT_OC_LV_FAULT
            uint8_t iout_oc_fault   : 1;
        };
    };
} sStatusIOUT_t;

// A one indicates a fault.
typedef struct {
    union {
        uint8_t all;

        struct {
            uint8_t                : 1; // UNSUPPORTED - PIN_OP_WARNING
            uint8_t                : 1; // UNSUPPORTED - IIN_OC_WARNING
            uint8_t                : 1; // UNSUPPORTED - IIN_OC_FAULT
            uint8_t                : 1; // UNSUPPORTED - Unit Off For Insufficient Input Voltage
            uint8_t vin_uv_fault   : 1;
            uint8_t                : 1; // UNSUPPORTED - VIN_UV_WARNING
            uint8_t vin_ov_warning : 1;
            uint8_t vin_ov_fault   : 1;
        };
    };
} sStatusInput_t;

// A one indicates a fault.
typedef struct {
    union {
        uint8_t all;

        struct {
            uint8_t reserved   : 4; // Reserved bits
            uint8_t ut_fault   : 1;
            uint8_t            : 1; // UNSUPPORTED - UT_WARNING
            uint8_t ot_warning : 1;
            uint8_t ot_fault   : 1;
        };
    };
} sStatusTemperature_t;

// The STATUS_CML data byte will be asserted when an unsupported
// PMBus® command or data or other communication fault occurs.
typedef struct {
    union {
        uint8_t all;
        
        struct {
            uint8_t              : 1; // UNSUPPORTED - Other Memory Or Logic Fault
            uint8_t other        : 1; // Other Communication Faults
            uint8_t reserved     : 1; // Reserved bit
            uint8_t              : 1; // UNSUPPORTED - Processor Fault Detected
            uint8_t              : 1; // UNSUPPORTED - Memory Fault Detected
            uint8_t              : 1; // UNSUPPORTED - Packet Error Check Failed
            uint8_t data_stat_rx : 1; // Invalid Or Unsupported Data Received
            uint8_t cmd_stat_rx  : 1; // Invalid Or Unsupported Command Received
        };
    };
} sStatusCML_t;

typedef struct {
    union {
        uint8_t all;
        
        struct {
            uint8_t reverse_operation : 1; // BCM Reverse Operation.
            uint8_t shutdown_fault    : 1; // Hardware Protections Shutdown Fault
            uint8_t bcm_uart_cml      : 1; // BCM UART CML
            uint8_t reserved          : 5; // Reserved bits            
        };
    };
} sStatusSpecific_t;

// Data structure for SET_ALL_THRESHOLD (D5h)
typedef struct {
    union {
        uint8_t all[6];
        
        struct {
            uint8_t ot_fault_limit;     // Overtemperature fault threshold.
            uint8_t ot_warn_limit;      // Overtemperature warning threshold.
            uint8_t vin_ov_fault_limit; // Input- over-voltage fault threshold.
            uint8_t vin_ov_warn_limit;  // Input- over-voltage warning threshold.
            uint8_t iout_oc_fault_limit;// Output- over-current fault threshold.
            uint8_t iout_oc_warn_limit; // Output- over-current fault threshold.
        };
    };
} sThresholds_t;

// Data structure for measurement data.
typedef struct {
    float   vin;  // HI-side voltage. Range: 130V to 780V
    int16_t iin;  // HI-side current Range: -0.85A to 4.4A
    float   vout; // LO-side voltage Range: 8.125V to 48.75V
    int16_t iout; // LO-side current: -13.6A to 70A
    int16_t pout; // LO-side power
    int16_t rout; // Low-voltage side output resistance. Range: 5mOhm to 40mOhm.
    int16_t temperature; // Internal temperature in °C. Range: -55°C to 130°C.
    float   kfactor;     // K factor
    float   tdelay;      // Start up delay in addition to fixed delay
} sData_t;

typedef struct _BCM4414 {
    sData_t              measurements;
    sStatusSpecific_t    status_specific;
    sStatusCML_t         status_cml;
    sStatusTemperature_t status_temperature;
    sStatusInput_t       status_input;
    sStatusIOUT_t        status_iout;
    sStatusData_t        status_data;
    sThresholds_t        thresholds;
} BCM4414;

extern BCM4414 bcm;

// Command codes
#define VICOR_CMD_PAGE                0x00 // Access BCM stored information
#define VICOR_CMD_OPERATION           0x01 // Turn BCM on or off
#define VICOR_CMD_CLEAR_FAULTS        0x03 // Clear all faults
#define VICOR_CMD_CAPABILITY          0x19 // PMBs key capabilites set by factory
#define VICOR_CMD_OT_FAULT_LIMIT      0x4F // Overtemperature protection
#define VICOR_CMD_OT_WARN_LIMIT       0x51 // Overtemperature warning
#define VICOR_CMD_VIN_OV_FAULT_LIMIT  0x55 // High-voltage-side overvoltage protection
#define VICOR_CMD_VIN_OV_WARN_LIMIT   0x57 // High-voltage-side overvoltage warning
#define VICOR_CMD_IIN_OC_FAULT_LIMIT  0x5B // High-voltage-side overcurrent protection
#define VICOR_CMD_IIN_OC_WARN_LIMIT   0x5D // High-voltage-side overcurrent warning
#define VICOR_CMD_TON_DELAY           0x60 // Start up delay in addition to fixed delay
#define VICOR_CMD_STATUS_BYTE         0x78 // Summary of faults
#define VICOR_CMD_STATUS_WORD         0x79 // Summary of fault conditions
#define VICOR_CMD_STATUS_IOUT         0x7B // Overcurrent fault status
#define VICOR_CMD_STATUS_INPUT        0x7C // Overvoltage and undervoltage fault status
#define VICOR_CMD_STATUS_TEMPERATURE  0x7D // Overtemperature and undertemperature fault status
#define VICOR_CMD_STATUS_CML          0x7E // PMBus communication fault
#define VICOR_CMD_STATUS_MFR_SPECIFIC 0x80 // Other BCM status indicator
#define VICOR_CMD_READ_VIN            0x88 // Reads HI-side voltage
#define VICOR_CMD_READ_IIN            0x89 // Reads HI-side current
#define VICOR_CMD_READ_VOUT           0x8B // Reads LO-side voltage
#define VICOR_CMD_READ_IOUT           0x8C // Reads LO-side current
#define VICOR_CMD_READ_TEMPERATURE_1  0x8D // Reads internal temperature
#define VICOR_CMD_READ_POUT           0x96 // Reads LO-side power
#define VICOR_CMD_PMBUS_REVISION      0x98 // PMBus compatible revision
#define VICOR_CMD_MFR_ID              0x99 // BCM controller ID
#define VICOR_CMD_MFR_MODEL           0x9A // Internal controller or BCM model
#define VICOR_CMD_MFR_REVISION        0x9B // Internal controller or BCM revision
#define VICOR_CMD_MFR_LOCATION        0x9C // Internal controller or BCM factory location
#define VICOR_CMD_MFR_DATE            0x9D // Internal controller or BCM manufacturing date
#define VICOR_CMD_MFR_SERIAL          0x9E // Internal controller or BCM serial number
#define VICOR_CMD_MFR_VIN_MIN         0xA0 // Minimum rated high side voltage
#define VICOR_CMD_MFR_VIN_MAX         0xA1 // Maximum rated high side voltage
#define VICOR_CMD_MFR_VOUT_MIN        0xA4 // Minimum rated low side voltage
#define VICOR_CMD_MFR_VOUT_MAX        0xA5 // Maximum rated low side voltage
#define VICOR_CMD_MFR_IOUT_MAX        0xA6 // Maximum rated low side current
#define VICOR_CMD_MFR_POUT_MAX        0xA7 // Maximum rated low side power
#define VICOR_CMD_READ_K_FACTOR       0xD1 // Reads K factor
#define VICOR_CMD_READ_BCM_ROUT       0xD4 // Reads low-voltage side output resistance
#define SET_ALL_THRESHOLDS            0xD5 // Set supervisory warning and protection thresholds
#define DISABLE_FAULT                 0xD7 // Disable overvoltage, overcurrent or undervoltage supervisory faults

#define DEVICE_ADDRESS 0x51
#define I2C_TIMEOUT    100

// Function declarations
void read_status_cml(I2C_HandleTypeDef *hi2c);
void read_status_specific(I2C_HandleTypeDef *hi2c);
void read_status_iout(I2C_HandleTypeDef *hi2c);
void read_status_input(I2C_HandleTypeDef *hi2c);

void read_temperature(I2C_HandleTypeDef *hi2c);
void read_iout(I2C_HandleTypeDef *hi2c);
void read_iin(I2C_HandleTypeDef *hi2c);
void read_pout(I2C_HandleTypeDef *hi2c);
void read_vin(I2C_HandleTypeDef *hi2c);
void read_ton_delay(I2C_HandleTypeDef *hi2c);
void read_vout(I2C_HandleTypeDef *hi2c);
void read_rout(I2C_HandleTypeDef *hi2c);

void clear_faults(I2C_HandleTypeDef *hi2c);

#endif