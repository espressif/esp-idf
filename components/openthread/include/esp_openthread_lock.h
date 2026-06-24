/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>

#include "esp_err.h"
#include "freertos/FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This function initializes the OpenThread API lock.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NO_MEM if allocation has failed
 *      - ESP_ERR_INVALID_STATE if already initialized
 *
 */
esp_err_t esp_openthread_lock_init(void);

/**
 * This function deinitializes the OpenThread API lock.
 *
 */
void esp_openthread_lock_deinit(void);

/**
 * @brief This function acquires the OpenThread API lock.
 *
 * @note Every Openthread APIs that takes an otInstance argument MUST be protected with this API lock
 *       except that the call site is in Openthread callbacks.
 *
 * @param[in] block_ticks   The maximum number of RTOS ticks to wait for the lock.
 *
 * @return
 *      - True on lock acquired
 *      - False on failing to acquire the lock with the timeout.
 *
 */
bool esp_openthread_lock_acquire(TickType_t block_ticks);

/**
 * @brief This function releases the OpenThread API lock.
 *
 */
void esp_openthread_lock_release(void);

/**
 * @brief This function acquires the OpenThread API task switching lock.
 *
 * @note In OpenThread API context, it waits for some actions to be done in other tasks (like lwip),
 *       after task switching, it needs to call OpenThread API again. Normally it's not allowed,
 *       since the previous OpenThread API lock is not released yet. This task_switching lock allows
 *       the OpenThread API can be called in this case.
 *
 * @note Please use esp_openthread_lock_acquire() for normal cases.
 *
 * @param[in] block_ticks   The maximum number of RTOS ticks to wait for the lock.
 *
 * @return
 *      - True on lock acquired
 *      - False on failing to acquire the lock with the timeout.
 *
 */
bool esp_openthread_task_switching_lock_acquire(TickType_t block_ticks);

/**
 * @brief This function releases the OpenThread API task switching lock.
 *
 * @note This API must be called after `esp_openthread_task_switching_lock_acquire` or
 *       `esp_openthread_lock_acquire` and will cause a crash if the current task is not the task switching lock holder.
 *       This error could be caused by calling OpenThread APIs without locking OpenThread stack.
 */
void esp_openthread_task_switching_lock_release(void);

#ifdef __cplusplus
}
#endif
