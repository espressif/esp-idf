/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the non-CPU-specific parts of interrupt watchdog.
 *
 * This function is automatically called during application startup if the
 * interrupt watchdog is enabled in menuconfig.
 */
void esp_int_wdt_init(void);

/**
 * @brief Enable the interrupt watchdog on the current CPU.
 *
 * This function is automatically called during application startup for each CPU
 * that has enabled the interrupt watchdog in menuconfig.
 *
 * @note esp_int_wdt_init() must be called first before calling this function
 */
void esp_int_wdt_cpu_init(void);

#ifdef __cplusplus
}
#endif
