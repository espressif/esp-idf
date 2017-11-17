// Copyright 2016-2017 Espressif Systems (Shanghai) PTE LTD
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
 * @file pm_impl.h
 *
 * This header file defines interface between PM lock functions (pm_locks.c)
 * and the chip-specific power management (DFS/light sleep) implementation.
 */

#include "soc/rtc.h"
#include "esp_pm.h"
#include "esp_timer.h"
#include "sdkconfig.h"


/**
 * This is an enum of possible power modes supported by the implementation
 */
typedef enum {
    PM_MODE_LIGHT_SLEEP,//!< Light sleep
    PM_MODE_APB_MIN,    //!< Idle (no CPU frequency or APB frequency locks)
    PM_MODE_APB_MAX,    //!< Maximum APB frequency mode
    PM_MODE_CPU_MAX,    //!< Maximum CPU frequency mode
    PM_MODE_COUNT       //!< Number of items
} pm_mode_t;

/**
 * @brief Get the mode corresponding to a certain lock
 * @param type lock type
 * @param arg argument value for this lock (passed to esp_pm_lock_create)
 * @return lowest power consumption mode which meets the constraints of the lock
 */
pm_mode_t esp_pm_impl_get_mode(esp_pm_lock_type_t type, int arg);

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
void esp_pm_impl_init();

/**
 * @brief Hook function for the idle task
 * Must be called from the IDLE task on each CPU before entering waiti state.
 */
void esp_pm_impl_idle_hook();

/**
 * @brief Hook function for the interrupt dispatcher
 * Must be called soon after entering the ISR
 */
void esp_pm_impl_isr_hook();

/**
 * @brief Dump the information about time spent in each of the pm modes.
 *
 * Prints three columns:
 * mode name, total time in mode (in microseconds), percentage of time in mode
 *
 * @param out stream to dump the information to
 */
void esp_pm_impl_dump_stats(FILE* out);


#ifdef CONFIG_PM_PROFILING
#define WITH_PROFILING
#endif

#ifdef WITH_PROFILING
static inline pm_time_t IRAM_ATTR pm_get_time()
{
    return esp_timer_get_time();
}
#endif // WITH_PROFILING
