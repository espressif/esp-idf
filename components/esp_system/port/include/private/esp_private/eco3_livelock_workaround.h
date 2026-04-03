/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_ESP32_ECO3_CACHE_LOCK_FIX

/**
 * @brief Enable or disable the livelock workaround and reconfigure the interrupt WDT.
 *
 * @param enable true to enable livelock workaround, false to disable
 */
void esp_int_wdt_livelock_workaround(bool enable);

/**
 * @brief Get stage0 ticks for a feed (from current livelock params).
 *
 * @return stage0 timeout ticks
 */
uint32_t esp_int_wdt_livelock_get_feed_stage0_ticks(void);

/**
 * @brief Initialize interrupt watchdog for livelock fix workaround
 */
void esp_int_wdt_init_for_livelock_fix(void);

/**
 * @brief Set livelock workaround params (window count from timeout, reset counter)
 *
 * Call at CPU init and when (re-)enabling the livelock workaround.
 *
 * @param timeout_ms Interrupt watchdog timeout in milliseconds
 */
void esp_int_wdt_set_livelock_params(uint32_t timeout_ms);

/**
 * @brief Reset livelock params (counter and max to 0) when disabling the livelock workaround
 */
void esp_int_wdt_reset_livelock_params(void);

#endif // CONFIG_ESP32_ECO3_CACHE_LOCK_FIX

#ifdef __cplusplus
}
#endif
