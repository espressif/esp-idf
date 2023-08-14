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


static const char *TAG = "external_power_control_main";


void app_main(void)
{ 
    static gpio_ctrl_context_t s_gpio_control;
    static uart_ctrl_context_t s_uart_control;

    /* Register gpio control */
    ESP_ERROR_CHECK( example_register_gpio_control(&s_gpio_control) );

    /* Register uart control */
    ESP_ERROR_CHECK( example_register_uart_control(&s_uart_control) );

    ESP_LOGI(TAG, "All init success");
}
