/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IWDT_INITIAL_TIMEOUT_S  (5)
#define IWDT_TICKS_PER_US       (500)
#define IWDT_STAGE0_TIMEOUT_US  (CONFIG_ESP_INT_WDT_TIMEOUT_MS * 1000 / IWDT_TICKS_PER_US)
#define IWDT_STAGE1_TIMEOUT_US  (2 * IWDT_STAGE0_TIMEOUT_US)
#define IWDT_INITIAL_TIMEOUT_US (IWDT_INITIAL_TIMEOUT_S * 1000000 / IWDT_TICKS_PER_US)

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

#if CONFIG_ESP32_ECO3_CACHE_LOCK_FIX
/**
 * @brief Reconfigure WDT stage timeouts (ticks).
 *
 * Low-level API used by the livelock workaround. Prefer esp_int_wdt_livelock_workaround() for sleep flows.
 *
 * @param stage0_ticks Stage0 timeout ticks
 * @param stage1_ticks Stage1 timeout ticks
 */
void esp_int_wdt_reconfigure_ticks(uint32_t stage0_ticks, uint32_t stage1_ticks);
#endif

#ifdef __cplusplus
}
#endif
