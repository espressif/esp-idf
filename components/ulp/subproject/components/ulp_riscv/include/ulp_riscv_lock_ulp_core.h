/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "ulp_riscv_lock_shared.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Locks are based on the Peterson's algorithm, https://en.wikipedia.org/wiki/Peterson%27s_algorithm
 *
 */

/**
 * @brief Acquire the lock, preventing the main CPU from taking until released. Spins until lock is acquired.
 *
 * @note  The lock is only designed for being used by a single thread on the ULP,
 *        it is not safe to try to acquire it from multiple threads.
 *
 * @param lock Pointer to lock struct, shared with main CPU
 */
void ulp_riscv_lock_acquire(ulp_riscv_lock_t *lock);

/**
 * @brief Release the lock
 *
 * @param lock Pointer to lock struct, shared with main CPU
 */
void ulp_riscv_lock_release(ulp_riscv_lock_t *lock);

#ifdef __cplusplus
}
#endif
