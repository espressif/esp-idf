/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_attr.h"
#include "test_pulse_cnt_board.h"

// helper function to simulate several rising edges on gpio
IRAM_ATTR void test_gpio_simulate_rising_edge(int gpio_sig, size_t times)
{
    while (times--) {
        gpio_set_level(gpio_sig, 0);
        gpio_set_level(gpio_sig, 1);
    }
}

// helper function to simulate several groups of quadrature signals
IRAM_ATTR void test_gpio_simulate_quadrature_signals(int gpio_sig_a, int gpio_sig_b, size_t times)
{
    while (times--) {
        gpio_set_level(gpio_sig_a, 1);
        gpio_set_level(gpio_sig_b, 0);
        vTaskDelay(1);
        gpio_set_level(gpio_sig_a, 0);
        gpio_set_level(gpio_sig_b, 0);
        vTaskDelay(1);
        gpio_set_level(gpio_sig_a, 0);
        gpio_set_level(gpio_sig_b, 1);
        vTaskDelay(1);
        gpio_set_level(gpio_sig_a, 1);
        gpio_set_level(gpio_sig_b, 1);
        vTaskDelay(1);
    }
}
