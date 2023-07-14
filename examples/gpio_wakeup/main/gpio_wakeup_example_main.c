/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_check.h"
#include "gpio_wakeup_example.h"


static const char *TAG = "gpio_wake_up_main";

#if IDF_TARGET_ESP32H2
    extern esp_err_t sleep_clock_system_retention_init(void);
    extern esp_err_t sleep_sys_periph_retention_init(void);
    extern esp_err_t esp_sleep_cpu_retention_init(void);
#endif

/* Initialization of the entire gpio wake-up example */
esp_err_t all_config_init(gpio_ws_t *arg)
{
    /* should first pm config, create lock, and then gpio intr configuration 
    (to avoid the problems caused by first configuring gpio intr when the lock has not been created) */

    // auto light sleep and light sleep lock init
    ESP_RETURN_ON_ERROR(power_config(arg), TAG, "power_config error");
    // config gpio interrupt、gpio wake
    ESP_RETURN_ON_ERROR(example_register_gpio_wakeup_sleep(arg), TAG, "example_register_gpio_wakeup error");

    return ESP_OK;
}

void app_main(void)
{ 
#if IDF_TARGET_ESP32H2
    sleep_clock_system_retention_init();
    sleep_sys_periph_retention_init();
    esp_sleep_cpu_retention_init();
#endif
    // In order to ensure that the variables still exist after the main function is launched, the static keyword must be used
    static gpio_ws_t gpio_wakeup_sleep;
    // all init: pm config、gpio config、gpio interrupt...
    ESP_ERROR_CHECK(all_config_init(&gpio_wakeup_sleep));

    ESP_LOGI(TAG, "All init success");
}
