/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
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
#include "gpio_wakeup_sleep/gpio_wakeup_sleep.h"
#include "uart_wakeup_sleep/uart_wakeup_sleep.h"
#include "auto_light_sleep_example.h"


static const char *TAG = "auto_light_sleep_main";


#if CONFIG_IDF_TARGET_ESP32H2 || CONFIG_IDF_TARGET_ESP32C6
    extern esp_err_t sleep_clock_system_retention_init(void);
    extern esp_err_t sleep_sys_periph_retention_init(void);
    extern esp_err_t esp_sleep_cpu_retention_init(void);
#endif

void app_main(void)
{ 
#if CONFIG_IDF_TARGET_ESP32H2 || CONFIG_IDF_TARGET_ESP32C6
    sleep_clock_system_retention_init();
    sleep_sys_periph_retention_init();
    esp_sleep_cpu_retention_init();
#endif
    // In order to ensure that the variables still exist after the main function is launched, the static keyword must be used
    static gpio_wakeup_object_t gpio_wakeup_sleep;
    static uart_wakeup_object_t uart_wakeup_sleep;

    // config gpio interrupt、gpio wake、lpio wakeup、ext1
    ESP_ERROR_CHECK( example_register_gpio_wakeup_sleep(&gpio_wakeup_sleep) );

    // config uart wakeup
    ESP_ERROR_CHECK( example_register_uart_wakeup_sleep(&uart_wakeup_sleep) );

    ESP_LOGI(TAG, "All init success");
}
