/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * This file provides only very simple stubs to build IDF-based FreeRTOSes which use spinlocks on Linux.
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SPINLOCK_FREE          0xB33FFFFF
#define SPINLOCK_WAIT_FOREVER  (-1)
#define SPINLOCK_NO_WAIT        0
#define SPINLOCK_INITIALIZER   {.owner = SPINLOCK_FREE,.count = 0}
#define CORE_ID_REGVAL_XOR_SWAP (0xCDCD ^ 0xABAB)

/**
 * @brief Spinlock object
 * Owner:
 *  - Set to 0 if uninitialized
 *  - Set to portMUX_FREE_VAL when free
 *  - Set to CORE_ID_REGVAL_PRO or CORE_ID_REGVAL_AP when locked
 *  - Any other value indicates corruption
 * Count:
 *  - 0 if unlocked
 *  - Recursive count if locked
 *
 * @note The simulator is single-core, hence, it doesn't have a proper spinlock implementation.
 * @note Keep portMUX_INITIALIZER_UNLOCKED in sync with this struct
 */
typedef struct {
    uint32_t owner;
    uint32_t count;
}spinlock_t;

static inline void __attribute__((always_inline)) spinlock_initialize(spinlock_t *lock)
{
}

static inline bool __attribute__((always_inline)) spinlock_acquire(spinlock_t *lock, int32_t timeout)
{
    return true;
}

static inline void __attribute__((always_inline)) spinlock_release(spinlock_t *lock)
{
}

#ifdef __cplusplus
}
#endif
