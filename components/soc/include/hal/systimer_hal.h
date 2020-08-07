// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "hal/systimer_types.h"

/**
 * @brief enable systimer counter
 */
void systimer_hal_enable_counter(systimer_counter_id_t counter_id);

/**
 * @brief get current counter value
 */
uint64_t systimer_hal_get_counter_value(systimer_counter_id_t counter_id);

/**
 * @brief get current time (in microseconds)
 */
uint64_t systimer_hal_get_time(systimer_counter_id_t counter_id);

/**
 * @brief set alarm time
 */
void systimer_hal_set_alarm_value(systimer_alarm_id_t alarm_id, uint64_t timestamp);

/**
 * @brief get alarm time
 */
uint64_t systimer_hal_get_alarm_value(systimer_alarm_id_t alarm_id);

/**
 * @brief enable alarm interrupt
 */
void systimer_hal_enable_alarm_int(systimer_alarm_id_t alarm_id);

/**
 * @brief select alarm mode
 */
void systimer_hal_select_alarm_mode(systimer_alarm_id_t alarm_id, systimer_alarm_mode_t mode);

/**
 * @brief update systimer step when apb clock gets changed
 */
void systimer_hal_on_apb_freq_update(uint32_t apb_ticks_per_us);

/**
 * @brief move systimer counter value forward or backward
 */
void systimer_hal_counter_value_advance(systimer_counter_id_t counter_id, int64_t time_us);

/**
 * @brief initialize systimer in HAL layer
 */
void systimer_hal_init(void);

/**
 * @brief  connect alarm unit to selected counter
 */
void systimer_hal_connect_alarm_counter(systimer_alarm_id_t alarm_id, systimer_counter_id_t counter_id);

#ifdef __cplusplus
}
#endif
