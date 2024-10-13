/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * @file esp_timer.h
 * @brief microsecond-precision 64-bit timer API, replacement for ets_timer
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
 * To be implemented: it should be possible to request the callback to be called
 * directly from the ISR. This reduces the latency, but has potential impact on
 * all other callbacks which need to be dispatched. This option should only be
 * used for simple callback functions, which do not take longer than a few
 * microseconds to run.
 *
 * Timer callbacks are called from a task running on CPU0.
 * On chips with multiple cores, CPU0 (default) can be changed using
 * the Kconfig option CONFIG_ESP_TIMER_TASK_AFFINITY.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_etm.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opaque type representing a single timer handle
 */
typedef struct esp_timer* esp_timer_handle_t;

/**
 * @brief Timer callback function type
 * @param arg pointer to opaque user-specific data
 */
typedef void (*esp_timer_cb_t)(void* arg);

/**
 * @brief Method to dispatch timer callback
 */
typedef enum {
    ESP_TIMER_TASK,     //!< Callback is dispatched from esp_timer task
#if CONFIG_ESP_TIMER_SUPPORTS_ISR_DISPATCH_METHOD || __DOXYGEN__
    ESP_TIMER_ISR,      //!< Callback is dispatched from interrupt handler
#endif
    ESP_TIMER_MAX,      //!< Sentinel value for the number of callback dispatch methods
} esp_timer_dispatch_t;

/**
 * @brief Timer configuration passed to esp_timer_create()
 */
typedef struct {
    esp_timer_cb_t callback;        //!< Callback function to execute when timer expires
    void* arg;                      //!< Argument to pass to callback
    esp_timer_dispatch_t dispatch_method;   //!< Dispatch callback from task or ISR; if not specified, esp_timer task
    //                                !< is used; for ISR to work, also set Kconfig option
    //                                !< `CONFIG_ESP_TIMER_SUPPORTS_ISR_DISPATCH_METHOD`
    const char* name;               //!< Timer name, used in esp_timer_dump() function
    bool skip_unhandled_events;     //!< Setting to skip unhandled events in light sleep for periodic timers
} esp_timer_create_args_t;

/**
 * @brief Minimal initialization of esp_timer
 *
 * @note This function is called from startup code. Applications do not need
 * to call this function before using other esp_timer APIs.
 *
 * This function can be called very early in startup process, after this call
 * only esp_timer_get_time() function can be used.
 *
 * @return
 *      - ESP_OK on success
 */
esp_err_t esp_timer_early_init(void);

/**
 * @brief Initialize esp_timer library
 *
 * @note This function is called from startup code. Applications do not need
 * to call this function before using other esp_timer APIs.
 * Before calling this function, esp_timer_early_init() must be called by the
 * startup code.
 *
 * This function will be called from startup code on every core.
 * If Kconfig option `CONFIG_ESP_TIMER_ISR_AFFINITY` is set to `NO_AFFINITY`,
 * it allocates the timer ISR on MULTIPLE cores and
 * creates the timer task which can be run on any core.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NO_MEM if allocation has failed
 *      - ESP_ERR_INVALID_STATE if already initialized
 *      - other errors from interrupt allocator
 */
esp_err_t esp_timer_init(void);

/**
 * @brief De-initialize esp_timer library
 *
 * @note Normally this function should not be called from applications
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if not yet initialized
 */
esp_err_t esp_timer_deinit(void);

/**
 * @brief Create an esp_timer instance
 *
 * @note When timer no longer needed, delete it using esp_timer_delete().
 *
 * @param create_args   Pointer to a structure with timer creation arguments.
 *                      Not saved by the library, can be allocated on the stack.
 * @param[out] out_handle  Output, pointer to esp_timer_handle_t variable that
 *                         holds the created timer handle.
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
 * @brief Start a one-shot timer
 *
 * Timer represented by `timer` should not be running when this function is called.
 *
 * @param timer timer handle created using esp_timer_create()
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
 * Timer represented by `timer` should not be running when this function is called.
 * This function starts the timer which will trigger every `period` microseconds.
 *
 * @param timer timer handle created using esp_timer_create()
 * @param period timer period, in microseconds
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the handle is invalid
 *      - ESP_ERR_INVALID_STATE if the timer is already running
 */
esp_err_t esp_timer_start_periodic(esp_timer_handle_t timer, uint64_t period);

/**
 * @brief Restart a currently running timer
 *
 * Type of `timer` | Action
 * --------------- | ------
 * One-shot timer  | Restarted immediately and times out once in `timeout_us` microseconds
 * Periodic timer  | Restarted immediately with a new period of `timeout_us` microseconds
 *
 * @param timer timer handle created using esp_timer_create()
 * @param timeout_us Timeout in microseconds relative to the current time.
 *                   In case of a periodic timer, also represents the new period.
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the handle is invalid
 *      - ESP_ERR_INVALID_STATE if the timer is not running
 */
esp_err_t esp_timer_restart(esp_timer_handle_t timer, uint64_t timeout_us);

/**
 * @brief Stop a running timer
 *
 * This function stops the timer previously started using esp_timer_start_once()
 * or esp_timer_start_periodic().
 *
 * @param timer timer handle created using esp_timer_create()
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
 * @param timer timer handle created using esp_timer_create()
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if the timer is running
 */
esp_err_t esp_timer_delete(esp_timer_handle_t timer);

/**
 * @brief Get time in microseconds since boot
 * @return Number of microseconds since the initialization of ESP Timer
 */
int64_t esp_timer_get_time(void);

/**
 * @brief Get the timestamp of the next expected timeout
 * @return Timestamp of the nearest timer event, in microseconds.
 *         The timebase is the same as for the values returned by esp_timer_get_time().
 */
int64_t esp_timer_get_next_alarm(void);

/**
 * @brief Get the timestamp of the next expected timeout excluding those timers
 *        that should not interrupt light sleep (such timers have
 *        ::esp_timer_create_args_t::skip_unhandled_events enabled)
 * @return Timestamp of the nearest timer event, in microseconds.
 *         The timebase is the same as for the values returned by esp_timer_get_time().
 */
int64_t esp_timer_get_next_alarm_for_wake_up(void);

/**
 * @brief Get the period of a timer
 *
 * This function fetches the timeout period of a timer.
 * For a one-shot timer, the timeout period will be 0.
 *
 * @param timer timer handle created using esp_timer_create()
 * @param period memory to store the timer period value in microseconds
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the arguments are invalid
 */
esp_err_t esp_timer_get_period(esp_timer_handle_t timer, uint64_t *period);

/**
 * @brief Get the expiry time of a one-shot timer
 *
 * This function fetches the expiry time of a one-shot timer.
 *
 * @note Passing the timer handle of a periodic timer will result in an error.
 *
 * @param timer timer handle created using esp_timer_create()
 * @param expiry memory to store the timeout value in microseconds
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the arguments are invalid
 *      - ESP_ERR_NOT_SUPPORTED if the timer type is periodic
 */
esp_err_t esp_timer_get_expiry_time(esp_timer_handle_t timer, uint64_t *expiry);

/**
 * @brief Dump the list of timers to a stream
 *
 * By default, this function prints the list of active (running) timers. The output format is:
 *
 * | Name | Period | Alarm |
 *
 * - Name — timer pointer
 * - Period — period of timer in microseconds, or 0 for one-shot timer
 * - Alarm - time of the next alarm in microseconds since boot, or 0 if the timer is not started
 *
 * To print the list of all created timers, enable Kconfig option `CONFIG_ESP_TIMER_PROFILING`.
 * In this case, the output format is:
 *
 * | Name | Period | Alarm | Times_armed | Times_trigg | Times_skip | Cb_exec_time |
 *
 * - Name — timer name
 * - Period — same as above
 * - Alarm — same as above
 * - Times_armed — number of times the timer was armed via esp_timer_start_X
 * - Times_triggered - number of times the callback was triggered
 * - Times_skipped - number of times the callback was skipped
 * - Callback_exec_time - total time taken by callback to execute, across all calls
 *
 * @param stream stream (such as stdout) to which to dump the information
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NO_MEM if can not allocate temporary buffer for the output
 */
esp_err_t esp_timer_dump(FILE* stream);

#if CONFIG_ESP_TIMER_SUPPORTS_ISR_DISPATCH_METHOD || defined __DOXYGEN__
/**
 * @brief Requests a context switch from a timer callback function.
 *
 * This only works for a timer that has an ISR dispatch method.
 * The context switch will be called after all ISR dispatch timers have been processed.
 */
void esp_timer_isr_dispatch_need_yield(void);
#endif // CONFIG_ESP_TIMER_SUPPORTS_ISR_DISPATCH_METHOD || defined __DOXYGEN__

/**
 * @brief Returns status of a timer, active or not
 *
 * This function is used to identify if the timer is still active (running) or not.
 *
 * @param timer timer handle created using esp_timer_create()
 * @return
 *      - 1 if timer is still active (running)
 *      - 0 if timer is not active
 */
bool esp_timer_is_active(esp_timer_handle_t timer);

/**
 * @brief Get the ETM event handle of esp_timer underlying alarm event
 *
 * @note The created ETM event object can be deleted later using esp_etm_del_event()
 *
 * @note The ETM event is generated by the underlying hardware - systimer;
 *       therefore, if the esp_timer is not clocked by systimer, then no ETM event will be generated.
 *
 * @param[out] out_event Returned ETM event handle
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t esp_timer_new_etm_alarm_event(esp_etm_event_handle_t *out_event);

#ifdef __cplusplus
}
#endif
