/*
 *
 */
#ifndef __MJD_DS3231_H__
#define __MJD_DS3231_H__

#ifdef __cplusplus
extern "C" {
#endif

// I2C features

// DS3231 Settings
#define MJD_DS3231_I2C_ADDR                   (0x68)       /*!< I2C slave address for the DS3231 (DS3231M = 0xD0) */
#define MJD_DS3231_I2C_MASTER_FREQ_HZ         (100 * 1000) /*!< I2C master clock frequency 100K (Normal) XOR 400K (High) */
#define MJD_DS3231_I2C_MASTER_RX_BUF_DISABLE  (0)          /*!< For slaves. I2C master does not need RX buffer */
#define MJD_DS3231_I2C_MASTER_TX_BUF_DISABLE  (0)          /*!< For slaves. I2C master does not need TX buffer */
#define MJD_DS3231_I2C_MASTER_INTR_FLAG_NONE  (0)

// DS3231 Registers
#define MJD_DS3231_REGISTER_SECONDS (0x00)
#define MJD_DS3231_REGISTER_MINUTES (0x01)
#define MJD_DS3231_REGISTER_HOURS   (0x02)
#define MJD_DS3231_REGISTER_WEEK_DAY (0x05)
#define MJD_DS3231_REGISTER_DAY     (0x04)
#define MJD_DS3231_REGISTER_MONTH   (0x05)
#define MJD_DS3231_REGISTER_YEAR    (0x06)

#define MJD_DS3231_REGISTER_CONTROL         (0x0E) /*!< [notusedyet] EOSC BBSQW CONV RS2 RS1 INTCN A2IE A1IE */
#define MJD_DS3231_REGISTER_CONTROL_STATUS  (0x0F) /*!< [notusedyet] OSF 0 0 0 EN32kHz BSY A2F A1F */
#define MJD_DS3231_REGISTER_AGING_OFFSET    (0x10) /*!< [notusedyet] SIGN DATA DATA DATA DATA DATA DATA DATA */
#define MJD_DS3231_REGISTER_TEMPERATURE_MSB (0x11)
#define MJD_DS3231_REGISTER_TEMPERATURE_LSB (0x12)

// DS3231 Values
#define MJD_DS3231_MAXVAL_YEAR    (2099)
#define MJD_DS3231_MAXVAL_MONTH   (12)
#define MJD_DS3231_MAXVAL_DAY     (31)
#define MJD_DS3231_MAXVAL_HOURS   (23)
#define MJD_DS3231_MAXVAL_MINUTES (59)
#define MJD_DS3231_MAXVAL_SECONDS (59)
#define MJD_DS3231_MAXVAL_WEEK_DAY (7)

/**
 * Data structs
 */
typedef struct {
        bool manage_i2c_driver;
        i2c_port_t i2c_port_num;
        uint8_t i2c_slave_addr;
        gpio_num_t scl_io_num;
        gpio_num_t sda_io_num;
} mjd_ds3231_config_t;

#define MJD_DS3231_CONFIG_DEFAULT() { \
    .manage_i2c_driver = true,  \
    .i2c_port_num = I2C_NUM_0,  \
    .i2c_slave_addr = MJD_DS3231_I2C_ADDR  \
};

/*
 * @brief mjd_ds3231_data_t
 *
 * @doc metric: temperature:
 *        Temperature is represented as a 10-bit code with a resolution of 0.25°C and is accessible at REG 11h and 12h.
 *          REG 11h all 8 bits: the integer portion.                                 Byte [SIGN DATA DATA DATA DATA DATA DATA DATA]
 *          REG 12h upper 2 bits b7 b6: the fractional portion, each tick is 0.25°C. Byte [DATA DATA . . . . . .]
 *        The temperature is encoded in two’s complement format.
 *        Example 00011001 01b = +25.25°C.
 *
 */
typedef struct {
        uint32_t year;    // u8 0000-9999
        uint8_t month;   // BCD 01–12 + Century
        uint8_t day;     // 01–31
        uint8_t hours;   // BCD 00–23PM
        uint8_t minutes; // BCD 00–59
        uint8_t seconds; // BCD 00–59
        uint8_t week_day; // 1–7 Values that correspond to the day of week are user-defined but must be sequential (i.e., if 1 equals Sunday, then 2 equals Monday, and so on)
        int8_t temperature_msb;  //  @important Signed integer!
        uint8_t temperature_lsb;
        float temperature_celsius;
} mjd_ds3231_data_t;

/**
 * Function declarations
 */
esp_err_t mjd_ds3231_init(const mjd_ds3231_config_t* config);
esp_err_t mjd_ds3231_deinit(const mjd_ds3231_config_t* config);
esp_err_t mjd_ds3231_get_data(const mjd_ds3231_config_t* config, mjd_ds3231_data_t* data);
esp_err_t mjd_ds3231_set_datetime(const mjd_ds3231_config_t* config, const mjd_ds3231_data_t* data);

#ifdef __cplusplus
}
#endif

#endif /* __MJD_DS3231_H__ */
