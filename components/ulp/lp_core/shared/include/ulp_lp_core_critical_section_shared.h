/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LOCK_CANDIDATE_LP_CORE = 0,
    LOCK_CANDIDATE_HP_CORE_0,
#if CONFIG_FREERTOS_NUMBER_OF_CORES > 1
    LOCK_CANDIDATE_HP_CORE_1,
#endif
    LOCK_CANDIDATE_NUM_MAX,
} ulp_lp_core_spinlock_candidate_t;

typedef struct {
    volatile int level[LOCK_CANDIDATE_NUM_MAX];
    volatile int last_to_enter[LOCK_CANDIDATE_NUM_MAX - 1];
    volatile unsigned int prev_int_level;
} ulp_lp_core_spinlock_t;

/**
 * @brief Initialize the spinlock that protects shared resources between main CPU and LP CPU.
 *
 * @note The spinlock can be initialized in either main program or LP program.
 *
 * @param lock Pointer to lock struct
 */
void ulp_lp_core_spinlock_init(ulp_lp_core_spinlock_t *lock);

/**
 * @brief Enter the critical section that protects shared resources between main CPU and LP CPU.
 *
 * @note  This critical section is designed for being used by multiple threads, it is safe to try to enter it
 *        simultaneously from multiple threads on multiple main CPU cores and LP CPU.
 *
 * @note This critical section does not support nesting entering and exiting.
 *
 * @param lock Pointer to lock struct
 */
void ulp_lp_core_enter_critical(ulp_lp_core_spinlock_t *lock);

/**
 * @brief Exit the critical section that protect shared resource between main CPU and LP CPU.
 *
 * @note This critical section does not support nesting entering and exiting.
 *
 * @param lock Pointer to lock struct
 */
void ulp_lp_core_exit_critical(ulp_lp_core_spinlock_t *lock);

#ifdef __cplusplus
}
#endif
