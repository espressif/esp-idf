/*
 * SPDX-FileCopyrightText: 2017-2026 Espressif Systems (Shanghai) CO LTD
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
 * Timer callbacks can be dispatched from either an esp_timer task (default) or
 * directly from an ISR if CONFIG_ESP_TIMER_SUPPORTS_ISR_DISPATCH_METHOD is enabled
 * and ESP_TIMER_ISR dispatch method is specified. ISR dispatch reduces latency but
 * callbacks must be very short and follow ISR restrictions (no blocking, no FreeRTOS
 * calls except those ending in FromISR).
 *
 * Timer callbacks are called from the esp_timer task by default.
 * The CPU core affinity of the esp_timer task can be configured using
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
    esp_timer_dispatch_t dispatch_method;   //!< Dispatch callback from task (ESP_TIMER_TASK, default) or
    //                                !< ISR (ESP_TIMER_ISR). For ISR dispatch, enable Kconfig option
    //                                !< CONFIG_ESP_TIMER_SUPPORTS_ISR_DISPATCH_METHOD
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
 * @brief Start a one-shot timer with absolute alarm time
 *
 * This function starts a one-shot timer that will trigger once at the specified absolute time.
 * To start a timer relative to the current time, see esp_timer_start_once().
 * Timer represented by `timer` should not be running when this function is
 * called.
 *
 * @param timer timer handle created using esp_timer_create()
 * @param alarm_us timer alarm time, in absolute microseconds (as returned by esp_timer_get_time())
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the handle is invalid or alarm_us is in the past
 *      - ESP_ERR_INVALID_STATE if the timer is already running
 */
esp_err_t esp_timer_start_once_at(esp_timer_handle_t timer, uint64_t alarm_us);

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
 * @brief Start a periodic timer with absolute alarm time
 *
 * Timer represented by `timer` should not be running when this function is called.
 * This function starts the timer which will trigger every `period` microseconds.
 * The first alarm will be triggered at `first_alarm_us` time.
 * To start a periodic timer relative to the current time, see esp_timer_start_periodic().
 *
 * @param timer timer handle created using esp_timer_create()
 * @param period_us timer period, in microseconds
 * @param first_alarm_us timer first alarm time, in absolute microseconds (as returned by esp_timer_get_time())
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the handle is invalid or first_alarm_us is in the past
 *      - ESP_ERR_INVALID_STATE if the timer is already running
 */
esp_err_t esp_timer_start_periodic_at(esp_timer_handle_t timer, uint64_t period_us, uint64_t first_alarm_us);

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
 * @brief Restart a currently running timer with absolute alarm time
 *
 * Type of `timer` | Action
 * --------------- | ------
 * One-shot timer  | Restarted immediately and times out once at `alarm_us` microseconds
 * Periodic timer  | Restarted immediately with a new period of `period_us` microseconds. Next alarm is at `first_alarm_us` microseconds
 *
 * @param timer timer handle created using esp_timer_create()
 * @param period_us In case of a periodic timer, represents the new period.
 * @param first_alarm_us timer alarm time, in absolute microseconds (as returned by esp_timer_get_time())
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the handle is invalid or first_alarm_us is in the past
 *      - ESP_ERR_INVALID_STATE if the timer is not running
 */
esp_err_t esp_timer_restart_at(esp_timer_handle_t timer, uint64_t period_us, uint64_t first_alarm_us);

/**
 * @brief Stop a running timer
 *
 * This function stops the timer previously started using esp_timer_start_once()
 * or esp_timer_start_periodic().
 *
 * This function disarms the timer (prevents future callbacks) but does NOT
 * preempt an already-dispatched or currently-running callback.
 *
 * For a hard guarantee that no callback is running or will run after stop,
 * use esp_timer_stop_blocking() or poll esp_timer_is_active() until it returns false.
 *
 * @param timer timer handle created using esp_timer_create()
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if the timer is not running
 */
esp_err_t esp_timer_stop(esp_timer_handle_t timer);

/**
 * @brief Stop a running timer and wait for any in-flight callback to complete
 *
 * This function disarms the timer (prevents future callbacks). If the timer callback is
 * currently executing, it waits for the callback to finish, taking into account the specified waiting time.
 *
 * If this function returns ESP_OK, the caller is guaranteed that the timer is disarmed and
 * no callback is executing after the function returns (see the calling-context rules below).
 *
 * @note Behavior depends on calling context:
 *       - **User task context (normal case)**: Disarms the timer and waits up to timeout_ticks
 *         for any in-flight callback to complete. Returns ESP_OK when no callback is running,
 *         or ESP_ERR_TIMEOUT if the callback didn't finish in time.
 *
 *       - **ISR context**: Never blocks. Disarms the timer and returns immediately.
 *         If the timer callback may still be running, returns ESP_ERR_NOT_FINISHED.
 *
 *       - **Callback context**: Disarms the timer and returns immediately. Blocking from a callback
 *         would delay the esp_timer task and distort timing of other TASK-dispatch timers.
 *         Return value depends on the stopped timer state and the currently running callback:
 *         - Returns ESP_OK when the stop request is issued from the stopped timer’s own callback
 *           (the callback will complete naturally after it returns).
 *         - Returns ESP_ERR_NOT_FINISHED when stopping an ISR-dispatch timer from a foreign callback
 *           (i.e., the currently running callback belongs to another timer). In this case the caller
 *           cannot safely wait for ISR context to complete, so it is informed that the callback may
 *           still be running.
 *
 * If ESP_ERR_NOT_FINISHED is returned, it means that the timer is disarmed, but its callback
 * may still be running or will run soon. In this case, you can poll esp_timer_is_active()
 * until it returns false to ensure the callback has completed.
 *
 * @param timer Timer handle created using esp_timer_create()
 * @param timeout_ticks Maximum time to wait for callback completion, in FreeRTOS ticks.
 *                      Use portMAX_DELAY to wait indefinitely.
 * @return
 *      - ESP_OK on success (timer is disarmed, no callback is running or will run)
 *      - ESP_ERR_TIMEOUT if the callback didn't complete within timeout_ticks
 *      - ESP_ERR_INVALID_ARG if the timer handle is invalid
 *      - ESP_ERR_INVALID_STATE if esp_timer is not initialized
 *      - ESP_ERR_NOT_FINISHED if the callback may still be running (e.g., ISR context, or TIMER_TASK while an ISR callback is in-flight)
 */
esp_err_t esp_timer_stop_blocking(esp_timer_handle_t timer, uint32_t timeout_ticks);

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
 * @brief Get the timestamp when the next timer will expire
 *
 * This function returns the alarm time of the timer that will expire soonest
 * across all dispatch methods (TASK and ISR).
 *
 * @return Timestamp of the nearest timer event, in microseconds since boot.
 *         Returns INT64_MAX if no timers are currently armed.
 *         The timebase is the same as for the values returned by esp_timer_get_time().
 */
int64_t esp_timer_get_next_alarm(void);

/**
 * @brief Get the timestamp when the next timer that should wake from light sleep will expire
 *
 * This function returns the alarm time of the timer that will expire soonest, excluding
 * timers that have ::esp_timer_create_args_t::skip_unhandled_events enabled. Used by
 * the power management system to determine when to wake from light sleep.
 *
 * @return Timestamp of the nearest timer event that should wake the system, in microseconds since boot.
 *         Returns INT64_MAX if no wake-capable timers are currently armed.
 *         The timebase is the same as for the values returned by esp_timer_get_time().
 */
int64_t esp_timer_get_next_alarm_for_wake_up(void);

/**
 * @brief Get the period of a timer
 *
 * This function fetches the period of a timer.
 * For a one-shot timer, the period will be 0.
 * For a periodic timer, returns the interval between successive callback invocations.
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
 * This function prints statistics about timers. Without CONFIG_ESP_TIMER_PROFILING,
 * it prints only the active (armed) timers. With CONFIG_ESP_TIMER_PROFILING enabled,
 * it prints all created timers (both armed and disarmed).
 *
 * Output format without CONFIG_ESP_TIMER_PROFILING:
 *
 * Name              | Period      | Alarm
 * ------------------|-------------|-------------
 * timer@0x3ffb1234  | 1000000     | 12345678
 *
 * - Name — timer pointer (or timer name if CONFIG_ESP_TIMER_PROFILING is enabled)
 * - Period — period of timer in microseconds, or 0 for one-shot timers
 * - Alarm — time of the next alarm in microseconds since boot, or 0 if the timer is not armed
 *
 * Output format with CONFIG_ESP_TIMER_PROFILING:
 *
 * Name              | Period      | Alarm       | Times_armed | Times_trigg | Times_skip | Cb_exec_time
 * ------------------|-------------|-------------|-------------|-------------|------------|-------------
 * my_timer          | 1000000     | 12345678    | 42          | 40          | 2          | 1234
 *
 * - Name — timer name (from esp_timer_create_args_t::name)
 * - Period — same as above
 * - Alarm — same as above
 * - Times_armed — number of times the timer was armed via esp_timer_start_*
 * - Times_trigg — number of times the callback was invoked
 * - Times_skip — number of times the callback was skipped (periodic timers with skip_unhandled_events)
 * - Cb_exec_time — total time spent executing the callback, in microseconds
 *
 * @param stream stream (such as stdout) to which to dump the information
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NO_MEM if cannot allocate temporary buffer for the output
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
 * @brief Check if a timer is active
 *
 * A timer is considered active if it is armed (will expire in the future) or
 * if its callback is currently executing. After calling esp_timer_stop(), the
 * timer is disarmed but its callback may still be running, so this function
 * may still return true.
 *
 * @param timer timer handle created using esp_timer_create()
 * @return
 *      - true if timer is armed or its callback is currently running
 *      - false if timer is disarmed and callback is not running
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
