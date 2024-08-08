/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "esp_log.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio_control/gpio_control.h"
#include "uart_control/uart_control.h"
#include "external_power_control_example.h"

static const char *TAG = "ext_pw_ctrl_main";

void app_main(void)
{
#if !CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP || !CONFIG_IDF_TARGET_ESP32
    static uart_ctrl_context_t s_uart_context;
#endif
    static gpio_ctrl_context_t s_gpio_context;

#if !CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP || !CONFIG_IDF_TARGET_ESP32
    ESP_ERROR_CHECK( example_register_uart_control(&s_uart_context) );
#endif
    ESP_ERROR_CHECK( example_register_gpio_control(&s_gpio_context) );

    ESP_LOGI(TAG, "All init success");
}