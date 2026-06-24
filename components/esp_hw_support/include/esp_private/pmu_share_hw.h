/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Acquire PMU lock
 *
 * @note If any of the locks are taken, this API will wait until the lock is successfully acquired.
 */
void pmu_lock_acquire(void);

/**
 * @brief Release PMU lock
 */
void pmu_lock_release(void);

#ifdef __cplusplus
}
#endif
