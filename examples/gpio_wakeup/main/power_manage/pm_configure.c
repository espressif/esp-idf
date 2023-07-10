/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "esp_pm.h"
#include "esp_check.h"
#include "gpio_wakeup_example.h"

static const char *TAG = "power_config";

/* pm config */
esp_err_t power_config(gpio_ws_t* arg)
{
    gpio_ws_t* gpio_ws = arg;
    /* Initialize pm lock */
    ESP_RETURN_ON_ERROR(esp_pm_lock_create(EXAMPLE_PM_LOCK_TYPE, 0, "contorl_light_sleep", &(gpio_ws->pm_lock)), TAG, "create lock failed");
    ESP_LOGI(TAG, "create %s lock success!", PM_LOCK_TYPE_TO_STRING);

    /* Initialize pm configure,eg: auto light sleep */
    esp_pm_config_t pm_conf = {};
    pm_conf.max_freq_mhz = EXAMPLE_PM_CONFIG_MAX_FREQ_MHZ;
    pm_conf.min_freq_mhz = EXAMPLE_PM_CONFIG_MIN_FREQ_MHZ;
    pm_conf.light_sleep_enable = true;
    ESP_RETURN_ON_ERROR(esp_pm_configure(&pm_conf), TAG, "auto light sleep config failed");

    // after power on, it should remain active, so it should acquire lock
    ESP_RETURN_ON_ERROR(esp_pm_lock_acquire(gpio_ws->pm_lock), TAG, "acquire %s lock failed", PM_LOCK_TYPE_TO_STRING);
    ESP_LOGI(TAG, "acquired %s lock, system is active", PM_LOCK_TYPE_TO_STRING);

    // set the initial software state: hold lock
    gpio_ws->hold_lock_state = HOLD_LOCK_STATE;
    ESP_LOGI(TAG, "Set software state success: %s", gpio_ws->hold_lock_state ? "hold lock" : "no lock");

    return ESP_OK;
}


