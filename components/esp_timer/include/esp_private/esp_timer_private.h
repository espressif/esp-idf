// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
 * @brief Notify esp_timer implementation that APB frequency has changed
 *
 * Called by the frequency switching code.
 *
 * @param apb_ticks_per_us new number of APB clock ticks per microsecond
 */
void esp_timer_private_update_apb_freq(uint32_t apb_ticks_per_us);

/**
 * @brief Adjust current esp_timer time by a certain value
 *
 * Called from light sleep code to synchronize esp_timer time with RTC time.
 *
 * @param time_us  adjustment to apply to esp_timer time, in microseconds
 */
void esp_timer_private_advance(int64_t time_us);

/**
 * @brief obtain internal critical section used esp_timer implementation
 * This can be used when a sequence of calls to esp_timer has to be made,
 * and it is necessary that the state of the timer is consistent between
 * the calls. Should be treated in the same way as a spinlock.
 * Call esp_timer_unlock to release the lock
 */
void esp_timer_private_lock(void);

/**
 * @brief counterpart of esp_timer_lock
 */
void esp_timer_private_unlock(void);

#ifdef __cplusplus
}
#endif
