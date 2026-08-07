/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"
#include <stdint.h>
#include <stdbool.h>
#include "esp_cpu.h"
#include "soc/soc_caps.h"

#if __XTENSA__
#include "xtensa/xtruntime.h"
#include "xt_utils.h"
#elif __riscv
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
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    lock->owner = SPINLOCK_FREE;
    lock->count = 0;
#endif
}

/**
 * @brief Get the spinlock owner id of the executing core
 *
 * @return owner id of the current core
 */
static inline uint32_t __attribute__((always_inline)) spinlock_owner_id(void)
{
#if SOC_CPU_CORES_NUM > 1
#if __XTENSA__
    // On Xtensa the raw PRID register value is used directly as the owner id
    // (the full 32 bit CORE_ID_REGVAL_PRO/CORE_ID_REGVAL_APP values).
    return xt_utils_get_raw_core_id();
#else //__riscv
    return rv_utils_get_core_id() == 0 ? SPINLOCK_OWNER_ID_0 : SPINLOCK_OWNER_ID_1;
#endif
#else
    return 0;
#endif
}

/**
 * @brief Get the spinlock owner id for a given core index
 *
 * Lets a caller that already knows the executing core's index derive the owner id without reading the core id register.
 *
 * @param core_id - core index (0 or 1)
 * @return owner id for that core
 */
static inline uint32_t __attribute__((always_inline)) spinlock_owner_id_for_core(uint32_t core_id)
{
    return core_id ? SPINLOCK_OWNER_ID_1 : SPINLOCK_OWNER_ID_0;
}

/**
 * @brief Get the core index of a given spinlock owner id
 *
 * Inverse of spinlock_owner_id_for_core(). Lets a caller that already has the owner id (e.g. from spinlock_owner_id())
 * derive the core index without reading the core id register again.
 *
 * @param owner_id - a spinlock owner id
 * @return core index (0 or 1) that owner id belongs to
 */
static inline uint32_t __attribute__((always_inline)) spinlock_core_id_from_owner_id(uint32_t owner_id)
{
#if __XTENSA__
    // On Xtensa the owner id is the raw PRID register value.
    return xt_utils_get_core_id_from_raw(owner_id);
#else //__riscv
    return owner_id == SPINLOCK_OWNER_ID_0 ? 0 : 1;
#endif
}

/**
 * @brief Disable interrupts on the current core and return the previous interrupt state
 *
 * Disables up to the level used to protect spinlocks. Spinlocks are compiled out to no-ops on single-core and
 * bootloader builds.
 */
static inline uint32_t __attribute__((always_inline)) spinlock_int_disable(void)
{
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE && !BOOTLOADER_BUILD
#if __XTENSA__
    return XTOS_SET_INTLEVEL(XCHAL_EXCM_LEVEL);
#elif SOC_INT_CLIC_SUPPORTED
    return rv_utils_set_intlevel_regval(RVHAL_EXCM_LEVEL_CLIC);
#else
    return rv_utils_set_intlevel_regval(RVHAL_EXCM_LEVEL);
#endif
#else
    return 0;
#endif
}

/**
 * @brief Restore interrupts to a state previously returned by spinlock_int_disable()
 */
static inline void __attribute__((always_inline)) spinlock_int_restore(uint32_t int_state)
{
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE && !BOOTLOADER_BUILD
#if __XTENSA__
    XTOS_RESTORE_INTLEVEL(int_state);
#else //__riscv
    rv_utils_restore_intlevel_regval(int_state);
#endif
#else
    (void)int_state;
#endif
}

/**
 * @brief Acquire a spinlock without managing interrupts
 *
 * Performs the lock acquisition (owner tracking + atomic compare-and-set spin) but, unlike spinlock_acquire(), does
 * NOT save/disable/restore interrupts and takes a caller-supplied owner id instead of reading the core id register.
 *
 * @note The caller MUST disable interrupts before calling and keep them disabled until the matching
 *       spinlock_release_impl(). The owner id must be that of the executing core
 *       (obtain it via spinlock_owner_id() or spinlock_owner_id_for_core()).
 *
 * @param lock - target spinlock object
 * @param timeout - cycles to wait, passing SPINLOCK_WAIT_FOREVER blocks indefinitely
 * @param core_owner_id - owner id of the executing core
 * @return true if the lock was acquired, false on timeout
 */
static inline bool __attribute__((always_inline)) spinlock_acquire_impl(spinlock_t *lock, int32_t timeout, uint32_t core_owner_id)
{
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE && !BOOTLOADER_BUILD
    // Unused if asserts are disabled
    uint32_t __attribute__((unused)) other_core_owner_id;
    bool lock_set;
    esp_cpu_cycle_count_t start_count;

    assert(lock);
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
    } while ((timeout == SPINLOCK_WAIT_FOREVER) || (esp_cpu_get_cycle_count() - start_count) <= (esp_cpu_cycle_count_t)timeout);

exit:
    if (lock_set) {
        assert(lock->owner == core_owner_id);
        assert(lock->count == 0);   // This is the first time the lock is set, so count should still be 0
        lock->count++;  // Finally, we increment the lock count
    } else {    // We timed out waiting for lock
        assert(lock->owner == SPINLOCK_FREE || lock->owner == other_core_owner_id);
        assert(lock->count < 0xFF); // Bad count value implies memory corruption
    }

    return lock_set;

#else  // !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    (void)lock;
    (void)timeout;
    (void)core_owner_id;
    return true;
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
 * @param timeout - cycles to wait, passing SPINLOCK_WAIT_FOREVER blocks indefinitely
 */
static inline bool __attribute__((always_inline)) spinlock_acquire(spinlock_t *lock, int32_t timeout)
{
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE && !BOOTLOADER_BUILD
    uint32_t irq_status = spinlock_int_disable();
    bool lock_set = spinlock_acquire_impl(lock, timeout, spinlock_owner_id());
    spinlock_int_restore(irq_status);
    return lock_set;
#else
    (void)lock;
    (void)timeout;
    return true;
#endif
}

/**
 * @brief Release a spinlock without managing interrupts
 *
 * Unlike spinlock_release(), does NOT save/disable/restore interrupts and takes a caller-supplied owner id (only used
 * by the debug assert that validates lock ownership).
 *
 * @note The caller MUST have interrupts disabled (matching the preceding spinlock_acquire_impl()).
 *
 * @param lock - target, locked before, spinlock object
 * @param core_owner_id - owner id of the executing core (only used for the debug assert)
 */
static inline void __attribute__((always_inline)) spinlock_release_impl(spinlock_t *lock, uint32_t core_owner_id)
{
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE && !BOOTLOADER_BUILD
    assert(lock);
    assert(core_owner_id == lock->owner); // This is a lock that we didn't acquire, or the lock is corrupt
    (void)core_owner_id;
    lock->count--;

    if (!lock->count) { // If this is the last recursive release of the lock, mark the lock as free
        lock->owner = SPINLOCK_FREE;
    } else {
        assert(lock->count < 0x100); // Indicates memory corruption
    }
#else
    (void)lock;
    (void)core_owner_id;
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
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE && !BOOTLOADER_BUILD
    uint32_t irq_status = spinlock_int_disable();
    spinlock_release_impl(lock, spinlock_owner_id());
    spinlock_int_restore(irq_status);
#else
    (void)lock;
#endif
}

#ifdef __cplusplus
}
#endif
