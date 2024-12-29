/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* TODO IDF-11226 rename this file to esp_libc.h */
#pragma once

#include <sys/reent.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Function which sets up newlib in ROM for use with ESP-IDF
 *
 * Includes defining the syscall table, setting up any common locks, etc.
 *
 * Called from the startup code, not intended to be called from application
 * code.
 */
void esp_libc_init(void);

void esp_setup_syscall_table(void) __attribute__((deprecated("Please call esp_libc_init() in newer code")));

/**
 * Postponed _GLOBAL_REENT stdio FPs initialization.
 *
 * Can not be a part of esp_reent_init() because stdio device may not initialized yet.
 *
 * Called from startup code and FreeRTOS, not intended to be called from
 * application code.
 *
 */
#if CONFIG_VFS_SUPPORT_IO
void esp_libc_init_global_stdio(const char *stdio_dev);
#else
void esp_libc_init_global_stdio(void);
#endif

void esp_libc_time_init(void);

#if CONFIG_LIBC_NEWLIB
/**
 * Replacement for newlib's _REENT_INIT_PTR and __sinit.
 *
 * Called from startup code and FreeRTOS, not intended to be called from
 * application code.
 */
void esp_reent_init(struct _reent* r);
#endif

/**
 * Clean up some of lazily allocated buffers in REENT structures.
 */
void esp_reent_cleanup(void);

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
 * Initialize libc static locks
 */
void esp_libc_locks_init(void);

/* TODO IDF-11226 */
void esp_newlib_time_init(void) __attribute__((deprecated("Please use esp_libc_time_init instead")));
void esp_newlib_init(void) __attribute__((deprecated("Please use esp_libc_init instead")));
void esp_newlib_locks_init(void) __attribute__((deprecated("Please use esp_libc_locks_init instead")));

#ifdef __cplusplus
}
#endif
