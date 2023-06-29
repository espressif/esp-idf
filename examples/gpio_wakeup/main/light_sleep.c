/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "esp_pm.h"
#include "esp_check.h"
#include "gpio_wakeup_example.h"

static const char *TAG = "light_sleep";

esp_err_t light_sleep_config(void)
{
    /* Initialize pm for auto light sleep */
    esp_pm_config_t pm_conf = {};
    pm_conf.max_freq_mhz = CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ;
    pm_conf.min_freq_mhz = CONFIG_XTAL_FREQ;
    pm_conf.light_sleep_enable = true;// enable auto light sleep

    ESP_RETURN_ON_ERROR(esp_pm_configure(&pm_conf), TAG, "light sleep config failed");

    return ESP_OK;
}


