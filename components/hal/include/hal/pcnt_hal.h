// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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

// The HAL layer for PCNT.
// There is no parameter check in the hal layer, so the caller must ensure the correctness of the parameters.

#pragma once

#include <stdio.h>
#include "soc/pcnt_periph.h"
#include "hal/pcnt_types.h"
#include "hal/pcnt_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Context that should be maintained by both the driver and the HAL
 */

typedef struct {
    pcnt_dev_t *dev;
} pcnt_hal_context_t;

/**
 * @brief Set PCNT counter mode
 *
 * @param hal Context of the HAL layer
 * @param unit PCNT unit number
 * @param channel PCNT channel number
 * @param pos_mode Counter mode when detecting positive edge
 * @param neg_mode Counter mode when detecting negative edge
 * @param hctrl_mode Counter mode when control signal is high level
 * @param lctrl_mode Counter mode when control signal is low level
 */
#define pcnt_hal_set_mode(hal, unit, channel, pos_mode, neg_mode, hctrl_mode, lctrl_mode) pcnt_ll_set_mode((hal)->dev, unit, channel, pos_mode, neg_mode, hctrl_mode, lctrl_mode)

/**
 * @brief Get pulse counter value
 *
 * @param hal Context of the HAL layer
 * @param unit  Pulse Counter unit number
 * @param count Pointer to accept counter value
 */
#define pcnt_hal_get_counter_value(hal, unit, count) pcnt_ll_get_counter_value((hal)->dev, unit, count)

/**
 * @brief Pause PCNT counter of PCNT unit
 *
 * @param hal Context of the HAL layer
 * @param unit PCNT unit number
 */
#define pcnt_hal_counter_pause(hal, unit) pcnt_ll_counter_pause((hal)->dev, unit)

/**
 * @brief Resume counting for PCNT counter
 *
 * @param hal Context of the HAL layer
 * @param unit PCNT unit number, select from unit_t
 */
#define pcnt_hal_counter_resume(hal, unit) pcnt_ll_counter_resume((hal)->dev, unit)

/**
 * @brief Clear and reset PCNT counter value to zero
 *
 * @param hal Context of the HAL layer
 * @param  unit PCNT unit number, select from unit_t
 */
#define pcnt_hal_counter_clear(hal, unit) pcnt_ll_counter_clear((hal)->dev, unit)

/**
 * @brief Enable PCNT interrupt for PCNT unit
 *        @note
 *        Each Pulse counter unit has five watch point events that share the same interrupt.
 *        Configure events with pcnt_event_enable() and pcnt_event_disable()
 *
 * @param hal Context of the HAL layer
 * @param unit PCNT unit number
 */
#define pcnt_hal_intr_enable(hal, unit) pcnt_ll_intr_enable((hal)->dev, unit)

/**
 * @brief Disable PCNT interrupt for PCNT unit
 *
 * @param hal Context of the HAL layer
 * @param unit PCNT unit number
 */
#define pcnt_hal_intr_disable(hal, unit) pcnt_ll_intr_disable((hal)->dev, unit)

/**
 * @brief Get PCNT interrupt status
 *
 * @param  hal Context of the HAL layer
 * @param  mask The interrupt status mask to be cleared. Pointer to accept value interrupt status mask.
 */
#define pcnt_hal_get_intr_status(hal, mask)  pcnt_ll_get_intr_status((hal)->dev, mask)

/**
 * @brief Clear PCNT interrupt status
 *
 * @param hal Context of the HAL layer
 * @param mask The interrupt status mask to be cleared.
 */
#define pcnt_hal_clear_intr_status(hal, mask)  pcnt_ll_clear_intr_status((hal)->dev, mask)

/**
 * @brief Enable PCNT event of PCNT unit
 *
 * @param hal Context of the HAL layer
 * @param unit PCNT unit number
 * @param evt_type Watch point event type.
 *                 All enabled events share the same interrupt (one interrupt per pulse counter unit).
 */
#define pcnt_hal_event_enable(hal, unit, evt_type) pcnt_ll_event_enable((hal)->dev, unit, evt_type)

/**
 * @brief Disable PCNT event of PCNT unit
 *
 * @param hal Context of the HAL layer
 * @param unit PCNT unit number
 * @param evt_type Watch point event type.
 *                 All enabled events share the same interrupt (one interrupt per pulse counter unit).
 */
#define pcnt_hal_event_disable(hal, unit, evt_type) pcnt_ll_event_disable((hal)->dev, unit, evt_type)

/**
 * @brief Set PCNT event value of PCNT unit
 *
 * @param hal Context of the HAL layer
 * @param unit PCNT unit number
 * @param evt_type Watch point event type.
 *                 All enabled events share the same interrupt (one interrupt per pulse counter unit).
 *
 * @param value Counter value for PCNT event
 */
#define pcnt_hal_set_event_value(hal, unit, evt_type, value) pcnt_ll_set_event_value((hal)->dev, unit, evt_type, value)

/**
 * @brief Get PCNT event value of PCNT unit
 *
 * @param hal Context of the HAL layer
 * @param unit PCNT unit number
 * @param evt_type Watch point event type.
 *                 All enabled events share the same interrupt (one interrupt per pulse counter unit).
 * @param value Pointer to accept counter value for PCNT event
 */
#define pcnt_hal_get_event_value(hal, unit, evt_type, value) pcnt_ll_get_event_value((hal)->dev, unit, evt_type, value)

/**
 * @brief Get PCNT event status
 *
 * @param hal Context of the HAL layer
 * @param unit PCNT unit number
 * @return event status word
 */
#define pcnt_hal_get_event_status(hal, unit) pcnt_ll_get_event_status((hal)->dev, unit)

/**
 * @brief Set PCNT filter value
 *
 * @param hal Context of the HAL layer
 * @param unit PCNT unit number
 * @param filter_val PCNT signal filter value, counter in APB_CLK cycles.
 *        Any pulses lasting shorter than this will be ignored when the filter is enabled.
 *        @note
 *        filter_val is a 10-bit value, so the maximum filter_val should be limited to 1023.
 */
#define pcnt_hal_set_filter_value(hal, unit, filter_val) pcnt_ll_set_filter_value((hal)->dev, unit, filter_val)

/**
 * @brief Get PCNT filter value
 *
 * @param hal Context of the HAL layer
 * @param unit PCNT unit number
 * @param filter_val Pointer to accept PCNT filter value.
 */
#define pcnt_hal_get_filter_value(hal, unit, filter_val) pcnt_ll_get_filter_value((hal)->dev, unit, filter_val)

/**
 * @brief Enable PCNT input filter
 *
 * @param hal Context of the HAL layer
 * @param unit PCNT unit number
 */
#define pcnt_hal_filter_enable(hal, unit) pcnt_ll_filter_enable((hal)->dev, unit)

/**
 * @brief Disable PCNT input filter
 *
 * @param hal Context of the HAL layer
 * @param unit PCNT unit number
 */
#define pcnt_hal_filter_disable(hal, unit) pcnt_ll_filter_disable((hal)->dev, unit)

/**
 * @brief Init the PCNT hal and set the PCNT to the default configuration. This function should be called first before other hal layer function is called
 *
 * @param hal Context of the HAL layer
 * @param pcnt_num The uart port number, the max port number is (PCNT_NUM_MAX -1)
 */
void pcnt_hal_init(pcnt_hal_context_t *hal, int pcnt_num);

#ifdef __cplusplus
}
#endif