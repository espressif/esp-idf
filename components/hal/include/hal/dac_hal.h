/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

#pragma once

#include "hal/dac_ll.h"
#include "hal/dac_types.h"

/**
 * Power on dac module and start output voltage.
 *
 * @note Before powering up, make sure the DAC PAD is set to RTC PAD and floating status.
 * @param channel DAC channel num.
 */
#define dac_hal_power_on(channel) dac_ll_power_on(channel)

/**
 * Power done dac module and stop output voltage.
 *
 * @param channel DAC channel num.
 */
#define dac_hal_power_down(channel) dac_ll_power_down(channel)

/**
 * Enable/disable the synchronization operation function of ADC1 and DAC.
 *
 * @note  If enabled(default), ADC RTC controller sampling will cause the DAC channel output voltage.
 *
 * @param enable Enable or disable adc and dac synchronization function.
 */
#define  dac_hal_rtc_sync_by_adc(enable) dac_ll_rtc_sync_by_adc(enable)

/**
 * Output voltage with value (8 bit).
 *
 * @param channel DAC channel num.
 * @param value Output value. Value range: 0 ~ 255.
 *        The corresponding range of voltage is 0v ~ VDD3P3_RTC.
 */
#define dac_hal_update_output_value(channel, value) dac_ll_update_output_value(channel, value)

/**
 * Enable cosine wave generator output.
 */
#define dac_hal_cw_generator_enable() dac_ll_cw_generator_enable()

/**
 * Disable cosine wave generator output.
 */
#define dac_hal_cw_generator_disable() dac_ll_cw_generator_disable()

/**
 * Config the cosine wave generator function in DAC module.
 *
 * @param cw Configuration.
 */
// void dac_hal_cw_generator_config(dac_cw_config_t *cw);

/**
 * Enable/disable DAC output data from DMA.
 */
#define dac_hal_digi_enable_dma(enable) dac_ll_digi_enable_dma(enable)
