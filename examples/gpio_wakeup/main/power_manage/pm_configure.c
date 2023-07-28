/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "esp_pm.h"
#include "esp_check.h"
#include "gpio_wakeup_example.h"

/* pm config freq */
#define DEFAULT_PM_MAX_FREQ_MHZ	(CONFIG_EXAMPLE_PM_MAX_FREQ_MHZ)
#define DEFAULT_PM_MIN_FREQ_MHZ	(CONFIG_EXAMPLE_PM_MIN_FREQ_MHZ)

static const char *TAG = "power_config";

/* pm config function */
esp_err_t power_config(gpio_ws_t* args)
{
    gpio_ws_t* object = args;
    /* Initialize pm lock */
    ESP_RETURN_ON_ERROR( esp_pm_lock_create(DEFAULT_PM_LOCK_TYPE, 0, "contorl_auto_light_sleep", &(object->pm_lock)), TAG, "create lock failed" );
    ESP_LOGI(TAG, "create %s lock success!", PM_LOCK_TYPE_TO_STRING);

    /* Initialize pm configure, eg: auto light sleep */
    esp_pm_config_t pm_conf = {
        .max_freq_mhz = DEFAULT_PM_MAX_FREQ_MHZ,
        .min_freq_mhz = DEFAULT_PM_MIN_FREQ_MHZ,
        .light_sleep_enable = true
    };
    ESP_RETURN_ON_ERROR( esp_pm_configure(&pm_conf), TAG, "auto light sleep config failed" );

    // after power on, it should remain active, so it should acquire lock
    ESP_RETURN_ON_ERROR( esp_pm_lock_acquire(object->pm_lock), TAG, "acquire %s lock failed", PM_LOCK_TYPE_TO_STRING );
    ESP_LOGI(TAG, "acquired %s lock, can to do something...", PM_LOCK_TYPE_TO_STRING);

    // set the initial software state: hold lock
    object->hold_lock_state = HOLD_LOCK_STATE;
    ESP_LOGI(TAG, "Set software state success: %s", object->hold_lock_state ? "hold lock" : "no lock");

    return ESP_OK;
}


