/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "soc/soc_caps.h"
#include "hal/adc_types.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief DAC channel mask
 *
 */
typedef enum {
    DAC_CHANNEL_MASK_1 = BIT(0),    /*!< DAC channel 1 is GPIO25(ESP32) / GPIO17(ESP32S2) */
    DAC_CHANNEL_MASK_2 = BIT(1),    /*!< DAC channel 2 is GPIO26(ESP32) / GPIO18(ESP32S2) */
    DAC_CHANNEL_MASK_BOTH = BIT(0) | BIT(1),    /*!< DAC channel 2 is GPIO26(ESP32) / GPIO18(ESP32S2) */
} dac_channel_mask_t;

/**
 * @brief DAC channel work mode in dma mode
 * @note  Only take effect when multiple channels enabled.
 */
typedef enum {
    DAC_CHANNEL_SIMULTANEOUS,       /*!< The data in the DMA buffer is simultaneously output to the enable channel of the DAC. */
    DAC_CHANNEL_ALTERNATE,          /*!< The data in the DMA buffer is alternately output to the enable channel of the DAC. */
} dac_dma_channel_mode_t;

/**
 * @brief The multiple of the amplitude of the cosine wave generator. The max amplitude is VDD3P3_RTC.
 */
typedef enum {
    DAC_COSINE_SCALE_1     = 0x0,   /*!< No scaling to the DAC cosine wave amplitude. Default. */
    DAC_COSINE_SCALE_2     = 0x1,   /*!< 1/2 amplitude of the  DAC cosine wave */
    DAC_COSINE_SCALE_4     = 0x2,   /*!< 1/4 amplitude of the  DAC cosine wave */
    DAC_COSINE_SCALE_8     = 0x3,   /*!< 1/8 amplitude of the  DAC cosine wave */
} dac_cosine_scale_t;

/**
 * @brief Set the phase of the cosine wave generator output.
 */
typedef enum {
    DAC_COSINE_PHASE_0   = 0x2, /*!< Phase shift +0° */
    DAC_COSINE_PHASE_180 = 0x3, /*!< Phase shift +180° */
} dac_cosine_phase_t;

#ifdef __cplusplus
}
#endif
