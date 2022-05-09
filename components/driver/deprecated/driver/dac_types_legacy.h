/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "driver/dac_types.h"
#include "hal/dac_types.h"

#ifdef __cplusplus
extern "C" {
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
