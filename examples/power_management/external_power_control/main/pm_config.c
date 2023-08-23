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
 * @param pm_lock      the power management lock
 * @param lock_state   record whether the power management lock was acquired or released
 * 
 * @return
 *  - ESP_OK on success
 */
esp_err_t example_register_power_config(esp_pm_lock_handle_t *pm_lock, bool *lock_state)
{
    ESP_RETURN_ON_ERROR( esp_pm_lock_create(DEFAULT_PM_LOCK_TYPE, 0, "ctrl_auto_light_sleep", pm_lock), TAG, "create lock failed" );

    esp_pm_config_t config = {
        .max_freq_mhz = DEFAULT_PM_MAX_FREQ_MHZ,
        .min_freq_mhz = DEFAULT_PM_MIN_FREQ_MHZ,
        .light_sleep_enable = true
    };
    ESP_RETURN_ON_ERROR( esp_pm_configure(&config), TAG, "auto light sleep config failed" );

    // after power on, it should remain active, so it should acquire lock
    ESP_RETURN_ON_ERROR( esp_pm_lock_acquire(*pm_lock), TAG, "acquire lock failed" );
    ESP_LOGI(TAG, "Acquired lock at [%p], system remains active", *pm_lock);
    *lock_state = ACQUIRED_LOCK;
    return ESP_OK;
}