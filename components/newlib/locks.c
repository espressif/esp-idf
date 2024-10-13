/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/lock.h>
#include <stdlib.h>
#include <sys/reent.h>
#include "esp_attr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/portable.h"
#include "esp_rom_caps.h"

/* Notes on our newlib lock implementation:
 *
 * - Use FreeRTOS mutex semaphores as locks.
 * - lock_t is int, but we store an SemaphoreHandle_t there.
 * - Locks are no-ops until the FreeRTOS scheduler is running.
 * - Due to this, locks need to be lazily initialised the first time
 *   they are acquired. Initialisation/deinitialisation of locks is
 *   protected by lock_init_spinlock.
 * - Race conditions around lazy initialisation (via lock_acquire) are
 *   protected against.
 * - Anyone calling lock_close is reponsible for ensuring noone else
 *   is holding the lock at this time.
 * - Race conditions between lock_close & lock_init (for the same lock)
 *   are the responsibility of the caller.
 */

static portMUX_TYPE lock_init_spinlock = portMUX_INITIALIZER_UNLOCKED;

/* Initialize the given lock by allocating a new mutex semaphore
   as the _lock_t value.

   Called by _lock_init*, also called by _lock_acquire* to lazily initialize locks that might have
   been initialised (to zero only) before the RTOS scheduler started.
*/
static void IRAM_ATTR lock_init_generic(_lock_t *lock, uint8_t mutex_type)
{
    portENTER_CRITICAL(&lock_init_spinlock);
    if (*lock) {
        /* Lock already initialised (either we didn't check earlier,
         or it got initialised while we were waiting for the
         spinlock.) */
    } else {
        /* Create a new semaphore

           this is a bit of an API violation, as we're calling the
           private function xQueueCreateMutex(x) directly instead of
           the xSemaphoreCreateMutex / xSemaphoreCreateRecursiveMutex
           wrapper functions...

           The better alternative would be to pass pointers to one of
           the two xSemaphoreCreate___Mutex functions, but as FreeRTOS
           implements these as macros instead of inline functions
           (*party like it's 1998!*) it's not possible to do this
           without writing wrappers. Doing it this way seems much less
           spaghetti-like.
        */
        SemaphoreHandle_t new_sem = xQueueCreateMutex(mutex_type);
        if (!new_sem) {
            abort(); /* No more semaphores available or OOM */
        }
        *lock = (_lock_t)new_sem;
    }
    portEXIT_CRITICAL(&lock_init_spinlock);
}

void IRAM_ATTR _lock_init(_lock_t *lock)
{
    *lock = 0; // In case lock's memory is uninitialized
    lock_init_generic(lock, queueQUEUE_TYPE_MUTEX);
}

void IRAM_ATTR _lock_init_recursive(_lock_t *lock)
{
    *lock = 0; // In case lock's memory is uninitialized
    lock_init_generic(lock, queueQUEUE_TYPE_RECURSIVE_MUTEX);
}

/* Free the mutex semaphore pointed to by *lock, and zero it out.

   Note that FreeRTOS doesn't account for deleting mutexes while they
   are held, and neither do we... so take care not to delete newlib
   locks while they may be held by other tasks!

   Also, deleting a lock in this way will cause it to be lazily
   re-initialised if it is used again. Caller has to avoid doing
   this!
*/
void IRAM_ATTR _lock_close(_lock_t *lock)
{
    portENTER_CRITICAL(&lock_init_spinlock);
    if (*lock) {
        SemaphoreHandle_t h = (SemaphoreHandle_t)(*lock);
#if (INCLUDE_xSemaphoreGetMutexHolder == 1)
        configASSERT(xSemaphoreGetMutexHolder(h) == NULL); /* mutex should not be held */
#endif
        vSemaphoreDelete(h);
        *lock = 0;
    }
    portEXIT_CRITICAL(&lock_init_spinlock);
}

void _lock_close_recursive(_lock_t *lock) __attribute__((alias("_lock_close")));

/* Acquire the mutex semaphore for lock. wait up to delay ticks.
   mutex_type is queueQUEUE_TYPE_RECURSIVE_MUTEX or queueQUEUE_TYPE_MUTEX
*/
static int IRAM_ATTR lock_acquire_generic(_lock_t *lock, uint32_t delay, uint8_t mutex_type)
{
    SemaphoreHandle_t h = (SemaphoreHandle_t)(*lock);
    if (!h) {
        if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
            return 0; /* locking is a no-op before scheduler is up, so this "succeeds" */
        }
        /* lazy initialise lock - might have had a static initializer (that we don't use) */
        lock_init_generic(lock, mutex_type);
        h = (SemaphoreHandle_t)(*lock);
        configASSERT(h != NULL);
    }

    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
        return 0; /* locking is a no-op before scheduler is up, so this "succeeds" */
    }
    BaseType_t success;
    if (!xPortCanYield()) {
        /* In ISR Context */
        if (mutex_type == queueQUEUE_TYPE_RECURSIVE_MUTEX) {
            abort(); /* recursive mutexes make no sense in ISR context */
        }
        BaseType_t higher_task_woken = false;
        success = xSemaphoreTakeFromISR(h, &higher_task_woken);
        if (!success && delay > 0) {
            abort(); /* Tried to block on mutex from ISR, couldn't... rewrite your program to avoid libc interactions in ISRs! */
        }
        if (higher_task_woken) {
            portYIELD_FROM_ISR();
        }
    } else {
        /* In task context */
        if (mutex_type == queueQUEUE_TYPE_RECURSIVE_MUTEX) {
            success = xSemaphoreTakeRecursive(h, delay);
        } else {
            success = xSemaphoreTake(h, delay);
        }
    }

    return (success == pdTRUE) ? 0 : -1;
}

void IRAM_ATTR _lock_acquire(_lock_t *lock)
{
    lock_acquire_generic(lock, portMAX_DELAY, queueQUEUE_TYPE_MUTEX);
}

void IRAM_ATTR _lock_acquire_recursive(_lock_t *lock)
{
    lock_acquire_generic(lock, portMAX_DELAY, queueQUEUE_TYPE_RECURSIVE_MUTEX);
}

int IRAM_ATTR _lock_try_acquire(_lock_t *lock)
{
    return lock_acquire_generic(lock, 0, queueQUEUE_TYPE_MUTEX);
}

int IRAM_ATTR _lock_try_acquire_recursive(_lock_t *lock)
{
    return lock_acquire_generic(lock, 0, queueQUEUE_TYPE_RECURSIVE_MUTEX);
}

/* Release the mutex semaphore for lock.
   mutex_type is queueQUEUE_TYPE_RECURSIVE_MUTEX or queueQUEUE_TYPE_MUTEX
*/
static void IRAM_ATTR lock_release_generic(_lock_t *lock, uint8_t mutex_type)
{
    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
        return; /* locking is a no-op before scheduler is up */
    }
    SemaphoreHandle_t h = (SemaphoreHandle_t)(*lock);
    assert(h);

    if (!xPortCanYield()) {
        if (mutex_type == queueQUEUE_TYPE_RECURSIVE_MUTEX) {
            abort(); /* indicates logic bug, it shouldn't be possible to lock recursively in ISR */
        }
        BaseType_t higher_task_woken = false;
        xSemaphoreGiveFromISR(h, &higher_task_woken);
        if (higher_task_woken) {
            portYIELD_FROM_ISR();
        }
    } else {
        if (mutex_type == queueQUEUE_TYPE_RECURSIVE_MUTEX) {
            xSemaphoreGiveRecursive(h);
        } else {
            xSemaphoreGive(h);
        }
    }
}

void IRAM_ATTR _lock_release(_lock_t *lock)
{
    lock_release_generic(lock, queueQUEUE_TYPE_MUTEX);
}

void IRAM_ATTR _lock_release_recursive(_lock_t *lock)
{
    lock_release_generic(lock, queueQUEUE_TYPE_RECURSIVE_MUTEX);
}

/* To ease the transition to newlib 3.3.0, this part is kept under an ifdef.
 * After the toolchain with newlib 3.3.0 is released and merged, the ifdefs
 * can be removed.
 *
 * Also the retargetable locking functions still rely on the previous
 * implementation. Once support for !_RETARGETABLE_LOCKING is removed,
 * the code can be simplified, removing support for lazy initialization of
 * locks. At the same time, IDF code which relies on _lock_acquire/_lock_release
 * will have to be updated to not depend on lazy initialization.
 *
 * Explanation of the different lock types:
 *
 *  Newlib 2.2.0 and 3.0.0:
 *    _lock_t is defined as int, stores SemaphoreHandle_t.
 *
 *  Newlib 3.3.0:
 *    struct __lock is (or contains) StaticSemaphore_t
 *    _LOCK_T is a pointer to struct __lock, equivalent to SemaphoreHandle_t.
 *    It has the same meaning as _lock_t in the previous implementation.
 *
 */

/* This ensures the platform-specific definition in lock.h is correct.
 * We use "greater or equal" since the size of StaticSemaphore_t may
 * vary by 2 words, depending on whether configUSE_TRACE_FACILITY is enabled.
 */
_Static_assert(sizeof(struct __lock) >= sizeof(StaticSemaphore_t),
               "Incorrect size of struct __lock");

/* FreeRTOS configuration check */
_Static_assert(configSUPPORT_STATIC_ALLOCATION,
               "FreeRTOS should be configured with static allocation support");

/* These 2 locks are used instead of 9 distinct newlib static locks,
 * as most of the locks are required for lesser-used features, so
 * the chance of performance degradation due to lock contention is low.
 */
static StaticSemaphore_t s_common_mutex;
static StaticSemaphore_t s_common_recursive_mutex;

#if ESP_ROM_HAS_RETARGETABLE_LOCKING
/* C3 and S3 ROMs are built without Newlib static lock symbols exported, and
 * with an extra level of _LOCK_T indirection in mind.
 * The following is a workaround for this:
 * - on startup, we call esp_rom_newlib_init_common_mutexes to set
 *   the two mutex pointers to magic values.
 * - if in __retarget_lock_acquire*, we check if the argument dereferences
 *   to the magic value. If yes, we lock the correct mutex defined in the app,
 *   instead.
 * Casts from &StaticSemaphore_t to _LOCK_T are okay because _LOCK_T
 * (which is SemaphoreHandle_t) is a pointer to the corresponding
 * StaticSemaphore_t structure. This is ensured by asserts below.
 */

#define ROM_NEEDS_MUTEX_OVERRIDE
#endif // ESP_ROM_HAS_RETARGETABLE_LOCKING

#ifdef ROM_NEEDS_MUTEX_OVERRIDE
#define ROM_MUTEX_MAGIC  0xbb10c433
/* This is a macro, since we are overwriting the argument  */
#define MAYBE_OVERRIDE_LOCK(_lock, _lock_to_use_instead) \
    if (*(int*)_lock == ROM_MUTEX_MAGIC) { \
        (_lock) = (_LOCK_T) (_lock_to_use_instead); \
    }
#else  // ROM_NEEDS_MUTEX_OVERRIDE
#define MAYBE_OVERRIDE_LOCK(_lock, _lock_to_use_instead)
#endif // ROM_NEEDS_MUTEX_OVERRIDE

void IRAM_ATTR __retarget_lock_init(_LOCK_T *lock)
{
    *lock = NULL;  /* In case lock's memory is uninitialized */
    lock_init_generic(lock, queueQUEUE_TYPE_MUTEX);
}

void IRAM_ATTR __retarget_lock_init_recursive(_LOCK_T *lock)
{
    *lock = NULL;  /* In case lock's memory is uninitialized */
    lock_init_generic(lock, queueQUEUE_TYPE_RECURSIVE_MUTEX);
}

void IRAM_ATTR __retarget_lock_close(_LOCK_T lock)
{
    _lock_close(&lock);
}

void IRAM_ATTR __retarget_lock_close_recursive(_LOCK_T lock)
{
    _lock_close_recursive(&lock);
}

/* Separate function, to prevent generating multiple assert strings */
static void IRAM_ATTR check_lock_nonzero(_LOCK_T lock)
{
    assert(lock != NULL && "Uninitialized lock used");
}

void IRAM_ATTR __retarget_lock_acquire(_LOCK_T lock)
{
    check_lock_nonzero(lock);
    MAYBE_OVERRIDE_LOCK(lock, &s_common_mutex);
    _lock_acquire(&lock);
}

void IRAM_ATTR __retarget_lock_acquire_recursive(_LOCK_T lock)
{
    check_lock_nonzero(lock);
    MAYBE_OVERRIDE_LOCK(lock, &s_common_recursive_mutex);
    _lock_acquire_recursive(&lock);
}

int IRAM_ATTR __retarget_lock_try_acquire(_LOCK_T lock)
{
    check_lock_nonzero(lock);
    MAYBE_OVERRIDE_LOCK(lock, &s_common_mutex);
    return _lock_try_acquire(&lock);
}

int IRAM_ATTR __retarget_lock_try_acquire_recursive(_LOCK_T lock)
{
    check_lock_nonzero(lock);
    MAYBE_OVERRIDE_LOCK(lock, &s_common_recursive_mutex);
    return _lock_try_acquire_recursive(&lock);
}

void IRAM_ATTR __retarget_lock_release(_LOCK_T lock)
{
    check_lock_nonzero(lock);
    _lock_release(&lock);
}

void IRAM_ATTR __retarget_lock_release_recursive(_LOCK_T lock)
{
    check_lock_nonzero(lock);
    _lock_release_recursive(&lock);
}

/* When _RETARGETABLE_LOCKING is enabled, newlib expects the following locks to be provided: */

extern StaticSemaphore_t __attribute__((alias("s_common_recursive_mutex"))) __lock___sinit_recursive_mutex;
extern StaticSemaphore_t __attribute__((alias("s_common_recursive_mutex"))) __lock___malloc_recursive_mutex;
extern StaticSemaphore_t __attribute__((alias("s_common_recursive_mutex"))) __lock___env_recursive_mutex;
extern StaticSemaphore_t __attribute__((alias("s_common_recursive_mutex"))) __lock___sfp_recursive_mutex;
extern StaticSemaphore_t __attribute__((alias("s_common_recursive_mutex"))) __lock___atexit_recursive_mutex;
extern StaticSemaphore_t __attribute__((alias("s_common_mutex"))) __lock___at_quick_exit_mutex;
extern StaticSemaphore_t __attribute__((alias("s_common_mutex"))) __lock___tz_mutex;
extern StaticSemaphore_t __attribute__((alias("s_common_mutex"))) __lock___dd_hash_mutex;
extern StaticSemaphore_t __attribute__((alias("s_common_mutex"))) __lock___arc4random_mutex;

void esp_newlib_locks_init(void)
{
    /* Initialize the two mutexes used for the locks above.
     * Asserts below check our assumption that SemaphoreHandle_t will always
     * point to the corresponding StaticSemaphore_t structure.
     */
    SemaphoreHandle_t handle;
    handle = xSemaphoreCreateMutexStatic(&s_common_mutex);
    assert(handle == (SemaphoreHandle_t) &s_common_mutex);
    handle = xSemaphoreCreateRecursiveMutexStatic(&s_common_recursive_mutex);
    assert(handle == (SemaphoreHandle_t) &s_common_recursive_mutex);
    (void) handle;

    /* Chip ROMs are built with older versions of newlib, and rely on different lock variables.
     * Initialize these locks to the same values.
     */
#ifdef CONFIG_IDF_TARGET_ESP32
    /* Newlib 2.2.0 is used in ROM, the following lock symbols are defined: */
    extern _lock_t __sfp_lock;
    __sfp_lock = (_lock_t) &s_common_recursive_mutex;
    extern _lock_t __sinit_lock;
    __sinit_lock = (_lock_t) &s_common_recursive_mutex;
    extern _lock_t __env_lock_object;
    __env_lock_object = (_lock_t) &s_common_recursive_mutex;
    extern _lock_t __tz_lock_object;
    __tz_lock_object = (_lock_t) &s_common_mutex;
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
    /* Newlib 3.0.0 is used in ROM, the following lock symbols are defined: */
    extern _lock_t __sinit_recursive_mutex;
    __sinit_recursive_mutex = (_lock_t) &s_common_recursive_mutex;
    extern _lock_t __sfp_recursive_mutex;
    __sfp_recursive_mutex = (_lock_t) &s_common_recursive_mutex;
#elif ESP_ROM_HAS_RETARGETABLE_LOCKING
    /* Newlib 3.3.0 is used in ROM, built with _RETARGETABLE_LOCKING.
     * No access to lock variables for the purpose of ECO forward compatibility,
     * however we have an API to initialize lock variables used in the ROM.
     */
    extern void esp_rom_newlib_init_common_mutexes(_LOCK_T, _LOCK_T);
    /* See notes about ROM_NEEDS_MUTEX_OVERRIDE above */
    int magic_val = ROM_MUTEX_MAGIC;
    _LOCK_T magic_mutex = (_LOCK_T) &magic_val;
    esp_rom_newlib_init_common_mutexes(magic_mutex, magic_mutex);
#else // other target
#error Unsupported target
#endif
}
