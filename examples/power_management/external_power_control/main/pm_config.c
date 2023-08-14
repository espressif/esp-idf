/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "esp_pm.h"
#include "esp_check.h"
#include "gpio_control/gpio_control.h"
#include "uart_control/uart_control.h"
#include "external_power_control_example.h"

/* Max and min frequencies set by menuconfig */
#define DEFAULT_PM_MAX_FREQ_MHZ (CONFIG_EXAMPLE_PM_MAX_FREQ_MHZ)
#define DEFAULT_PM_MIN_FREQ_MHZ	(CONFIG_EXAMPLE_PM_MIN_FREQ_MHZ)

static const char *TAG = "power_config";

/**
 * @brief  Initialize power management unit. 
 * 
 * The system should remain active after initialization.
 * The function first creates a lock based on the user-selected 
 * lock type, stored in the parameter `pm_lock`. The power management 
 * unit is then configured according to the set maximum and minimum 
 * frequencies. Immediately after that the lock should be acquired, 
 * thus ensuring that the system is active. Finally the current lock 
 * status is then recorded: acquired.
 * 
 * @param[out] pm_lock      the power management lock
 * @param[out] lock_state   record whether the power management lock was acquired or released
 * 
 * @return
 *  - ESP_OK on success
 */
esp_err_t example_register_power_config(esp_pm_lock_handle_t* pm_lock, bool* lock_state)
{
    /* Initialize power management lock */
    ESP_RETURN_ON_ERROR( esp_pm_lock_create(DEFAULT_PM_LOCK_TYPE, 0, "contorl_auto_light_sleep", pm_lock), TAG, "create lock failed" );
    ESP_LOGI(TAG, "create %s lock success!", PM_LOCK_TYPE_TO_STRING);

    /* Power management configuration */
    esp_pm_config_t pm_conf = {
        .max_freq_mhz = DEFAULT_PM_MAX_FREQ_MHZ,
        .min_freq_mhz = DEFAULT_PM_MIN_FREQ_MHZ,
        .light_sleep_enable = true
    };
    ESP_RETURN_ON_ERROR( esp_pm_configure(&pm_conf), TAG, "auto light sleep config failed" );

    // after power on, it should remain active, so it should acquire lock
    ESP_RETURN_ON_ERROR( esp_pm_lock_acquire(*pm_lock), TAG, "acquire %s lock failed", PM_LOCK_TYPE_TO_STRING );
    ESP_LOGI(TAG, "acquired %s lock, can to do something...", PM_LOCK_TYPE_TO_STRING);

    // Record power management lock has been acquired
    *lock_state = ACQUIRED_LOCK;
    ESP_LOGI(TAG, "Set software state success: %s", (*lock_state) ? "acquired lock" : "released lock");

    return ESP_OK;
}
