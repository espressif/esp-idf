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

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The HAL layer for touch sensor (esp32 specific part)

#pragma once

#include "hal/touch_sensor_ll.h"
#include "hal/touch_sensor_types.h"

#include_next "hal/touch_sensor_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Set touch sensor measurement time.
 *
 * @param meas_time The duration of the touch sensor measurement.
 *                  t_meas = meas_time / (8MHz), the maximum measure time is 0xffff / 8M = 8.19 ms.
 */
#define touch_hal_set_meas_time(meas_time) touch_ll_set_meas_time(meas_time)

/**
 * Get touch sensor measurement time.
 *
 * @param meas_time Pointer to accept measurement cycle count.
 */
#define touch_hal_get_meas_time(meas_time) touch_ll_get_meas_time(meas_time)

/**
 * Set touch sensor interrupt trigger mode.
 * Interrupt can be triggered either when touch value is less than
 * threshold or when touch value is more than threshold.
 *
 * @param mode Touch sensor interrupt trigger mode.
 */
#define touch_hal_set_trigger_mode(mode) touch_ll_set_trigger_mode(mode)

/**
 * Get touch sensor interrupt trigger mode.
 * Interrupt can be triggered either when touch value is less than
 * threshold or when touch value is more than threshold.
 *
 * @param mode Touch sensor interrupt trigger mode.
 */
#define touch_hal_get_trigger_mode(mode) touch_ll_get_trigger_mode(mode)

/**
 * Set touch sensor interrupt trigger source. There are two sets of touch signals.
 * Set1 and set2 can be mapped to several touch signals. Either set will be triggered
 * if at least one of its touch signal is 'touched'. The interrupt can be configured to be generated
 * if set1 is triggered, or only if both sets are triggered.
 *
 * @param src Touch sensor interrupt trigger source.
 */
#define touch_hal_set_trigger_source(src) touch_ll_set_trigger_source(src)

/**
 * Get touch sensor interrupt trigger source.
 *
 * @param src Pointer to accept touch sensor interrupt trigger source.
 */
#define touch_hal_get_trigger_source(src) touch_ll_get_trigger_source(src)

/**
 * Set touch sensor group mask.
 * Touch pad module has two sets of signals, 'Touched' signal is triggered only if
 * at least one of touch pad in this group is "touched".
 * This function will set the register bits according to the given bitmask.
 *
 * @param set1_mask bitmask of touch sensor signal group1, it's a 10-bit value
 * @param set2_mask bitmask of touch sensor signal group2, it's a 10-bit value
 */
#define touch_hal_set_group_mask(group1_mask, group2_mask) touch_ll_set_group_mask(group1_mask, group2_mask)

/**
 * Get touch sensor group mask.
 *
 * @param set1_mask pointer to accept bitmask of touch sensor signal group1, it's a 10-bit value
 * @param set2_mask pointer to accept bitmask of touch sensor signal group2, it's a 10-bit value
 */
#define touch_hal_get_group_mask(group1_mask, group2_mask) touch_ll_get_group_mask(group1_mask, group2_mask)

/**
 * Clear touch sensor group mask.
 *
 * @param set1_mask pointer to accept bitmask of touch sensor signal group1, it's a 10-bit value
 * @param set2_mask pointer to accept bitmask of touch sensor signal group2, it's a 10-bit value
 */
#define touch_hal_clear_group_mask(group1_mask, group2_mask) touch_ll_clear_group_mask(group1_mask, group2_mask)

/**
 * To enable touch pad interrupt.
 */
#define touch_hal_intr_enable() touch_ll_intr_enable()

/**
 * To disable touch pad interrupt.
 */
#define touch_hal_intr_disable() touch_ll_intr_disable()

/**
 * To clear touch pad interrupt.
 */
#define touch_hal_intr_clear() touch_ll_intr_clear()

/**
 * Get the touch pad which caused wakeup from deep sleep.
 *
 * @param pad_num pointer to touch pad which caused wakeup.
 */
void touch_hal_get_wakeup_status(touch_pad_t *pad_num);

#ifdef __cplusplus
}
#endif