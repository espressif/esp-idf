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

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in soc/include/hal/readme.md
 ******************************************************************************/

// The HAL layer for ADC (esp32s2 specific part)

#pragma once

#include "hal/adc_ll.h"
#include "hal/adc_types.h"

#include_next "hal/adc_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------
                    Digital controller setting
---------------------------------------------------------------*/
/**
 * Digital controller deinitialization.
 */
void adc_hal_digi_deinit(void);

/**
 * Setting the digital controller.
 *
 * @param cfg Pointer to digital controller paramter.
 */
void adc_hal_digi_controller_config(const adc_digi_config_t *cfg);

/**
 * ADC Digital controller output data invert or not.
 *
 * @param adc_n ADC unit.
 * @param inv_en data invert or not.
 */
#define adc_hal_digi_output_invert(adc_n, inv_en) adc_ll_digi_output_invert(adc_n, inv_en)

/**
 * Sets the number of interval clock cycles for the digital controller to trigger the measurement.
 *
 * @note The trigger interval should not be less than the sampling time of the SAR ADC.
 * @param cycle The number of clock cycles for the trigger interval. The unit is the divided clock. Range: 40 ~ 4095.
 */
#define adc_hal_digi_set_trigger_interval(cycle) adc_ll_digi_set_trigger_interval(cycle)

/**
 * Enable digital controller to trigger the measurement.
 */
void adc_hal_digi_enable(void);

/**
 * Disable digital controller to trigger the measurement.
 */
void adc_hal_digi_disable(void);

/**
 * Set ADC digital controller clock division factor. The clock divided from `APLL` or `APB` clock.
 * Enable clock and select clock source for ADC digital controller.
 * Expression: controller_clk = APLL/APB * (div_num  + div_a / div_b + 1).
 *
 * @param clk Refer to `adc_digi_clk_t`.
 */
void adc_hal_digi_clk_config(void);

/**
 * Reset adc digital controller filter.
 *
 * @param filter_idx ADC filter unit.
 */
#define adc_hal_digi_filter_reset(filter_idx) adc_ll_digi_filter_reset(filter_idx)

/**
 * Set adc digital controller filter factor.
 *
 * @param filter_idx ADC filter unit.
 * @param filter Filter config. Expression: filter_data = (k-1)/k * last_data + new_data / k. Set values: (2, 4, 8, 16, 64).
 */
void adc_hal_digi_filter_set_factor(adc_digi_filter_idx_t filter_idx, adc_digi_filter_t *filter);

/**
 * Get adc digital controller filter factor.
 *
 * @param filter_idx ADC filter unit.
 * @param factor Expression: filter_data = (k-1)/k * last_data + new_data / k. Set values: (2, 4, 8, 16, 64).
 */
void adc_hal_digi_filter_get_factor(adc_digi_filter_idx_t filter_idx, adc_digi_filter_t *filter);

/**
 * Enable/disable adc digital controller filter.
 * Filtering the ADC data to obtain smooth data at higher sampling rates.
 *
 * @note The filter will filter all the enabled channel data of the each ADC unit at the same time.
 * @param filter_idx ADC filter unit.
 * @param enable True to enable the filter, otherwise disable.
 */
void adc_hal_digi_filter_enable(adc_digi_filter_idx_t filter_idx, bool enable);

/**
 * Config monitor of adc digital controller.
 *
 * @note If the channel info is not supported, the monitor function will not be enabled.
 * @param mon_idx ADC monitor index.
 * @param config Refer to `adc_digi_monitor_t`.
 */
void adc_hal_digi_monitor_config(adc_digi_monitor_idx_t mon_idx, adc_digi_monitor_t *config);

/**
 * Enable/disable monitor of adc digital controller.
 *
 * @note The monitor will monitor all the enabled channel data of the each ADC unit at the same time.
 * @param mon_idx ADC monitor index.
 * @param enable True to enable the monitor, otherwise disable.
 */
void adc_hal_digi_monitor_enable(adc_digi_monitor_idx_t mon_idx, bool enable);

/**
 * Enable interrupt of adc digital controller by bitmask.
 *
 * @param adc_n ADC unit.
 * @param intr Interrupt bitmask.
 */
#define adc_hal_digi_intr_enable(adc_n, intr) adc_ll_digi_intr_enable(adc_n, intr)

/**
 * Disable interrupt of adc digital controller by bitmask.
 *
 * @param adc_n ADC unit.
 * @param intr Interrupt bitmask.
 */
#define adc_hal_digi_intr_disable(adc_n, intr) adc_ll_digi_intr_disable(adc_n, intr)

/**
 * Clear interrupt of adc digital controller by bitmask.
 *
 * @param adc_n ADC unit.
 * @param intr Interrupt bitmask.
 */
#define adc_hal_digi_intr_clear(adc_n, intr) adc_ll_digi_intr_clear(adc_n, intr)

/**
 * Get interrupt status mask of adc digital controller.
 *
 * @param adc_n ADC unit.
 * @return
 *     - intr Interrupt bitmask.
 */
#define adc_hal_digi_get_intr_status(adc_n) adc_ll_digi_get_intr_status(adc_n)


/**
 * Set DMA eof num of adc digital controller.
 * If the number of measurements reaches `dma_eof_num`, then `dma_in_suc_eof` signal is generated.
 *
 * @param num eof num of DMA.
 */
#define adc_hal_digi_dma_set_eof_num(num) adc_ll_digi_dma_set_eof_num(num)

/**
 * Enable output data to DMA from adc digital controller.
 */
#define adc_hal_digi_dma_enable() adc_ll_digi_dma_enable()

/**
 * Disable output data to DMA from adc digital controller.
 */
#define adc_hal_digi_dma_disable() adc_ll_digi_dma_disable()

/**
 * Reset adc digital controller.
 */
#define adc_hal_digi_reset() adc_ll_digi_reset()

/*---------------------------------------------------------------
                    Common setting
---------------------------------------------------------------*/

/**
 * Config ADC2 module arbiter.
 * The arbiter is to improve the use efficiency of ADC2. After the control right is robbed by the high priority,
 * the low priority controller will read the invalid ADC2 data, and the validity of the data can be judged by the flag bit in the data.
 *
 * @note Only ADC2 support arbiter.
 * @note The arbiter's working clock is APB_CLK. When the APB_CLK clock drops below 8 MHz, the arbiter must be in shield mode.
 * @note Default priority: Wi-Fi > RTC > Digital;
 *
 * @param config Refer to `adc_arbiter_t`.
 */
void adc_hal_arbiter_config(adc_arbiter_t *config);

#ifdef __cplusplus
}
#endif
