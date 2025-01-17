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

#define TOUCH_LSLEEP_SUPPORTED (SOC_TOUCH_SENSOR_SUPPORTED && SOC_PM_SUPPORT_TOUCH_SENSOR_WAKEUP)

void example_wait_gpio_inactive(void);

esp_err_t example_register_gpio_wakeup(void);

esp_err_t example_register_timer_wakeup(void);

esp_err_t example_register_uart_wakeup(void);

#if TOUCH_LSLEEP_SUPPORTED
void example_register_touch_wakeup(void);
#endif

#ifdef __cplusplus
}
#endif
