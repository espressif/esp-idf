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
 * @file esp_timer.h
 * @brief microsecond-precision 64-bit timer API, replacement for ets_timer
 *
 * Not a public header yet. To be moved into include/ directory when it is made
 * public.
 *
 * esp_timer APIs allow components to receive callbacks when a hardware timer
 * reaches certain value. The timer provides microsecond accuracy and
 * up to 64 bit range. Note that while the timer itself provides microsecond
 * accuracy, callbacks are dispatched from an auxiliary task. Some time is
 * needed to notify this task from timer ISR, and then to invoke the callback.
 * If more than one callback needs to be dispatched at any particular time,
 * each subsequent callback will be dispatched only when the previous callback
 * returns. Therefore, callbacks should not do much work; instead, they should
 * use RTOS notification mechanisms (queues, semaphores, event groups, etc.) to
 * pass information to other tasks.
 *
 * <to be implemented> It should be possible to request the callback to be called
 * directly from the ISR. This reduces the latency, but has potential impact on
 * all other callbacks which need to be dispatched. This option should only be
 * used for simple callback functions, which do not take longer than a few
 * microseconds to run. </to be implemented>
 *
 * Implementation note: on the ESP32, esp_timer APIs use the "legacy" FRC2
 * timer. Timer callbacks are called from a task running on the PRO CPU.
 */

#include <stdint.h>
#include <stdio.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opaque type representing a single esp_timer
 */
typedef struct esp_timer* esp_timer_handle_t;

/**
 * @brief Timer callback function type
 * @param arg pointer to opaque user-specific data
 */
typedef void (*esp_timer_cb_t)(void* arg);


/**
 * @brief Method for dispatching timer callback
 */
typedef enum {
    ESP_TIMER_TASK,     //!< Callback is called from timer task

    /* Not supported for now, provision to allow callbacks to run directly
     * from an ISR:

        ESP_TIMER_ISR,      //!< Callback is called from timer ISR

     */
} esp_timer_dispatch_t;

/**
 * @brief Timer configuration passed to esp_timer_create
 */
typedef struct {
    esp_timer_cb_t callback;        //!< Function to call when timer expires
    void* arg;                      //!< Argument to pass to the callback
    esp_timer_dispatch_t dispatch_method;   //!< Call the callback from task or from ISR
    const char* name;               //!< Timer name, used in esp_timer_dump function
} esp_timer_create_args_t;

/**
 * @brief Initialize esp_timer library
 *
 * @note This function is called from startup code. Applications do not need
 * to call this function before using other esp_timer APIs.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NO_MEM if allocation has failed
 *      - ESP_ERR_INVALID_STATE if already initialized
 *      - other errors from interrupt allocator
 */
esp_err_t esp_timer_init();

/**
 * @brief De-initialize esp_timer library
 *
 * @note Normally this function should not be called from applications
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if not yet initialized
 */
esp_err_t esp_timer_deinit();

/**
 * @brief Create an esp_timer instance
 *
 * @note When done using the timer, delete it with esp_timer_delete function.
 *
 * @param create_args   Pointer to a structure with timer creation arguments.
 *                      Not saved by the library, can be allocated on the stack.
 * @param[out] out_handle  Output, pointer to esp_timer_handle_t variable which
 *                         will hold the created timer handle.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if some of the create_args are not valid
 *      - ESP_ERR_INVALID_STATE if esp_timer library is not initialized yet
 *      - ESP_ERR_NO_MEM if memory allocation fails
 */
esp_err_t esp_timer_create(const esp_timer_create_args_t* create_args,
                           esp_timer_handle_t* out_handle);

/**
 * @brief Start one-shot timer
 *
 * Timer should not be running when this function is called.
 *
 * @param timer timer handle created using esp_timer_create
 * @param timeout_us timer timeout, in microseconds relative to the current moment
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the handle is invalid
 *      - ESP_ERR_INVALID_STATE if the timer is already running
 */
esp_err_t esp_timer_start_once(esp_timer_handle_t timer, uint64_t timeout_us);

/**
 * @brief Start a periodic timer
 *
 * Timer should not be running when this function is called. This function will
 * start the timer which will trigger every 'period' microseconds.
 *
 * @param timer timer handle created using esp_timer_create
 * @param period timer period, in microseconds
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the handle is invalid
 *      - ESP_ERR_INVALID_STATE if the timer is already running
 */
esp_err_t esp_timer_start_periodic(esp_timer_handle_t timer, uint64_t period);

/**
 * @brief Stop the timer
 *
 * This function stops the timer previously started using esp_timer_start_once
 * or esp_timer_start_periodic.
 *
 * @param timer timer handle created using esp_timer_create
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if the timer is not running
 */
esp_err_t esp_timer_stop(esp_timer_handle_t timer);

/**
 * @brief Delete an esp_timer instance
 *
 * The timer must be stopped before deleting. A one-shot timer which has expired
 * does not need to be stopped.
 *
 * @param timer timer handle allocated using esp_timer_create
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if the timer is not running
 */
esp_err_t esp_timer_delete(esp_timer_handle_t timer);

/**
 * @brief Get time in microseconds since boot
 * @return number of microseconds since esp_timer_init was called (this normally
 *          happens early during application startup).
 */
int64_t esp_timer_get_time();

/**
 * @brief Dump the list of timers to a stream
 *
 * If CONFIG_ESP_TIMER_PROFILING option is enabled, this prints the list of all
 * the existing timers. Otherwise, only the list active timers is printed.
 *
 * The format is:
 *
 *   name  period  alarm  times_armed  times_triggered  total_callback_run_time
 *
 * where:
 *
 * name — timer name (if CONFIG_ESP_TIMER_PROFILING is defined), or timer pointer
 * period — period of timer, in microseconds, or 0 for one-shot timer
 * alarm - time of the next alarm, in microseconds since boot, or 0 if the timer
 *         is not started
 *
 * The following fields are printed if CONFIG_ESP_TIMER_PROFILING is defined:
 *
 * times_armed — number of times the timer was armed via esp_timer_start_X
 * times_triggered - number of times the callback was called
 * total_callback_run_time - total time taken by callback to execute, across all calls
 *
 * @param stream stream (such as stdout) to dump the information to
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NO_MEM if can not allocate temporary buffer for the output
 */
esp_err_t esp_timer_dump(FILE* stream);


#ifdef __cplusplus
}
#endif

