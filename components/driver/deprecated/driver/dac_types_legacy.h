/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include "sdkconfig.h"
#include "hal/dac_types.h"
#if CONFIG_IDF_TARGET_ESP32S2
#include "hal/adc_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The multiple of the amplitude of the cosine wave generator. The max amplitude is VDD3P3_RTC.
 */
typedef enum {
    DAC_CW_SCALE_1 = 0x0,   /*!< 1/1. Default. */
    DAC_CW_SCALE_2 = 0x1,   /*!< 1/2. */
    DAC_CW_SCALE_4 = 0x2,   /*!< 1/4. */
    DAC_CW_SCALE_8 = 0x3,   /*!< 1/8. */
} dac_cw_scale_t;

/**
 * @brief Set the phase of the cosine wave generator output.
 */
typedef enum {
    DAC_CW_PHASE_0   = 0x2, /*!< Phase shift +0° */
    DAC_CW_PHASE_180 = 0x3, /*!< Phase shift +180° */
} dac_cw_phase_t;

#if CONFIG_IDF_TARGET_ESP32S2
/**
 * @brief DAC digital controller (DMA mode) work mode.
 */
typedef enum {
    DAC_CONV_NORMAL,        /*!< The data in the DMA buffer is simultaneously output to the enable channel of the DAC. */
    DAC_CONV_ALTER,         /*!< The data in the DMA buffer is alternately output to the enable channel of the DAC. */
    DAC_CONV_MAX
} dac_digi_convert_mode_t;

/**
 * @brief DAC digital controller (DMA mode) configuration parameters.
 */
typedef struct {
    dac_digi_convert_mode_t mode;   /*!<DAC digital controller (DMA mode) work mode. See ``dac_digi_convert_mode_t``. */
    uint32_t interval;              /*!<The number of interval clock cycles for the DAC digital controller to output voltage.
                                        The unit is the divided clock. Range: 1 ~ 4095.
                                        Expression: `dac_output_freq` = `controller_clk` / interval. Refer to ``adc_digi_clk_t``.
                                        Note: The sampling rate of each channel is also related to the conversion mode (See ``dac_digi_convert_mode_t``) and pattern table settings. */
    adc_digi_clk_t dig_clk;         /*!<DAC digital controller clock divider settings. Refer to ``adc_digi_clk_t``.
                                        Note: The clocks of the DAC digital controller use the ADC digital controller clock divider. */
} dac_digi_config_t;
#endif

/**
 * @brief Config the cosine wave generator function in DAC module.
 */
typedef struct {
    dac_channel_t en_ch;    /*!< Enable the cosine wave generator of DAC channel. */
    dac_cw_scale_t scale;   /*!< Set the amplitude of the cosine wave generator output. */
    dac_cw_phase_t phase;   /*!< Set the phase of the cosine wave generator output. */
    uint32_t freq;          /*!< Set frequency of cosine wave generator output. Range: 130(130Hz) ~ 55000(100KHz). */
    int8_t offset;          /*!< Set the voltage value of the DC component of the cosine wave generator output.
                                 Note: Unreasonable settings can cause waveform to be oversaturated. Range: -128 ~ 127. */
} dac_cw_config_t;

#ifdef __cplusplus
}
#endif
