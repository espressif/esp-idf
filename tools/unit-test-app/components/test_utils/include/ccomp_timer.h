// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#include <stdint.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Start the timer on the current core.
 *
 * @return
 *  - ESP_OK: Success
 *  - ESP_ERR_INVALID_STATE: The timer has already been started previously.
 *  - Others: Fail
 */
esp_err_t ccomp_timer_start(void);

/**
 * @brief Stop the timer on the current core.
 * 
 * @note Returns -1 if an error has occured and stopping the timer failed.
 *
 * @return The time elapsed from the last ccomp_timer_start call on the current
 *         core.
 */
int64_t ccomp_timer_stop(void);

/**
 * Return the current timer value on the current core without stopping the timer.
 *
 * @note Returns -1 if an error has occured and stopping the timer failed.
 * 
 * @note If called while timer is active i.e. between ccomp_timer_start and ccomp_timer_stop,
 * this function returns the elapsed time from ccomp_timer_start. Once ccomp_timer_stop 
 * has been called, the timer becomes inactive and stops keeping time. As a result, if this function gets
 * called after esp_cccomp_timer_stop, this function will return the same value as when the timer was stopped.
 *
 * @return The elapsed time from the last ccomp_timer_start call on the current 
 *          core.
 */
int64_t ccomp_timer_get_time(void);

#ifdef __cplusplus
}
#endif
