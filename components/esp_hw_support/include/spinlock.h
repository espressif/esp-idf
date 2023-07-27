/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"
#include <stdint.h>
#include <stdbool.h>
#include "esp_cpu.h"

#if __XTENSA__
#include "xtensa/xtruntime.h"
#include "xt_utils.h"
#else
#include "riscv/rv_utils.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_SPIRAM_WORKAROUND_NEED_VOLATILE_SPINLOCK
#define NEED_VOLATILE_MUX volatile
#else
#define NEED_VOLATILE_MUX
#endif

#define SPINLOCK_FREE          0xB33FFFFF
#define SPINLOCK_WAIT_FOREVER  (-1)
#define SPINLOCK_NO_WAIT        0
#define SPINLOCK_INITIALIZER   {.owner = SPINLOCK_FREE,.count = 0}

#define SPINLOCK_OWNER_ID_0 0xCDCD /* Use these values to avoid 0 being a valid lock owner, same as CORE_ID_REGVAL_PRO on Xtensa */
#define SPINLOCK_OWNER_ID_1 0xABAB /* Same as CORE_ID_REGVAL_APP on Xtensa*/

#define CORE_ID_REGVAL_XOR_SWAP (0xCDCD ^ 0xABAB)
#define SPINLOCK_OWNER_ID_XOR_SWAP CORE_ID_REGVAL_XOR_SWAP

typedef struct {
    NEED_VOLATILE_MUX uint32_t owner;
    NEED_VOLATILE_MUX uint32_t count;
} spinlock_t;

/**
 * @brief Initialize a lock to its default state - unlocked
 * @param lock - spinlock object to initialize
 */
static inline void __attribute__((always_inline)) spinlock_initialize(spinlock_t *lock)
{
    assert(lock);
#if !CONFIG_FREERTOS_UNICORE
    lock->owner = SPINLOCK_FREE;
    lock->count = 0;
#endif
}

/**
 * @brief Top level spinlock acquire function, spins until get the lock
 *
 * This function will:
 * - Save current interrupt state, then disable interrupts
 * - Spin until lock is acquired or until timeout occurs
 * - Restore interrupt state
 *
 * @note Spinlocks alone do no constitute true critical sections (as this
 *       function reenables interrupts once the spinlock is acquired). For critical
 *       sections, use the interface provided by the operating system.
 * @param lock - target spinlock object
 * @param timeout - cycles to wait, passing SPINLOCK_WAIT_FOREVER blocs indefinitely
 */
static inline bool __attribute__((always_inline)) spinlock_acquire(spinlock_t *lock, int32_t timeout)
{
#if !CONFIG_FREERTOS_UNICORE && !BOOTLOADER_BUILD
    uint32_t irq_status;
    uint32_t core_owner_id, other_core_owner_id;
    bool lock_set;
    esp_cpu_cycle_count_t start_count;

    assert(lock);
#if __XTENSA__
    irq_status = XTOS_SET_INTLEVEL(XCHAL_EXCM_LEVEL);

    // Note: The core IDs are the full 32 bit (CORE_ID_REGVAL_PRO/CORE_ID_REGVAL_APP) values
    core_owner_id = xt_utils_get_raw_core_id();
#else  //__riscv

    irq_status = rv_utils_mask_int_level_lower_than(RVHAL_EXCM_LEVEL);
    core_owner_id = rv_utils_get_core_id() == 0 ? SPINLOCK_OWNER_ID_0 : SPINLOCK_OWNER_ID_1;
#endif
    other_core_owner_id = CORE_ID_REGVAL_XOR_SWAP ^ core_owner_id;

    /* lock->owner should be one of SPINLOCK_FREE, CORE_ID_REGVAL_PRO,
     * CORE_ID_REGVAL_APP:
     *  - If SPINLOCK_FREE, we want to atomically set to 'core_owner_id'.
     *  - If "our" core_owner_id, we can drop through immediately.
     *  - If "other_core_owner_id", we spin here.
     */

    // The caller is already the owner of the lock. Simply increment the nesting count
    if (lock->owner == core_owner_id) {
        assert(lock->count > 0 && lock->count < 0xFF);    // Bad count value implies memory corruption
        lock->count++;
#if __XTENSA__
        XTOS_RESTORE_INTLEVEL(irq_status);
#else
        rv_utils_restore_intlevel(irq_status);
#endif
        return true;
    }

    /* First attempt to take the lock.
     *
     * Note: We do a first attempt separately (instead of putting this into a loop) in order to avoid call to
     * esp_cpu_get_cycle_count(). This doing a first attempt separately makes acquiring a free lock quicker, which
     * is the case for the majority of spinlock_acquire() calls (as spinlocks are free most of the time since they
     * aren't meant to be held for long).
     */
    lock_set = esp_cpu_compare_and_set(&lock->owner, SPINLOCK_FREE, core_owner_id);
    if (lock_set || timeout == SPINLOCK_NO_WAIT) {
        // We've successfully taken the lock, or we are not retrying
        goto exit;
    }

    // First attempt to take the lock has failed. Retry until the lock is taken, or until we timeout.
    start_count = esp_cpu_get_cycle_count();
    do {
        lock_set = esp_cpu_compare_and_set(&lock->owner, SPINLOCK_FREE, core_owner_id);
        if (lock_set) {
            break;
        }
        // Keep looping if we are waiting forever, or check if we have timed out
    } while ((timeout == SPINLOCK_WAIT_FOREVER) || (esp_cpu_get_cycle_count() - start_count) <= timeout);

exit:
    if (lock_set) {
        assert(lock->owner == core_owner_id);
        assert(lock->count == 0);   // This is the first time the lock is set, so count should still be 0
        lock->count++;  // Finally, we increment the lock count
    } else {    // We timed out waiting for lock
        assert(lock->owner == SPINLOCK_FREE || lock->owner == other_core_owner_id);
        assert(lock->count < 0xFF); // Bad count value implies memory corruption
    }

#if __XTENSA__
    XTOS_RESTORE_INTLEVEL(irq_status);
#else
    rv_utils_restore_intlevel(irq_status);
#endif
    return lock_set;

#else  // !CONFIG_FREERTOS_UNICORE
    return true;
#endif
}

/**
 * @brief Top level spinlock unlock function, unlocks a previously locked spinlock
 *
 * This function will:
 * - Save current interrupt state, then disable interrupts
 * - Release the spinlock
 * - Restore interrupt state
 *
 * @note Spinlocks alone do no constitute true critical sections (as this
 *       function reenables interrupts once the spinlock is acquired). For critical
 *       sections, use the interface provided by the operating system.
 * @param lock - target, locked before, spinlock object
 */
static inline void __attribute__((always_inline)) spinlock_release(spinlock_t *lock)
{
#if !CONFIG_FREERTOS_UNICORE && !BOOTLOADER_BUILD
    uint32_t irq_status;
    uint32_t core_owner_id;

    assert(lock);
#if __XTENSA__
    irq_status = XTOS_SET_INTLEVEL(XCHAL_EXCM_LEVEL);

    core_owner_id = xt_utils_get_raw_core_id();
#else
    irq_status = rv_utils_mask_int_level_lower_than(RVHAL_EXCM_LEVEL);
    core_owner_id = rv_utils_get_core_id() == 0 ? SPINLOCK_OWNER_ID_0 : SPINLOCK_OWNER_ID_1;
#endif
    assert(core_owner_id == lock->owner); // This is a lock that we didn't acquire, or the lock is corrupt
    lock->count--;

    if (!lock->count) { // If this is the last recursive release of the lock, mark the lock as free
        lock->owner = SPINLOCK_FREE;
    } else {
        assert(lock->count < 0x100); // Indicates memory corruption
    }

#if __XTENSA__
    XTOS_RESTORE_INTLEVEL(irq_status);
#else
    rv_utils_restore_intlevel(irq_status);
#endif  //#if __XTENSA__
#endif  //#if !CONFIG_FREERTOS_UNICORE && !BOOTLOADER_BUILD
}

#ifdef __cplusplus
}
#endif
