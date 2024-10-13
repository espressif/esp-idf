/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

// TODO: [ESP32P4] add P4 when runner is ready
#define EXAMPLE_TOUCH_LSLEEP_WAKEUP_SUPPORT (CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3)

void example_wait_gpio_inactive(void);

esp_err_t example_register_gpio_wakeup(void);

esp_err_t example_register_timer_wakeup(void);

esp_err_t example_register_uart_wakeup(void);

#if EXAMPLE_TOUCH_LSLEEP_WAKEUP_SUPPORT
void example_register_touch_wakeup(void);
#endif

#ifdef __cplusplus
}
#endif
