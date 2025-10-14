/*
 * SPDX-FileCopyrightText: 2017-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * @file esp_private/esp_timer_private.h
 *
 * @brief Interface between common and platform-specific parts of esp_timer.
 *
 * The functions in this header file are implemented for each supported SoC.
 * High level functions defined in esp_timer.c call the functions here to
 * interact with the hardware.
 *
 * Note: The functions from this file are marked as private and are used exclusively
 * inside the IDF in the power management and sleep files.
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Set esp_timer time to a certain value
 *
 * Called from light sleep code to synchronize esp_timer time with RTC time.
 *
 * @param new_us  the value to be set to esp_timer time, in microseconds
 */
void esp_timer_private_set(uint64_t new_us);

/**
 * @brief Adjust current esp_timer time by a certain value
 *
 * @param time_diff_us  adjustment to apply to esp_timer time, in microseconds
 */
void esp_timer_private_advance(int64_t time_diff_us);

/**
 * @brief obtain internal critical section used in the esp_timer implementation
 * This can be used when a sequence of calls to esp_timer has to be made,
 * and it is necessary that the state of the timer is consistent between
 * the calls. Should be treated in the same way as a spinlock.
 * Call esp_timer_private_unlock to release the lock
 */
void esp_timer_private_lock(void);

/**
 * @brief counterpart of esp_timer_lock
 */
void esp_timer_private_unlock(void);

#ifdef __cplusplus
}
#endif
