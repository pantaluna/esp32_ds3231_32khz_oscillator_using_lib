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
esp_err_t mjd_ds3231_init(const mjd_ds3231_config_t* param_ptr_config) {
    ESP_LOGD(TAG, "%s()", __FUNCTION__);

    esp_err_t f_retval = ESP_OK;
    i2c_cmd_handle_t cmd;

    if (param_ptr_config->manage_i2c_driver == true) {
        // Config
        i2c_config_t i2c_conf =
                    { 0 };
        i2c_conf.mode = I2C_MODE_MASTER;
        i2c_conf.scl_io_num = param_ptr_config->scl_io_num;
        i2c_conf.sda_io_num = param_ptr_config->sda_io_num;
        i2c_conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
        i2c_conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
        i2c_conf.master.clk_speed = MJD_DS3231_I2C_MASTER_FREQ_HZ;
        f_retval = i2c_param_config(param_ptr_config->i2c_port_num, &i2c_conf);
        if (f_retval != ESP_OK) {
            ESP_LOGE(TAG, "%s(). ABORT. i2c_param_config() | err %i (%s)", __FUNCTION__, f_retval,
                    esp_err_to_name(f_retval));
            // GOTO
            goto cleanup;
        }
        f_retval = i2c_driver_install(param_ptr_config->i2c_port_num, I2C_MODE_MASTER, MJD_DS3231_I2C_MASTER_RX_BUF_DISABLE,
        MJD_DS3231_I2C_MASTER_TX_BUF_DISABLE,
        MJD_DS3231_I2C_MASTER_INTR_FLAG_NONE);
        if (f_retval != ESP_OK) {
            ESP_LOGE(TAG, "%s(). ABORT. i2c_driver_install() | err %i (%s)", __FUNCTION__, f_retval,
                    esp_err_to_name(f_retval));
            // GOTO
            goto cleanup;
        }
    }

    // Verify that the I2C slave is working properly
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (param_ptr_config->i2c_slave_addr << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK);
    i2c_master_stop(cmd);
    f_retval = i2c_master_cmd_begin(param_ptr_config->i2c_port_num, cmd, RTOS_DELAY_1SEC);
    if (f_retval != ESP_OK) {
        ESP_LOGE(TAG, "%s(). ABORT. i2c_master_cmd_begin() I2C slave is NOT working properly | err %i (%s)", __FUNCTION__,
                f_retval, esp_err_to_name(f_retval));
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
esp_err_t mjd_ds3231_deinit(const mjd_ds3231_config_t* param_ptr_config) {
    ESP_LOGD(TAG, "%s()", __FUNCTION__);

    esp_err_t f_retval = ESP_OK;

    if (param_ptr_config->manage_i2c_driver == true) {
        f_retval = i2c_driver_delete(param_ptr_config->i2c_port_num);
        if (f_retval != ESP_OK) {
            ESP_LOGE(TAG, "%s(). ABORT. i2c_driver_delete() | err %i (%s)", __FUNCTION__, f_retval,
                    esp_err_to_name(f_retval));
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
 * Log a DS3231 data structure.
 *
 *********************************************************************************/
esp_err_t mjd_ds3231_log_data(const mjd_ds3231_data_t param_data) {
    ESP_LOGD(TAG, "%s()", __FUNCTION__);

    esp_err_t f_retval = ESP_OK;

    ESP_LOGI(TAG, "Log mjd_ds3231_data_t param_data:");
    ESP_LOGI(TAG, "  %32s = %u", "(uint32_t) year", param_data.year);
    ESP_LOGI(TAG, "  %32s = %u", "(uint8_t)  month", param_data.month);
    ESP_LOGI(TAG, "  %32s = %u", "(uint8_t)  day", param_data.day);
    ESP_LOGI(TAG, "  %32s = %u", "(uint8_t)  hours", param_data.hours);
    ESP_LOGI(TAG, "  %32s = %u", "(uint8_t)  minutes", param_data.minutes);
    ESP_LOGI(TAG, "  %32s = %u", "(uint8_t)  seconds", param_data.seconds);
    ESP_LOGI(TAG, "  %32s = %u", "(uint8_t)  week_day", param_data.week_day);
    ESP_LOGI(TAG, "  %32s = %i", "(int8_t)   temperature_msb", param_data.temperature_msb);
    ESP_LOGI(TAG, "  %32s = %u", "(uint8_t)  temperature_lsb", param_data.temperature_lsb);
    ESP_LOGI(TAG, "  %32s = %f", "(float)    temperature_celsius", param_data.temperature_celsius);

    return f_retval;
}

/*********************************************************************************
 * PUBLIC.
 * Read the DS3231 data
 *
 * @doc It is not named "mjd_ds3231_get_datetime()" because it not only returns the datetime props but also the temperature prop.
 *
 *********************************************************************************/
esp_err_t mjd_ds3231_get_data(const mjd_ds3231_config_t* param_ptr_config, mjd_ds3231_data_t* param_ptr_data) {
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
    i2c_master_write_byte(cmd, (param_ptr_config->i2c_slave_addr << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK);
    i2c_master_write_byte(cmd, MJD_DS3231_REGISTER_SECONDS, I2C_MASTER_ACK);
    i2c_master_stop(cmd);
    f_retval = i2c_master_cmd_begin(param_ptr_config->i2c_port_num, cmd, RTOS_DELAY_1SEC);
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
    i2c_master_write_byte(cmd, (param_ptr_config->i2c_slave_addr << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, &param_ptr_data->seconds, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, &param_ptr_data->minutes, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, &param_ptr_data->hours, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, &param_ptr_data->week_day, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, &param_ptr_data->day, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, &param_ptr_data->month, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, &year_raw, I2C_MASTER_NACK); // last read must be NO_ACK
    i2c_master_stop(cmd);
    f_retval = i2c_master_cmd_begin(param_ptr_config->i2c_port_num, cmd, RTOS_DELAY_1SEC);
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
    i2c_master_write_byte(cmd, (param_ptr_config->i2c_slave_addr << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK);
    i2c_master_write_byte(cmd, MJD_DS3231_REGISTER_TEMPERATURE_MSB, I2C_MASTER_ACK);
    i2c_master_stop(cmd);
    f_retval = i2c_master_cmd_begin(param_ptr_config->i2c_port_num, cmd, RTOS_DELAY_1SEC);
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
    i2c_master_write_byte(cmd, (param_ptr_config->i2c_slave_addr << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, &msb, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, &param_ptr_data->temperature_lsb, I2C_MASTER_NACK); // last read must be NO_ACK
    i2c_master_stop(cmd);
    f_retval = i2c_master_cmd_begin(param_ptr_config->i2c_port_num, cmd, RTOS_DELAY_1SEC);
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
    param_ptr_data->seconds &= 0b01111111; // Keep 7 LSB
    param_ptr_data->seconds = mjd_bcd_to_byte(param_ptr_data->seconds);

    // minutes BCD
    param_ptr_data->minutes &= 0b01111111; // Keep 7 LSB
    param_ptr_data->minutes = mjd_bcd_to_byte(param_ptr_data->minutes);

    // hours BCD
    param_ptr_data->hours &= 0b00111111; // Keep 6 LSB
    param_ptr_data->hours = mjd_bcd_to_byte(param_ptr_data->hours);

    // week_day BCD
    param_ptr_data->week_day = mjd_bcd_to_byte(param_ptr_data->week_day);

    // day BCD
    param_ptr_data->day &= 0b00111111; // Keep 6 LSB
    param_ptr_data->day = mjd_bcd_to_byte(param_ptr_data->day);

    // month BCD
    param_ptr_data->month &= 0b00011111; // Keep 5 LSB
    param_ptr_data->month = mjd_bcd_to_byte(param_ptr_data->month);

    // year BCD @important year storage = 20YY
    param_ptr_data->year = 2000 + mjd_bcd_to_byte(year_raw);

    // temperature
    param_ptr_data->temperature_msb = (int8_t) msb; // convert from (unsigned)uint8_t to (signed)int8_t
    param_ptr_data->temperature_celsius = param_ptr_data->temperature_msb + (0.25 * (param_ptr_data->temperature_lsb >> 6));
    ESP_LOGD(TAG, "%s(). data->temperature_msb:      int8_t  %i", __FUNCTION__, param_ptr_data->temperature_msb);
    ESP_LOGD(TAG, "%s(). data->temperature_lsb:      uint8_t %u", __FUNCTION__, param_ptr_data->temperature_lsb);
    ESP_LOGD(TAG, "%s(). data->temperature_lsb >> 6: uint8_t %u", __FUNCTION__, param_ptr_data->temperature_lsb >> 6);

    // LABEL
    cleanup: ;

    return f_retval;
}

/*********************************************************************************
 * PUBLIC.
 * Set the DS3231 data
 *********************************************************************************/
esp_err_t mjd_ds3231_set_datetime(const mjd_ds3231_config_t* param_ptr_config, mjd_ds3231_data_t param_data) {
    ESP_LOGD(TAG, "%s()", __FUNCTION__);

    esp_err_t f_retval = ESP_OK;
    i2c_cmd_handle_t cmd;

    // Validation
    bool validation_passed = true;
    if (param_data.year > MJD_DS3231_MAXVAL_YEAR) {
        validation_passed = false;
        ESP_LOGE(TAG, "%s(). ABORT. param data->year (%u) > %u", __FUNCTION__, param_data.year, MJD_DS3231_MAXVAL_YEAR);
    }
    if (param_data.month > MJD_DS3231_MAXVAL_MONTH) {
        validation_passed = false;
        ESP_LOGE(TAG, "%s(). ABORT. param data->month (%u) > %u", __FUNCTION__, param_data.month,
                MJD_DS3231_MAXVAL_MONTH);
    }
    if (param_data.day > MJD_DS3231_MAXVAL_DAY) {
        validation_passed = false;
        ESP_LOGE(TAG, "%s(). ABORT. param data->day (%u) > %u", __FUNCTION__, param_data.day, MJD_DS3231_MAXVAL_DAY);
    }
    if (param_data.hours > MJD_DS3231_MAXVAL_HOURS) {
        validation_passed = false;
        ESP_LOGE(TAG, "%s(). ABORT. param data->hours (%u) > %u", __FUNCTION__, param_data.hours,
                MJD_DS3231_MAXVAL_HOURS);
    }
    if (param_data.minutes > MJD_DS3231_MAXVAL_MINUTES) {
        validation_passed = false;
        ESP_LOGE(TAG, "%s(). ABORT. param data->minutes (%u) > %u", __FUNCTION__, param_data.minutes,
                MJD_DS3231_MAXVAL_MINUTES);
    }
    if (param_data.seconds > MJD_DS3231_MAXVAL_SECONDS) {
        validation_passed = false;
        ESP_LOGE(TAG, "%s(). ABORT. param data->seconds (%u) > %u", __FUNCTION__, param_data.seconds,
                MJD_DS3231_MAXVAL_SECONDS);
    }
    if (param_data.week_day > MJD_DS3231_MAXVAL_WEEK_DAY) {
        validation_passed = false;
        ESP_LOGE(TAG, "%s(). ABORT. param data->week_day (%u) > %u", __FUNCTION__, param_data.week_day,
                MJD_DS3231_MAXVAL_WEEK_DAY);
    }
    if (validation_passed == false) {
        f_retval = ESP_ERR_INVALID_ARG;
        ESP_LOGE(TAG, "%s(). ABORT. Invalid input parameter(s) | err %i (%s)", __FUNCTION__, f_retval,
                esp_err_to_name(f_retval));
        // GOTO
        goto cleanup;
    }

    // Extract & transform field values
    uint8_t seconds = mjd_byte_to_bcd(param_data.seconds);
    uint8_t minutes = mjd_byte_to_bcd(param_data.minutes);
    uint8_t hours = mjd_byte_to_bcd(param_data.hours);
    uint8_t week_day = mjd_byte_to_bcd(param_data.week_day);
    uint8_t day = mjd_byte_to_bcd(param_data.day);
    uint8_t month = mjd_byte_to_bcd(param_data.month);
    uint8_t year = mjd_byte_to_bcd(param_data.year - 2000); // 20XX

    // Send write request. Bulk write.
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (param_ptr_config->i2c_slave_addr << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK);
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
    f_retval = i2c_master_cmd_begin(param_ptr_config->i2c_port_num, cmd, RTOS_DELAY_1SEC);
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

/*********************************************************************************
 * PUBLIC.
 * Apply RTC Time to MCU TimeOfDay.
 *
 * @problem https://github.com/espressif/esp-idf/issues/584
 *          The ESP32 function mktime() does not support datetime values with year >= 2038!
 *          This does affect this func when trying to set the datetime of the ESP32 MCU!
 *
 *********************************************************************************/
esp_err_t mjd_ds3231_apply_rtc_time_to_mcu(const mjd_ds3231_config_t* param_ptr_config) {
    ESP_LOGD(TAG, "%s()", __FUNCTION__);

    esp_err_t f_retval = ESP_OK;

    /*
     * Read RTC
     */
    mjd_ds3231_data_t data = MJD_DS3231_DATA_DEFAULT();
    f_retval = mjd_ds3231_get_data(param_ptr_config, &data);
    if (f_retval != ESP_OK) {
        ESP_LOGE(TAG, "%s(). mjd_ds3231_get_data() | err %i %s", __FUNCTION__, f_retval, esp_err_to_name(f_retval));
        // GOTO
        goto cleanup;
    }
    ESP_LOGD(TAG, "%s(). year %u | month %u | day %u | hours %u | minutes %u | seconds %u", __FUNCTION__, data.year,
            data.month, data.day, data.hours, data.minutes, data.seconds);

    /*
     * Check problem year >= MJD_DS3231_MAXVAL_MKTIME_YEAR
     */
    if (data.year > MJD_DS3231_MAXVAL_MKTIME_YEAR) {
        f_retval = ESP_ERR_INVALID_ARG;
        ESP_LOGE(TAG, "%s(). ABORT. data.year %u >= %u [bug newlib mktime()]", __FUNCTION__, data.year, MJD_DS3231_MAXVAL_MKTIME_YEAR);
        // GOTO
        goto cleanup;
    }

    /*
     * Apply RTC Time to MCU Time
     *
     * struct tm http://www.cplusplus.com/reference/ctime/tm/
     *      .tm_year  years since 1900
     *      .tm_mon   0-11
     *      .tm_mday  1-31
     *      .tm_hour  0-23
     *      .tm_min   0-59
     *      .tm_sec   0-60
     *      .tm_isdst The Daylight Saving Time flag (tm_isdst) is greater than zero if Daylight Saving Time is in effect, zero if Daylight Saving Time is not in effect, and less than zero if the information is not available.
     */
    struct tm the_tm =
        { 0 };
    the_tm.tm_year = data.year - 1900;
    the_tm.tm_mon = data.month - 1; // @important RTC tm_mon 0..11; MCU month 1..12
    the_tm.tm_mday = data.day;
    the_tm.tm_hour = data.hours;
    the_tm.tm_min = data.minutes;
    the_tm.tm_sec = data.seconds;
    the_tm.tm_isdst = 0;
    time_t epoch_time = mktime(&the_tm);
    ESP_LOGD(TAG, "%s(). ctime(&epoch_time): %s", __FUNCTION__, ctime(&epoch_time));

    struct timeval the_timeval =
                { 0 }; // @important Does .tv_usec = 0
    the_timeval.tv_sec = epoch_time;
    settimeofday(&the_timeval, NULL);

    // LABEL
    cleanup: ;

    return f_retval;
}

/*********************************************************************************
 * PUBLIC.
 * Save MCU TimeOfDay to the RTC chip.
 *
 * struct tm http://www.cplusplus.com/reference/ctime/tm/
 *      .tm_year  years since 1900
 *      .tm_mon   0-11  RTC month 1..12
 *      .tm_mday  1-31
 *      .tm_hour  0-23
 *      .tm_min   0-59
 *      .tm_sec   0-60
 *      .tm_isdst The Daylight Saving Time flag (tm_isdst) is greater than zero if Daylight Saving Time is in effect, zero if Daylight Saving Time is not in effect, and less than zero if the information is not available.
 *
 *********************************************************************************/
esp_err_t mjd_ds3231_save_mcu_time_to_rtc(const mjd_ds3231_config_t* param_ptr_config) {
    ESP_LOGD(TAG, "%s()", __FUNCTION__);

    esp_err_t f_retval = ESP_OK;

    /*
     * Get MCU TimeOfDay
     */
    struct timeval the_timeval = { 0 };
    gettimeofday(&the_timeval, NULL);
    struct tm* the_tm_ptr;
    the_tm_ptr = localtime(&the_timeval.tv_sec);
    ESP_LOGD(TAG, "%s(). the_tm: %s\n", __FUNCTION__, asctime(the_tm_ptr));

    /*
     * Save to RTC
     */
    mjd_ds3231_data_t data = MJD_DS3231_DATA_DEFAULT();
    data.year = the_tm_ptr->tm_year + 1900; // @important
    data.month = the_tm_ptr->tm_mon + 1; // @important
    data.day = the_tm_ptr->tm_mday;
    data.hours = the_tm_ptr->tm_hour;
    data.minutes = the_tm_ptr->tm_min;
    data.seconds = the_tm_ptr->tm_sec;
    ESP_LOGD(TAG, "%s(). mjd_ds3231_data_t data: year %u | month %u | day %u | hours %u | minutes %u | seconds %u", __FUNCTION__,
             data.year, data.month, data.day, data.hours, data.minutes, data.seconds);

    f_retval = mjd_ds3231_set_datetime(param_ptr_config, data);
    if (f_retval != ESP_OK) {
        ESP_LOGE(TAG, "%s(). mjd_ds3231_set_data() | err %i %s", __FUNCTION__, f_retval, esp_err_to_name(f_retval));
        // GOTO
        goto cleanup;
    }

    // LABEL
    cleanup: ;

    return f_retval;
}
