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
 * @file esp_timer_impl.h
 *
 * @brief Interface between common and platform-specific parts of esp_timer.
 *
 * The functions in this header file are implemented for each supported SoC.
 * High level functions defined in esp_timer.c call the functions here to
 * interact with the hardware.
 */

#include <stdint.h>
#include "esp_err.h"
#include "esp_intr_alloc.h"

/**
 * @brief Initialize platform specific layer of esp_timer
 * @param alarm_handler function to call on timer interrupt
 * @return ESP_OK, ESP_ERR_NO_MEM, or one of the errors from interrupt allocator
 */
esp_err_t esp_timer_impl_init(intr_handler_t alarm_handler);

/**
 * @brief Deinitialize platform specific layer of esp_timer
 */
void esp_timer_impl_deinit();

/**
 * @brief Set up the timer interrupt to fire at a particular time
 *
 * If the alarm time is too close in the future, implementation should set the
 * alarm to the earliest time possible.
 *
 * @param timestamp time in microseconds when interrupt should fire (relative to
 *                  boot time, i.e. as returned by esp_timer_impl_get_time)
 */
void esp_timer_impl_set_alarm(uint64_t timestamp);

/**
 * @brief Notify esp_timer implementation that APB frequency has changed
 *
 * Called by the frequency switching code.
 *
 * @param apb_ticks_per_us new number of APB clock ticks per microsecond
 */
void esp_timer_impl_update_apb_freq(uint32_t apb_ticks_per_us);

/**
 * @brief Get time, in microseconds, since esp_timer_impl_init was called
 * @return timestamp in microseconds
 */
uint64_t esp_timer_impl_get_time();

/**
 * @brief Get minimal timer period, in microseconds
 * Periods shorter than the one returned may not be possible to achieve due to
 * interrupt latency and context switch time. Short period of periodic timer may
 * cause the system to spend all the time servicing the interrupt and timer
 * callback, preventing other tasks from running.
 * @return minimal period of periodic timer, in microseconds
 */
uint64_t esp_timer_impl_get_min_period_us();
