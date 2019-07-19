/*
 * ZS-042 DS1302 Real Time Clock Module | I2C protocol
 *
 */

// Component header file(s)
#include "mjd.h"
#include "mjd_ds3231.h"

/*
 * Logging
 */
static const char TAG[] = "mjd_ds3231";

/*
 * MAIN
 */

/*********************************************************************************
 * PUBLIC.
 * DS3231: I2C initialization
 * @important Custom for the DS3231 sensor!
 *********************************************************************************/
esp_err_t mjd_ds3231_init(const mjd_ds3231_config_t* config) {
    ESP_LOGD(TAG, "%s()", __FUNCTION__);

    esp_err_t f_retval = ESP_OK;
    i2c_cmd_handle_t cmd;

    if (config->manage_i2c_driver == true) {
        // Config
        i2c_config_t i2c_conf =
                    { 0 };
        i2c_conf.mode = I2C_MODE_MASTER;
        i2c_conf.scl_io_num = config->scl_io_num;
        i2c_conf.sda_io_num = config->sda_io_num;
        i2c_conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
        i2c_conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
        i2c_conf.master.clk_speed = MJD_DS3231_I2C_MASTER_FREQ_HZ;
        f_retval = i2c_param_config(config->i2c_port_num, &i2c_conf);
        if (f_retval != ESP_OK) {
            ESP_LOGE(TAG, "%s(). ABORT. i2c_param_config() | err %i (%s)", __FUNCTION__, f_retval, esp_err_to_name(f_retval));
            // GOTO
            goto cleanup;
        }
        f_retval = i2c_driver_install(config->i2c_port_num, I2C_MODE_MASTER, MJD_DS3231_I2C_MASTER_RX_BUF_DISABLE,
                MJD_DS3231_I2C_MASTER_TX_BUF_DISABLE,
                MJD_DS3231_I2C_MASTER_INTR_FLAG_NONE);
        if (f_retval != ESP_OK) {
            ESP_LOGE(TAG, "%s(). ABORT. i2c_driver_install() | err %i (%s)", __FUNCTION__, f_retval, esp_err_to_name(f_retval));
            // GOTO
            goto cleanup;
        }
    }

    // Verify that the I2C slave is working properly
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (config->i2c_slave_addr << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK);
    i2c_master_stop(cmd);
    f_retval = i2c_master_cmd_begin(config->i2c_port_num, cmd, RTOS_DELAY_1SEC);
    if (f_retval != ESP_OK) {
        ESP_LOGE(TAG, "%s(). ABORT. i2c_master_cmd_begin() I2C slave is NOT working properly | err %i (%s)", __FUNCTION__, f_retval, esp_err_to_name(f_retval));
        // GOTO
        goto cleanup;
    }
    i2c_cmd_link_delete(cmd);

    // LABEL
    cleanup: ;

    return f_retval;
}

/*********************************************************************************
 * PUBLIC.
 * DS3231: I2C DE-initialization
 * @important Custom for the DS3231 sensor!
 *********************************************************************************/
esp_err_t mjd_ds3231_deinit(const mjd_ds3231_config_t* config) {
    ESP_LOGD(TAG, "%s()", __FUNCTION__);

    esp_err_t f_retval = ESP_OK;

    if (config->manage_i2c_driver == true) {
        f_retval = i2c_driver_delete(config->i2c_port_num);
        if (f_retval != ESP_OK) {
            ESP_LOGE(TAG, "%s(). ABORT. i2c_driver_delete() | err %i (%s)", __FUNCTION__, f_retval, esp_err_to_name(f_retval));
            // GOTO
            goto cleanup;
        }
    }

    // LABEL
    cleanup: ;

    return f_retval;
}

/*********************************************************************************
 * PUBLIC.
 * Read the DS3231 data
 *********************************************************************************/
esp_err_t mjd_ds3231_get_data(const mjd_ds3231_config_t* config, mjd_ds3231_data_t* data) {
    ESP_LOGD(TAG, "%s()", __FUNCTION__);

    esp_err_t f_retval = ESP_OK;
    i2c_cmd_handle_t cmd;
    uint8_t year_raw;

    /*
     * Read batch 1
     */
    // Send request. Goto MJD_DS3231_REGISTER_SECONDS
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (config->i2c_slave_addr << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK);
    i2c_master_write_byte(cmd, MJD_DS3231_REGISTER_SECONDS, I2C_MASTER_ACK);
    i2c_master_stop(cmd);
    f_retval = i2c_master_cmd_begin(config->i2c_port_num, cmd, RTOS_DELAY_1SEC);
    if (f_retval != ESP_OK) {
        ESP_LOGE(TAG, "%s(). ABORT. Send request | err %i (%s)", __FUNCTION__, f_retval, esp_err_to_name(f_retval));
        i2c_cmd_link_delete(cmd); // CLEANUP
        // GOTO
        goto cleanup;
    }
    i2c_cmd_link_delete(cmd);

    // Wait for the sensor/module. @important The DS3231 datasheet does not specify how long to wait for a response! Use an arbitrary value.
    mjd_delay_millisec(10);

    // Receive response (bulk read).
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (config->i2c_slave_addr << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, &data->seconds, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, &data->minutes, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, &data->hours, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, &data->week_day, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, &data->day, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, &data->month, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, &year_raw, I2C_MASTER_NACK); // last read must be NO_ACK
    i2c_master_stop(cmd);
    f_retval = i2c_master_cmd_begin(config->i2c_port_num, cmd, RTOS_DELAY_1SEC);
    if (f_retval != ESP_OK) { // This function will trigger sending all queued commands.
        ESP_LOGE(TAG, "%s(). ABORT. Receive answer | err %i (%s)", __FUNCTION__, f_retval, esp_err_to_name(f_retval));
        i2c_cmd_link_delete(cmd); // CLEANUP
        // GOTO
        goto cleanup;
    }
    i2c_cmd_link_delete(cmd);

    /*
     * Read batch 2
     */
    // Send request. Goto MJD_DS3231_REGISTER_TEMPERATURE_MSB
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (config->i2c_slave_addr << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK);
    i2c_master_write_byte(cmd, MJD_DS3231_REGISTER_TEMPERATURE_MSB, I2C_MASTER_ACK);
    i2c_master_stop(cmd);
    f_retval = i2c_master_cmd_begin(config->i2c_port_num, cmd, RTOS_DELAY_1SEC);
    if (f_retval != ESP_OK) {
        ESP_LOGE(TAG, "%s(). ABORT. Send request | err %i (%s)", __FUNCTION__, f_retval, esp_err_to_name(f_retval));
        i2c_cmd_link_delete(cmd); // CLEANUP
        // GOTO
        goto cleanup;
    }
    i2c_cmd_link_delete(cmd);

    // Wait for the sensor/module. @important The DS3231 datasheet does not specify how long to wait for a response! Use an arbitrary value.
    mjd_delay_millisec(10);

    // Receive response (bulk read).
    uint8_t msb;
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (config->i2c_slave_addr << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, &msb, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, &data->temperature_lsb, I2C_MASTER_NACK); // last read must be NO_ACK
    i2c_master_stop(cmd);
    f_retval = i2c_master_cmd_begin(config->i2c_port_num, cmd, RTOS_DELAY_1SEC);
    if (f_retval != ESP_OK) { // This function will trigger sending all queued commands.
        ESP_LOGE(TAG, "%s(). ABORT. Receive answer | err %i (%s)", __FUNCTION__, f_retval, esp_err_to_name(f_retval));
        i2c_cmd_link_delete(cmd); // CLEANUP
        // GOTO
        goto cleanup;
    }
    i2c_cmd_link_delete(cmd);

    /*
     * Process response
     */
    // seconds BCD
    data->seconds &= 0b01111111; // Keep 7 LSB
    data->seconds = mjd_bcd_to_byte(data->seconds);

    // minutes BCD
    data->minutes &= 0b01111111; // Keep 7 LSB
    data->minutes = mjd_bcd_to_byte(data->minutes);

    // hours BCD
    data->hours &= 0b00111111; // Keep 6 LSB
    data->hours = mjd_bcd_to_byte(data->hours);

    // week_day BCD
    data->week_day = mjd_bcd_to_byte(data->week_day);

    // day BCD
    data->day &= 0b00111111; // Keep 6 LSB
    data->day = mjd_bcd_to_byte(data->day);

    // month BCD
    data->month &= 0b00011111; // Keep 5 LSB
    data->month = mjd_bcd_to_byte(data->month);

    // year BCD @important year storage = 20YY
    data->year = 2000 + mjd_bcd_to_byte(year_raw);

    // temperature
    data->temperature_msb = (int8_t) msb; // convert from (unsigned)uint8_t to (signed)int8_t
    data->temperature_celsius = data->temperature_msb + (0.25 * (data->temperature_lsb >> 6));
    ESP_LOGD(TAG, "%s(). data->temperature_msb:      int8_t  %i", __FUNCTION__, data->temperature_msb);
    ESP_LOGD(TAG, "%s(). data->temperature_lsb:      uint8_t %u", __FUNCTION__, data->temperature_lsb);
    ESP_LOGD(TAG, "%s(). data->temperature_lsb >> 6: uint8_t %u", __FUNCTION__, data->temperature_lsb >> 6);

    // LABEL
    cleanup: ;

    return f_retval;
}

/*********************************************************************************
 * PUBLIC.
 * Set the DS3231 data
 *********************************************************************************/
esp_err_t mjd_ds3231_set_datetime(const mjd_ds3231_config_t* config, const mjd_ds3231_data_t* data) {
    ESP_LOGD(TAG, "%s()", __FUNCTION__);

    esp_err_t f_retval = ESP_OK;
    i2c_cmd_handle_t cmd;

    // Validation
    bool validation_passed = true;
    if (data->year > MJD_DS3231_MAXVAL_YEAR) {
        validation_passed = false;
        ESP_LOGE(TAG, "%s(). ABORT. param data->year (%u) > %u", __FUNCTION__, data->year, MJD_DS3231_MAXVAL_YEAR);
    }
    if (data->month > MJD_DS3231_MAXVAL_MONTH) {
        validation_passed = false;
        ESP_LOGE(TAG, "%s(). ABORT. param data->month (%u) > %u", __FUNCTION__, data->month, MJD_DS3231_MAXVAL_MONTH);
    }
    if (data->day > MJD_DS3231_MAXVAL_DAY) {
        validation_passed = false;
        ESP_LOGE(TAG, "%s(). ABORT. param data->day (%u) > %u", __FUNCTION__, data->day, MJD_DS3231_MAXVAL_DAY);
    }
    if (data->hours > MJD_DS3231_MAXVAL_HOURS) {
        validation_passed = false;
        ESP_LOGE(TAG, "%s(). ABORT. param data->hours (%u) > %u", __FUNCTION__, data->hours, MJD_DS3231_MAXVAL_HOURS);
    }
    if (data->minutes > MJD_DS3231_MAXVAL_MINUTES) {
        validation_passed = false;
        ESP_LOGE(TAG, "%s(). ABORT. param data->minutes (%u) > %u", __FUNCTION__, data->minutes, MJD_DS3231_MAXVAL_MINUTES);
    }
    if (data->seconds > MJD_DS3231_MAXVAL_SECONDS) {
        validation_passed = false;
        ESP_LOGE(TAG, "%s(). ABORT. param data->seconds (%u) > %u", __FUNCTION__, data->seconds, MJD_DS3231_MAXVAL_SECONDS);
    }
    if (data->week_day > MJD_DS3231_MAXVAL_WEEK_DAY) {
        validation_passed = false;
        ESP_LOGE(TAG, "%s(). ABORT. param data->week_day (%u) > %u", __FUNCTION__, data->week_day, MJD_DS3231_MAXVAL_WEEK_DAY);
    }
    if (validation_passed == false) {
        f_retval = ESP_ERR_INVALID_ARG;
        ESP_LOGE(TAG, "%s(). ABORT. Invalid input parameter(s) | err %i (%s)", __FUNCTION__, f_retval, esp_err_to_name(f_retval));
        // GOTO
        goto cleanup;
    }

    // Extract & transform field values
    uint8_t seconds = mjd_byte_to_bcd(data->seconds);
    uint8_t minutes = mjd_byte_to_bcd(data->minutes);
    uint8_t hours = mjd_byte_to_bcd(data->hours);
    uint8_t week_day = mjd_byte_to_bcd(data->week_day);
    uint8_t day = mjd_byte_to_bcd(data->day);
    uint8_t month = mjd_byte_to_bcd(data->month);
    uint8_t year = mjd_byte_to_bcd(data->year - 2000); // 20XX

    // Send write request. Bulk write.
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (config->i2c_slave_addr << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK);
    // Start writing at this register (and increment to next register for each subsequent write)
    i2c_master_write_byte(cmd, MJD_DS3231_REGISTER_SECONDS, I2C_MASTER_ACK);

    i2c_master_write_byte(cmd, seconds, I2C_MASTER_ACK);
    i2c_master_write_byte(cmd, minutes, I2C_MASTER_ACK);
    i2c_master_write_byte(cmd, hours, I2C_MASTER_ACK);
    i2c_master_write_byte(cmd, week_day, I2C_MASTER_ACK);
    i2c_master_write_byte(cmd, day, I2C_MASTER_ACK);
    i2c_master_write_byte(cmd, month, I2C_MASTER_ACK);
    i2c_master_write_byte(cmd, year, I2C_MASTER_ACK);

    i2c_master_stop(cmd);
    f_retval = i2c_master_cmd_begin(config->i2c_port_num, cmd, RTOS_DELAY_1SEC);
    if (f_retval != ESP_OK) {
        ESP_LOGE(TAG, "%s(). ABORT. Send write request | err %i (%s)", __FUNCTION__, f_retval, esp_err_to_name(f_retval));
        i2c_cmd_link_delete(cmd); // CLEANUP
        // GOTO
        goto cleanup;
    }
    i2c_cmd_link_delete(cmd);

    // LABEL
    cleanup: ;

    return f_retval;
}
