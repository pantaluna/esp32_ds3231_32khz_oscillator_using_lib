#include "mjd.h"
#include "mjd_ds3231.h"
#include "mjd_net.h"
#include "mjd_wifi.h"

/*
 * Logging
 */
static const char TAG[] = "myapp";

/*
 * KConfig:
 * - LED
 * - sensor GPIO's
 */
static const int MY_LED_ON_DEVBOARD_GPIO_NUM = CONFIG_MY_LED_ON_DEVBOARD_GPIO_NUM;
static const int MY_LED_ON_DEVBOARD_WIRING_TYPE = CONFIG_MY_LED_ON_DEVBOARD_WIRING_TYPE;
static const char *MY_WIFI_SSID = CONFIG_MY_WIFI_SSID;
static const char *MY_WIFI_PASSWORD = CONFIG_MY_WIFI_PASSWORD;

/*
 * Config:
 *
 */

/*
 * FreeRTOS settings
 */
#define MYAPP_RTOS_TASK_STACK_SIZE_8K (8192)
#define MYAPP_RTOS_TASK_PRIORITY_NORMAL (RTOS_TASK_PRIORITY_NORMAL)

/*
 * TASKS
 */
void main_task(void *pvParameter) {
    ESP_LOGD(TAG, "%s()", __FUNCTION__);

    /********************************************************************************
     * Reuseable variables
     */
    esp_err_t f_retval = ESP_OK;

    /*********************************
     * LOGGING
     * Optional for Production: dump less messages
     * @doc It is possible to lower the log level for specific modules.
     *
     */
    esp_log_level_set("memory_layout", ESP_LOG_WARN); // @important Disable INFO messages which are too detailed for me.
    esp_log_level_set("nvs", ESP_LOG_WARN); // @important Disable INFO messages which are too detailed for me.
    esp_log_level_set("phy_init", ESP_LOG_WARN); // @important Disable INFO messages which are too detailed for me.
    esp_log_level_set("tcpip_adapter", ESP_LOG_WARN); // @important Disable INFO messages which are too detailed for me.
    esp_log_level_set("wifi", ESP_LOG_WARN); // @important Disable INFO messages which are too detailed for me.

    /********************************************************************************
     * SOC init
     *
     * @doc exec nvs_flash_init() - mandatory for Wifi to work later on
     *
     */
// DEVTEMP_BEGIN Fix for ESP-IDF github tag v3.2-dev
#ifndef ESP_ERR_NVS_NEW_VERSION_FOUND
#define ESP_ERR_NVS_NEW_VERSION_FOUND (ESP_ERR_NVS_BASE + 0xff)
#endif
// DEVTEMP-END

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_LOGW(TAG, "  ESP_ERR_NVS_NO_FREE_PAGES - do nvs_flash_erase()");
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    } else if (err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "  ESP_ERR_NVS_NEW_VERSION_FOUND - do nvs_flash_erase()");
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    /********************************************************************************
     * STANDARD Init
     */
    mjd_set_timezone_utc();
    mjd_log_time();

    mjd_log_wakeup_details();
    mjd_increment_mcu_boot_count();
    mjd_log_mcu_boot_count();

    /********************************************************************************
     * KConfig
     */
    ESP_LOGI(TAG, "KConfig:");
    ESP_LOGI(TAG, "MY_LED_ON_DEVBOARD_GPIO_NUM:    %i", MY_LED_ON_DEVBOARD_GPIO_NUM);
    ESP_LOGI(TAG, "MY_LED_ON_DEVBOARD_WIRING_TYPE: %i", MY_LED_ON_DEVBOARD_WIRING_TYPE);

    /********************************************************************************
     * Config
     */

    /********************************************************************************
     * LED
     */
    mjd_led_config_t led_config =
                { 0 };
    led_config.gpio_num = MY_LED_ON_DEVBOARD_GPIO_NUM;
    led_config.wiring_type = MY_LED_ON_DEVBOARD_WIRING_TYPE; // 1 GND MCU Huzzah32 | 2 VCC MCU Lolin32lite
    mjd_led_config(&led_config);

    /********************************************************************************
     * NET - SNTP sync datetime
     *
     * @important Do this only once when the MCU booting for the first time (so not after subsequent deep sleep periods).
     *
     */
    ESP_LOGI(TAG, "***SECTION: NET - SNTP sync datetime***");
    if (mjd_get_mcu_boot_count() > 1) {
        ESP_LOGI(TAG, "  Sync not done because boot count > 1");
    } else {
        ESP_LOGI(TAG, "  Sync required because boot count == 1");

        mjd_led_on(MY_LED_ON_DEVBOARD_GPIO_NUM);
        ESP_LOGI(TAG, "MY_WIFI_SSID:     %s", MY_WIFI_SSID);
        ESP_LOGI(TAG, "MY_WIFI_PASSWORD: %s", MY_WIFI_PASSWORD);
        f_retval = mjd_wifi_sta_init(MY_WIFI_SSID, MY_WIFI_PASSWORD);
        if (f_retval != ESP_OK) {
            ESP_LOGE(TAG, "mjd_wifi_sta_init() err %i (%s)", f_retval, esp_err_to_name(f_retval));
            ESP_LOGE(TAG, "ABORT app due to wifi_init error");
            // GOTO
            goto cleanup;
        }
        f_retval = mjd_wifi_sta_start();
        if (f_retval != ESP_OK) {
            ESP_LOGE(TAG, "mjd_wifi_sta_start() err %i (%s)", f_retval, esp_err_to_name(f_retval));
            ESP_LOGE(TAG, "ABORT app due to wifi_start error");
            // GOTO
            goto cleanup;
        }
        f_retval = mjd_net_sync_current_datetime(false);
        if (f_retval != ESP_OK) {
            ESP_LOGE(TAG, "mjd_net_sync_current_datetime(false) err %i (%s)", f_retval, esp_err_to_name(f_retval));
            ESP_LOGE(TAG, "COULD NOT sync datetime due to error");
        }

        mjd_wifi_sta_disconnect_stop();
        mjd_led_off(MY_LED_ON_DEVBOARD_GPIO_NUM);
        mjd_log_time();

        // GOTO
        goto deep_sleep;
    }

    /********************************************************************************
     * MAIN
     */
    ESP_LOGI(TAG, "***SECTION: MAIN: null...***");

    /********************************************************************************
     * DEEP SLEEP & RESTART TIMER
     * @important In deep sleep mode, wireless peripherals are powered down. Before entering sleep mode, applications must disable WiFi and BT using appropriate calls (esp_wifi_stop(), esp_bluedroid_disable(), esp_bt_controller_disable()).
     * @doc https://esp-idf.readthedocs.io/en/latest/api-reference/system/sleep_modes.html
     *
     */

    // LABEL
    deep_sleep:

    ESP_LOGI(TAG, "***SECTION: DEEP SLEEP***");

    const uint32_t MY_DEEP_SLEEP_TIME_SEC = 5 * 60; // 5 5*60 24*60
    esp_sleep_enable_timer_wakeup(mjd_seconds_to_microseconds(MY_DEEP_SLEEP_TIME_SEC));
    ESP_LOGI(TAG, "Entering deep sleep (the MCU should wake up %u seconds later)...", MY_DEEP_SLEEP_TIME_SEC);

    ESP_LOGI(TAG, "==> NEXT STEPS:");
    ESP_LOGI(TAG, "- Let the app cycle through periods of power-on and deep sleep.");
    ESP_LOGI(TAG,
            "- Check in the debug-log that the datetime is still accurate after 6 hours - 1 day (it should, with the external XO).");
    mjd_log_time();

    esp_deep_sleep_start();

    /********************************************************************************
     * LABEL
     */
    cleanup: ;

    /********************************************************************************
     * Task Delete
     * @doc Passing NULL will end the current task
     * @doc The code will only come here in case of errors.
     */
    ESP_LOGD(TAG, "END OF %s()", __FUNCTION__);
    vTaskDelete(NULL);
}

/*
 * MAIN
 */
void app_main() {
    ESP_LOGD(TAG, "%s()", __FUNCTION__);

    /**********
     * CREATE TASK:
     * @important For stability (RMT + Wifi etc.): always use xTaskCreatePinnedToCore(APP_CPU_NUM) [Opposed to xTaskCreate()]
     */
    BaseType_t xReturned;
    xReturned = xTaskCreatePinnedToCore(&main_task, "main_task (name)", MYAPP_RTOS_TASK_STACK_SIZE_8K, NULL,
    MYAPP_RTOS_TASK_PRIORITY_NORMAL, NULL,
    APP_CPU_NUM);
    if (xReturned == pdPASS) {
        ESP_LOGI(TAG, "OK Task has been created, and is running right now");
    } else {
        ESP_LOGE(TAG, "Cannot start task!");
    }

    /**********
     * END
     */
    ESP_LOGD(TAG, "END %s()", __FUNCTION__);
}
