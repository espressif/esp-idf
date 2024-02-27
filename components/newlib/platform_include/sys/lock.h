/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include_next <sys/lock.h>
#include "sdkconfig.h"

#ifdef _RETARGETABLE_LOCKING

/* Actual platfrom-specific definition of struct __lock.
 * The size here should be sufficient for a FreeRTOS mutex.
 * This is checked by a static assertion in locks.c
 *
 * Note: this might need to be made dependent on whether FreeRTOS
 * is included in the build.
 */
struct __lock {
#if (CONFIG_FREERTOS_USE_LIST_DATA_INTEGRITY_CHECK_BYTES && CONFIG_FREERTOS_USE_TRACE_FACILITY)
    int reserved[29];
#elif (CONFIG_FREERTOS_USE_LIST_DATA_INTEGRITY_CHECK_BYTES && !CONFIG_FREERTOS_USE_TRACE_FACILITY)
    int reserved[27];
#elif (!CONFIG_FREERTOS_USE_LIST_DATA_INTEGRITY_CHECK_BYTES && CONFIG_FREERTOS_USE_TRACE_FACILITY)
    int reserved[23];
#else
    int reserved[21];
#endif /* #if (CONFIG_FREERTOS_USE_LIST_DATA_INTEGRITY_CHECK_BYTES && CONFIG_FREERTOS_USE_TRACE_FACILITY) */
};

/* Compatibility definitions for the legacy ESP-specific locking implementation.
 * These used to be provided by libc/sys/xtensa/sys/lock.h in newlib.
 * Newer versions of newlib don't have this ESP-specific lock.h header, and are
 * built with _RETARGETABLE_LOCKING enabled, instead.
 */

typedef _LOCK_T _lock_t;

void _lock_init(_lock_t *plock);
void _lock_init_recursive(_lock_t *plock);
void _lock_close(_lock_t *plock);
void _lock_close_recursive(_lock_t *plock);
void _lock_acquire(_lock_t *plock);
void _lock_acquire_recursive(_lock_t *plock);
int _lock_try_acquire(_lock_t *plock);
int _lock_try_acquire_recursive(_lock_t *plock);
void _lock_release(_lock_t *plock);
void _lock_release_recursive(_lock_t *plock);

#endif // _RETARGETABLE_LOCKING
