/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "esp_pm.h"
#include "esp_check.h"
#include "gpio_wakeup_example.h"

static const char *TAG = "light_sleep";

esp_err_t power_config(void)
{
    /* Initialize pm lock */
    esp_pm_lock_type_t pm_lock = PM_LOCK_TYPE;
    ESP_RETURN_ON_ERROR(esp_pm_lock_create(pm_lock, 0, "contorl_light_sleep", &light_sleep_handle), TAG, "create lock failed");
    ESP_LOGI(TAG, "create pm lock success!");

    /* Initialize pm for auto light sleep */
    esp_pm_config_t pm_conf = {};
    pm_conf.max_freq_mhz = PM_CONFIG_MAX_FREQ_MHZ;
    pm_conf.min_freq_mhz = PM_CONFIG_MIN_FREQ_MHZ;
    pm_conf.light_sleep_enable = true;// enable auto light sleep

    ESP_RETURN_ON_ERROR(esp_pm_configure(&pm_conf), TAG, "light sleep config failed");

    //系统上电后，应保持active，所以应acquire lock
    ESP_RETURN_ON_ERROR(esp_pm_lock_acquire(light_sleep_handle), TAG, "acquire pm lock failed");
    ESP_LOGI(TAG, "acquired pm lock, system is active");

    //设置初始的软件状态
    SET_SOFTWARE_STATE(ACTIVE_STATE);
    ESP_LOGI(TAG, "Set software state success");

    return ESP_OK;
}


