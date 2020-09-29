/*!
 * @file vicor.h
 * @brief Define the infrastructure of the vicor BCM class
 * @n This is a library for the BCM 600V DC/DC regulator intended for Team Spark.
 * @author Guðni Már Gilbert
 * @version  V1.0
 * @date  2020-11-01
 */

#ifndef VICOR_H
#define VICOR_H

#include <Arduino.h>
#include <Wire.h>

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
#define VICOR_CMD_MFR_VOUT_MIN         0xA4 // Minimum rated low side voltage
#define VICOR_CMD_MFR_VOUT_MAX        0xA5 // Maximum rated low side voltage
#define VICOR_CMD_MFR_IOUT_MAX        0xA6 // Maximum rated low side current
#define VICOR_CMD_MFR_POUT_MAX        0xA7 // Maximum rated low side power
#define VICOR_CMD_READ_K_FACTOR       0xD1 // Reads K factor
#define VICOR_CMD_READ_BCM_ROUT       0xD4 // Reads low-voltage side output resistance
#define SET_ALL_THRESHOLDS            0xD5 // Set supervisory warning and protection thresholds
#define DISABLE_FAULT                 0xD7 // Disable overvoltage, overcurrent or undervoltage supervisory faults

// Default register values
#define DEFAULT_ADDRESS             0x51
#define DEFAULT_PAGE                0x00
#define DEFAULT_OPERATION           0x80
#define DEFAULT_CAPABILITY          0x20
#define DEFAULT_OT_FAULT_LIMIT      0x64
#define DEFAULT_OT_WARN_LIMIT       0x64
#define DEFAULT_VIN_OV_FAULT_LIMIT  0x64
#define DEFAULT_VIN_OV_WARN_LIMIT   0x64
#define DEFAULT_IIN_OC_FAULT_LIMIT  0x64
#define DEFAULT_IIN_OC_WARN_LIMIT   0x64
#define DEFAULT_TON_DELAY           0x00
#define DEFAULT_STATUS_BYTE         0x00
#define DEFAULT_STATUS_WORD         0x00
#define DEFAULT_STATUS_IOUT         0x00
#define DEFAULT_STATUS_INPUT        0x00
#define DEFAULT_STATUS_TEMPERATURE  0x00
#define DEFAULT_STATUS_CML          0x00
#define DEFAULT_STATUS_MFR_SPECIFIC 0x00
#define DEFAULT_READ_VIN            0xFFFF
#define DEFAULT_READ_IIN            0xFFFF
#define DEFAULT_READ_VOUT           0xFFFF
#define DEFAULT_READ_IOUT           0xFFFF
#define DEFAULT_READ_TEMPERATURE_1  0xFFFF
#define DEFAULT_READ_POUT           0xFFFF
#define DEFAULT_SET_ALL_THRESHOLDS  0x646464646464
#define DEFAULT_DISABLE_FAULT       0x00

class Vicor
{
public:
    #define ERR_OK             0      //No error
    #define ERR_DATA_BUS      -1      //Data bus error
    #define ERR_IC_VERSION    -2      //Chip version does not match

    typedef struct {
        uint8_t                     : 1; // UNSUPPORTED - VOUT FAULT OR WARNING
        uint8_t iout_pout_fault     : 1; // IOUT/POUT FAULT OR WARNING
        uint8_t input_fault         : 1; // INPUT FAULT OR WARNING
        uint8_t status_mfr_specific : 1; // STATUS_MFR_SPECIFIC
        uint8_t power_good          : 1; // POWER_GOOD Negated*
        uint8_t                     : 1; // UNSUPPORTED - FAN FAULT OR WARNING
        uint8_t                     : 1; // UNSUPPORTED - OTHER
        uint8_t                     : 1; // UNSUPPORTED - UNKNOWN FAULT OR WARNING
        uint8_t unit_is_busy        : 1;
        uint8_t unit_is_off         : 1;
        uint8_t                     : 1; // UNSUPPORTED - VOUT_OV_FAULT
        uint8_t iout_oc_fault       : 1;
        uint8_t vin_uv_fault        : 1;
        uint8_t temperature_fault   : 1; // TEMPERATURE FAULT OR WARNING
        uint8_t pmbus_communication_event : 1;
        uint8_t unknown_fault       : 1; // NONE OF THE ABOVE
    } __attribute__ ((packed)) sStatusData_t;

    struct {
        uint8_t iout_oc_fault   : 1;
        uint8_t                 : 1; // UNSUPPORTED - IOUT_OC_LV_FAULT
        uint8_t iout_oc_warning : 1;
        uint8_t                 : 1; // UNSUPPORTED - IOUT_UC_FAULT
        uint8_t                 : 1; // UNSUPPORTED - Current Share Fault
        uint8_t                 : 1; // UNSUPPORTED - In Power Limiting Mode
        uint8_t                 : 1; // UNSUPPORTED - POUT_OP_FAULT
        uint8_t                 : 1; // UNSUPPORTED - POUT_OP_WARNING
    } sStatusIOUT_t;

    // A one indicates a fault.
    struct {
        uint8_t vin_ov_fault   : 1;
        uint8_t vin_ov_warning : 1;
        uint8_t                : 1; // UNSUPPORTED - VIN_UV_WARNING
        uint8_t vin_uv_fault   : 1;
        uint8_t                : 1; // UNSUPPORTED - Unit Off For Insufficient Input Voltage
        uint8_t                : 1; // UNSUPPORTED - IIN_OC_FAULT
        uint8_t                : 1; // UNSUPPORTED - IIN_OC_WARNING
        uint8_t                : 1; // UNSUPPORTED - PIN_OP_WARNING
    } sStatusInput_t;

    // A one indicates a fault.
    struct {
        uint8_t ot_fault   : 1;
        uint8_t ot_warning : 1;
        uint8_t            : 1; // UNSUPPORTED - UT_WARNING
        uint8_t ut_fault   : 1;
        uint8_t reserved   : 4; // Reserved bits
    } sStatusTemperature_t;

    // The STATUS_CML data byte will be asserted when an unsupported
    // PMBus® command or data or other communication fault occurs.
    struct {
        uint8_t cmd_stat_rx  : 1; // Invalid Or Unsupported Command Received
        uint8_t data_stat_rx : 1;
        uint8_t              : 1; // UNSUPPORTED - Packet Error Check Failed
        uint8_t              : 1; // UNSUPPORTED - Memory Fault Detected
        uint8_t              : 1; // UNSUPPORTED - Processor Fault Detected
        uint8_t reserved     : 1; // Reserved bit
        uint8_t other        : 1; // Other Communication Faults
        uint8_t              : 1; // UNSUPPORTED - Other Memory Or Logic Fault
    } sStatusCML_t;

    struct {
        uint8_t reserved          : 5; // Reserved bits
        uint8_t bcm_uart_cml      : 1; // BCM UART CML
        uint8_t shutdown_fault    : 1; // Hardware Protections Shutdown Fault
        uint8_t reverse_operation : 1; // BCM Reverse Operation.
    } sStatusSpecific_t;

    // Data structure for measurement data.
    struct {
        float vin;  // HI-side voltage. Range: 130V to 780V
        float iin;  // HI-side current Range: -0.85A to 4.4A
        float vout; // LO-side voltage Range: 8.125V to 48.75V
        float iout; // LO-side current: -13.6A to 70A
        int16_t pout; // LO-side power
        float rout; // Low-voltage side output resistance. Range: 5mOhm to 40mOhm.
        int16_t temperature; // Internal temperature in °C. Range: -55°C to 130°C.
        float kfactor;     // K factor
        float tdelay;      // Start up delay in addition to fixed delay
    } sData_t;

    enum PAGE_DATA_BYTE {
        page_zero = 0x00,
        page_one = 0x01
    };

    /*!
   * @brief Construct the function
   * @param pWire IC bus pointer object and construction device, can both pass or not pass parameters, Wire in default.
   * @param address device I2C address.
   */
    Vicor(TwoWire *pWire = &Wire, uint8_t address = DEFAULT_ADDRESS);
    Vicor(uint8_t address);

    /**
    * @brief Reads HI-side voltage. 
    * @return the actual, corrected measurement.
    */
    float get_READ_VIN();

    /**
    * @brief Reads HI-side current. 
    * @return the actual, corrected measurement.
    */
    float get_READ_IIN();

    /**
    * @brief Reads LO-side voltage. 
    * @return the actual, corrected measurement.
    */
    float get_READ_VOUT();

    /**
    * @brief Reads LO-side current. 
    * @return the actual, corrected measurement.
    */
    float get_READ_IOUT();

    /**
    * @brief Reads LO-side output resistance. 
    * @return the actual, corrected measurement.
    */
    float get_READ_BCM_ROUT();

    /**
    * @brief Reads the BCM4414 module's internal temperature. 
    * @return the actual, corrected measurement.
    */
    int16_t get_READ_TEMPERATURE_1();

    /**
    * @brief Reads the BCM4414 module's K factor. 
    * @return the actual, corrected measurement.
    */
    float get_READ_K_FACTOR();

    /**
    * @brief Reads the BCM4414 module's time on delay. 
    * @return the actual, corrected measurement.
    */
    float get_TON_DELAY();

    /**
    * @brief Reads the BCM4414 module's output power on LO-side. 
    * @return the actual, corrected measurement.
    */
    int16_t get_READ_POUT();

    /**
    * @brief Reads the BCM4414 module's lowest HI-side voltage set by Vicor. 
    * @return the actual, corrected measurement.
    */
    int16_t get_MFR_VIN_MIN();
    int16_t get_MFR_VIN_MAX();
    int16_t get_MFR_VOUT_MIN();
    int16_t get_MFR_VOUT_MAX();
    int16_t get_MFR_IOUT_MAX();
    int16_t get_MFR_POUT_MAX();

    /**
    * @brief Reads the PMbus revision.
    * @return Should always return 0x22. (Can be useful to test communication).
    */
    void get_PMBUS_REVISION();
    
    /**
    * @brief writes a PAGE command and a date byte. Only use this to change the Page mode.
    * @param data_byte either 0x00 or 0x01
    */
    void write_PAGE(uint8_t data_byte);

    /**
    * @brief Reads the status word register.
    */
    sStatusData_t read_status_word();

    /**
     * @brief This command clears all status bits that have been previously set.
     * Persistent or active faults are re-asserted again once cleared. All
     * faults are latched once asserted in the BCM controller. 
     * 
     * Registered faults will not be cleared when shutting down the BCM
     * powertrain by recycling the BCM high side voltage or sending
     * the OPERATION command
     */ 
    void clear_faults();

    void operation();

    void read_CML();

    void read_status_mfr();

    /* STATUS IOUT
    Bit  | 0 | 1 | 2 | 3 | 4 |        5        | 6 |       7       |
    Name | x | x | x | x | x | IOUT_OC_WARNING | x | IOUT_OC_FAULT |
    */
    void read_status_iout();

    void read_status_input();

    void read_status_temperature();

    void print_status_word();

    void print_mfr_id();
    void print_mfr_revision();
    void print_mfr_model();
    void print_mfr_location();
    void print_mfr_date();
    void print_mfr_serial();

    void read_capability();

    void write_time_on_delay();

     /**
    * @brief Initialize the function
    * @return Return 0 indicates a successful initialization, while other values indicates failure and return to error code.
    */
    int begin();

private:
    /* data */
    TwoWire *_pWire;  // Wire library.
    uint8_t _address; // BCM module's I2C address.
    
    /**
    * @brief Writes bytes with I2C. The function:
    * 1. Creates a start condition.
    * 2. Sends the Device Address with write bit.
    * 3. Sends the contents of pBuf (a command byte for example).
    * 4. I2C bus is either stopped or a repeated start condition is created.
    * 
    * @param pBuf byte array of type uint8_t. Do not include the Device Address.
    *             This array should only contain command bytes and data bytes.
    * @param size number of bytes to write.
    * @param sendStop if true: the function sends a Stop command. If false: a repeated start condtion is created.
    */
    void write(uint8_t* pBuf, size_t size, bool sendStop);

    /**
    * @brief Writes bytes with I2C. The function:
    * 1. Creates a start condition.
    * 2. Sends the Device Address with write bit.
    * 3. Sends the contents of pBuf (a command byte for example).
    * 4. I2C bus is either stopped or a repeated start condition is created.
    * 
    * @param pBuf a single byte of type uint8_t. Do not include the Device Address.
    *             This array should only contain command bytes and data bytes.
    * @param size number of bytes to write.
    * @param sendStop if true: the function sends a Stop command. If false: a repeated start condtion is created.
    */
    void write(uint8_t pBuf, size_t size, bool sendStop);

    /**
    * @brief Write command to sensor chip.
    * @param pBuf  The data contained in the command.
    * @param size  Number of command data
    * @return Return 0 indicates the successful read, other return values suggest unsuccessful read.
    */
    uint8_t readData(uint8_t *pBuf, uint8_t size);

    /**
     * @brief Reads READ_VIN register from BCM module and converts
     * the reported value to actual value in Volts.
     */ 
    void convert_raw_READ_VIN();

    /**
     * @brief Reads READ_IIN register from BCM module and converts
     * the reported value to actual value in Amps.
     */
    void convert_raw_READ_IIN();

    /**
     * @brief Reads READ_VOUT register from BCM module and converts
     * the reported value to actual value in Volts.
     */
    void convert_raw_READ_VOUT();

    /**
     * @brief Reads READ_IOUT register from BCM module and converts
     * the reported value to actual value in Amps.
     */
    void convert_raw_READ_IOUT();

    /**
     * @brief Reads READ_BCM_ROUT register from BCM module and converts
     * the reported value to actual value in Ohms.
     */
    void convert_raw_READ_BCM_ROUT();

    /**
     * @brief Reads READ_TEMPERATURE_1 register from BCM module and converts
     * the reported value to actual value in °C.
     */
    void convert_raw_READ_TEMPERATURE_1();

    /**
     * @brief Reads READ_K_FACTOR register from BCM module and converts
     * the reported value to actual value in V/V.
     */
    void convert_raw_READ_K_FACTOR();

    /**
     * @brief Reads TON_DELAY register from BCM module and converts
     * the reported value to actual value in seconds.
     */
    void convert_raw_TON_DELAY();
    
    
    // TODO: function to read status byte
    // TODO: function to read status word


};

#endif