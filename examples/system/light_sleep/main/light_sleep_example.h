/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

void example_wait_gpio_inactive(void);

esp_err_t example_register_gpio_wakeup(void);

esp_err_t example_register_timer_wakeup(void);

esp_err_t example_register_uart_wakeup(void);

#ifdef __cplusplus
}
#endif
