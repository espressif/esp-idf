/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"
#include "hal/adc_types.h"
#include "esp_bit_defs.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_DAC_SUPPORTED

/**
 * @brief DAC channel mask
 *
 */
typedef enum {
    DAC_CHANNEL_MASK_CH0  = BIT(0),             /*!< DAC channel 0 is GPIO25(ESP32) / GPIO17(ESP32S2) */
    DAC_CHANNEL_MASK_CH1  = BIT(1),             /*!< DAC channel 1 is GPIO26(ESP32) / GPIO18(ESP32S2) */
    DAC_CHANNEL_MASK_BOTH = BIT(0) | BIT(1),    /*!< Both DAC channel 0 and channel 1 */
} dac_channel_mask_t;

/**
 * @brief DAC channel work mode in dma mode
 * @note  Only take effect when multiple channels enabled.
 * @note  Assume the data in buffer is 'A B C D E F'
 *        DAC_CHANNEL_MODE_SIMUL:
 *          - channel 0: A B C D E F
 *          - channel 1: A B C D E F
 *        DAC_CHANNEL_MODE_ALTER:
 *          - channel 0: A C E
 *          - channel 1: B D F
 */
typedef enum {
    DAC_CHANNEL_MODE_SIMUL,         /*!< The data in the DMA buffer is simultaneously output to the enable channel of the DAC. */
    DAC_CHANNEL_MODE_ALTER,         /*!< The data in the DMA buffer is alternately output to the enable channel of the DAC. */
} dac_conti_channel_mode_t;

/**
 * @brief DAC DMA (digitial controller) clock source
 *
 */
typedef soc_periph_dac_digi_clk_src_t       dac_conti_clk_src_t;

/**
 * @brief DAC cosine wave generator clock source
 *
 */
typedef soc_periph_dac_cosine_clk_src_t     dac_cosine_clk_src_t;

/**
 * @brief The attenuation of the amplitude of the cosine wave generator. The max amplitude is VDD3P3_RTC.
 */
typedef enum {
    DAC_COSINE_NO_ATTEN = 0x0,      /*!< No attenuation to the DAC cosine wave amplitude. Default. */
    DAC_COSINE_ATTEN_2  = 0x1,      /*!< 1/2 amplitude of the DAC cosine wave */
    DAC_COSINE_ATTEN_4  = 0x2,      /*!< 1/4 amplitude of the DAC cosine wave */
    DAC_COSINE_ATTEN_8  = 0x3,      /*!< 1/8 amplitude of the DAC cosine wave */
} dac_cosine_scale_t;

/**
 * @brief Set the phase of the cosine wave generator output.
 * @note  Only 0 or 180 are supported,
 *        it will be set to 0 as default if configured to an unsupported phase.
 */
typedef enum {
    DAC_COSINE_PHASE_0   = 0,   /*!< Phase shift +0° */
    DAC_COSINE_PHASE_180 = 180, /*!< Phase shift +180° */
} dac_cosine_phase_t;

#endif // SOC_DAC_SUPPORTED

#ifdef __cplusplus
}
#endif
