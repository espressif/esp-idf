/*
 * SPDX-FileCopyrightText: 2017-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * @file private_include/esp_timer_impl.h
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
 * @brief Minimal initialization of platform specific layer of esp_timer
 * This function can be called very early in startup process, after this call
 * only esp_timer_get_time function can be used.
 * esp_timer_impl_init has to be called after this function to initialize the
 * rest of esp_timer implementation.
 * @return ESP_OK
 */
esp_err_t esp_timer_impl_early_init(void);

/**
 * @brief Initialize platform specific layer of esp_timer
 * @param alarm_handler function to call on timer interrupt
 * Before calling this function, esp_timer_impl_early_init must be called.
 * @return ESP_OK, ESP_ERR_NO_MEM, or one of the errors from interrupt allocator
 */
esp_err_t esp_timer_impl_init(intr_handler_t alarm_handler);

/**
 * @brief Deinitialize platform specific layer of esp_timer
 */
void esp_timer_impl_deinit(void);

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
 * @brief Set up the timer interrupt to fire at a particular time for a particular alarm module.
 *
 * If the alarm time is too close in the future, implementation should set the
 * alarm to the earliest time possible.
 *
 * @param timestamp time in microseconds when interrupt should fire (relative to
 *                  boot time, i.e. as returned by esp_timer_impl_get_time)
 *
 * @param alarm_id Id alarm:
 *                 0 - alarm_0 for the ESP_TIMER_TASK dispatch method,
 *                 1 - alarm_1 for the ESP_TIMER_ISR dispatch method.
 */
void esp_timer_impl_set_alarm_id(uint64_t timestamp, unsigned alarm_id);

/**
 * @brief Notify esp_timer implementation that APB frequency has changed
 *
 * Called by the frequency switching code.
 *
 * @param apb_ticks_per_us new number of APB clock ticks per microsecond
 */
void esp_timer_impl_update_apb_freq(uint32_t apb_ticks_per_us);

/**
 * @brief Adjust current esp_timer time by a certain value
 *
 * Called from light sleep code to synchronize esp_timer time with RTC time.
 *
 * @param time_us  adjustment to apply to esp_timer time, in microseconds
 */
void esp_timer_impl_advance(int64_t time_us);

/**
 * @brief Get time, in microseconds, since esp_timer_impl_init was called
 * @return timestamp in microseconds
 */
int64_t esp_timer_impl_get_time(void);

/**
 * @brief Get minimal timer period, in microseconds
 * Periods shorter than the one returned may not be possible to achieve due to
 * interrupt latency and context switch time. Short period of periodic timer may
 * cause the system to spend all the time servicing the interrupt and timer
 * callback, preventing other tasks from running.
 * @return minimal period of periodic timer, in microseconds
 */
uint64_t esp_timer_impl_get_min_period_us(void);

/**
 * @brief obtain internal critical section used esp_timer implementation
 * This can be used when a sequence of calls to esp_timer has to be made,
 * and it is necessary that the state of the timer is consistent between
 * the calls. Should be treated in the same way as a spinlock.
 * Call esp_timer_impl_unlock to release the lock
 */
void esp_timer_impl_lock(void);

/**
 * @brief counterpart of esp_timer_impl_lock
 */
void esp_timer_impl_unlock(void);

/**
 * @brief Get counting register
 *
 * Bit depth dependents on implementation and can be 32-bit or 64-bit.
 *
 * @return the value of the counting register
 */
uint64_t esp_timer_impl_get_counter_reg(void);

/**
 * @brief Get alarm register
 *
 * Bit depth dependents on implementation and can be 32-bit or 64-bit.
 *
 * @return the value of the alarm register
 */
uint64_t esp_timer_impl_get_alarm_reg(void);

#if CONFIG_ESP_TIME_FUNCS_USE_ESP_TIMER
/**
 * @brief Initialize esp_timer as system time provider.
 */
void esp_timer_impl_init_system_time(void);
#endif

#if CONFIG_ESP_TIMER_SUPPORTS_ISR_DISPATCH_METHOD
/**
 * @brief Set the next alarm if there is such an alarm in the cached array.
 *
 * @note Available only when CONFIG_ESP_TIMER_SUPPORTS_ISR_DISPATCH_METHOD is enabled.
 */
void esp_timer_impl_try_to_set_next_alarm(void);
#endif
