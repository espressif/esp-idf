/*
 * SPDX-FileCopyrightText: 2016-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * @file esp_private/pm_impl.h
 *
 * This header file defines interface between PM lock functions (pm_locks.c)
 * and the chip-specific power management (DFS/light sleep) implementation.
 */

#include "soc/rtc.h"
#include "esp_pm.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This is an enum of possible power modes supported by the implementation
 */
typedef enum {
    PM_MODE_LIGHT_SLEEP,      //!< Light sleep
    PM_MODE_APB_MIN,          //!< Idle (no CPU frequency or APB frequency locks)
    PM_MODE_APB_MAX,          //!< Maximum APB frequency mode
    PM_MODE_CPU_MAX,          //!< Maximum CPU frequency mode
    PM_MODE_COUNT             //!< Number of items
} pm_mode_t;

/**
 * @brief Get the mode corresponding to a certain lock
 * @param type lock type
 * @param arg argument value for this lock (passed to esp_pm_lock_create)
 * @return lowest power consumption mode which meets the constraints of the lock
 */
pm_mode_t esp_pm_impl_get_mode(esp_pm_lock_type_t type, int arg);

/**
 * @brief Get CPU clock frequency by power mode
 * @param mode power mode
 * @return CPU clock frequency
 */
int esp_pm_impl_get_cpu_freq(pm_mode_t mode);

/**
 * If profiling is enabled, this data type will be used to store microsecond
 * timestamps.
 */
typedef int64_t pm_time_t;

/**
 * See \ref esp_pm_impl_switch_mode
 */
typedef enum {
    MODE_LOCK,
    MODE_UNLOCK
} pm_mode_switch_t;

/**
 * @brief Switch between power modes when lock is taken or released
 * @param mode pm_mode_t corresponding to the lock being taken or released,
 *             as returned by \ref esp_pm_impl_get_mode
 * @param lock_or_unlock
 *              - MODE_LOCK: lock was taken. Implementation needs to make sure
 *                that the constraints of the lock are met by switching to the
 *                given 'mode' or any of the higher power ones.
 *              - MODE_UNLOCK: lock was released. If all the locks for given
 *                mode are released, and no locks for higher power modes are
 *                taken, implementation can switch to one of lower power modes.
 * @param now timestamp when the lock was taken or released. Passed as
 *            a minor optimization, so that the implementation does not need to
 *            call pm_get_time again.
 */
void esp_pm_impl_switch_mode(pm_mode_t mode, pm_mode_switch_t lock_or_unlock, pm_time_t now);

/**
 * @brief Call once at startup to initialize pm implementation
 */
void esp_pm_impl_init(void);

/**
 * @brief Hook function for the idle task
 * Must be called from the IDLE task on each CPU before entering waiti state.
 */
void esp_pm_impl_idle_hook(void);

/**
 * @brief Hook function for the interrupt dispatcher
 * Must be called soon after entering the ISR
 */
void esp_pm_impl_isr_hook(void);

/**
 * @brief Dump the information about time spent in each of the pm modes.
 *
 * Prints three columns:
 * mode name, total time in mode (in microseconds), percentage of time in mode
 *
 * @param out stream to dump the information to
 */
void esp_pm_impl_dump_stats(FILE* out);

/**
 * @brief Hook function implementing `waiti` instruction, should be invoked from idle task context
 */
void esp_pm_impl_waiti(void);

/**
 * @brief Callback function type for peripherals to skip light sleep.
 *
 */
typedef bool (* skip_light_sleep_cb_t)(void);

/**
  * @brief  Register peripherals skip light sleep callback
  *
  * This function allows you to register a callback that gets the result
  * that if light sleep should be skipped by peripherals.
  * @param cb function to get the result
  * @return
  *   - ESP_OK on success
  *   - ESP_ERR_NO_MEM if no more callback slots are available
  */
esp_err_t esp_pm_register_skip_light_sleep_callback(skip_light_sleep_cb_t cb);

/**
  * @brief  Unregisterperipherals skip light sleep callback
  *
  * This function allows you to unregister a callback which was previously
  * registered using esp_register_skip_light_sleep_callback.
  * @param cb function to get the result
  * @return
  *   - ESP_OK on success
  *   - ESP_ERR_INVALID_STATE if the given callback hasn't been registered before
  */
esp_err_t esp_pm_unregister_skip_light_sleep_callback(skip_light_sleep_cb_t cb);

#ifdef CONFIG_PM_PROFILING
#define WITH_PROFILING
#include "esp_timer.h"
#endif

#ifdef WITH_PROFILING
static inline pm_time_t IRAM_ATTR pm_get_time(void)
{
    return esp_timer_get_time();
}
#endif // WITH_PROFILING

#ifdef __cplusplus
}
#endif
