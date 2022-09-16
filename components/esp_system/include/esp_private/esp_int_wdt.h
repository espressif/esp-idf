/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "system_internal.h"
#include "soc/periph_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_TIMER_GROUPS > 1

/* If we have two hardware timer groups, use the second one for interrupt watchdog. */
#define WDT_LEVEL_INTR_SOURCE   ETS_TG1_WDT_LEVEL_INTR_SOURCE
#define IWDT_PRESCALER          MWDT1_TICK_PRESCALER   // Tick period of 500us if WDT source clock is 80MHz
#define IWDT_TICKS_PER_US       MWDT1_TICKS_PER_US
#define IWDT_INSTANCE           WDT_MWDT1
#define IWDT_INITIAL_TIMEOUT_S  5

#else

#define WDT_LEVEL_INTR_SOURCE   ETS_TG0_WDT_LEVEL_INTR_SOURCE
#define IWDT_PRESCALER          MWDT0_TICK_PRESCALER   // Tick period of 500us if WDT source clock is 80MHz
#define IWDT_TICKS_PER_US       MWDT0_TICKS_PER_US
#define IWDT_INSTANCE           WDT_MWDT0
#define IWDT_INITIAL_TIMEOUT_S  5

#endif // SOC_TIMER_GROUPS > 1

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
