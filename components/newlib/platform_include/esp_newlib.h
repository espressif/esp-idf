/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <sys/reent.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Initialize newlib time functions
 */
void esp_newlib_time_init(void);

/**
 * Replacement for newlib's _REENT_INIT_PTR and __sinit.
 *
 * Called from startup code and FreeRTOS, not intended to be called from
 * application code.
 */
void esp_reent_init(struct _reent* r);

/**
 * Clean up some of lazily allocated buffers in REENT structures.
 */
void esp_reent_cleanup(void);

/**
 * Function which sets up newlib in ROM for use with ESP-IDF
 *
 * Includes defining the syscall table, setting up any common locks, etc.
 *
 * Called from the startup code, not intended to be called from application
 * code.
 */
void esp_newlib_init(void);

void esp_setup_syscall_table(void) __attribute__((deprecated("Please call esp_newlib_init() in newer code")));

/**
 * Update current microsecond time from RTC
 */
void esp_set_time_from_rtc(void);

/*
 * Sync timekeeping timers, RTC and high-resolution timer. Update boot_time.
 */
void esp_sync_timekeeping_timers(void);

/* Kept for backward compatibility */
#define esp_sync_counters_rtc_and_frc esp_sync_timekeeping_timers

/**
 * Initialize newlib static locks
 */
void esp_newlib_locks_init(void);

#ifdef __cplusplus
}
#endif
