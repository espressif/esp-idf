#pragma once

#include_next <sys/lock.h>

#ifdef _RETARGETABLE_LOCKING

/* Actual platfrom-specific definition of struct __lock.
 * The size here should be sufficient for a FreeRTOS mutex.
 * This is checked by a static assertion in locks.c
 *
 * Note 1: this might need to be made dependent on whether FreeRTOS
 * is included in the build.
 *
 * Note 2: the size is made sufficient for the case when
 * configUSE_TRACE_FACILITY is enabled. If it is disabled,
 * this definition wastes 8 bytes.
 */
struct __lock {
   int reserved[23];
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
