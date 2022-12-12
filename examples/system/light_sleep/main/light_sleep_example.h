/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void example_wait_gpio_inactive(void);

esp_err_t example_register_gpio_wakeup(void);

esp_err_t example_register_timer_wakeup(void);

esp_err_t example_register_uart_wakeup(void);

#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
void example_register_touch_wakeup(void);
#endif

#ifdef __cplusplus
}
#endif
