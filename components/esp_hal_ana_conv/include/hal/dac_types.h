/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_assert.h"
#include "esp_bit_defs.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_DAC_SUPPORTED

/**
 * ESP32:
 * - DAC channel 0: GPIO25
 * - DAC channel 1: GPIO26
 * ESP32S2:
 * - DAC channel 0: GPIO17
 * - DAC channel 1: GPIO18
 */

typedef enum {
    DAC_CHAN_0 = 0,
    DAC_CHAN_1 = 1,
    DAC_CHAN_MAX,       /*!< For checking purpose */
} dac_channel_t;

ESP_STATIC_ASSERT(DAC_CHAN_MAX == SOC_DAC_CHAN_NUM, "DAC channel number mismatch");

#define IS_VALID_DAC_CHANNEL(channel) ((uint32_t)(channel) < (uint32_t)DAC_CHAN_MAX)

typedef uint32_t dac_channel_mask_t;

#define DAC_CHANNEL_MASK_CH0    (BIT(0))                           /*!< DAC channel 0 mask */
#define DAC_CHANNEL_MASK_CH1    (BIT(1))                           /*!< DAC channel 1 mask */
#define DAC_CHANNEL_MASK_ALL    ((1ULL << SOC_DAC_CHAN_NUM) - 1)   /*!< Bitwise OR of all valid channel masks */

/**
 * @note 0 is valid
 */
#define IS_VALID_DAC_CHANNEL_MASK(mask) (((mask) & ~DAC_CHANNEL_MASK_ALL) == 0)

/**
 * @brief Use a loop to extract every channel (dac_channel_t) from the mask (dac_channel_mask_t).
 */
#define DAC_CHANNEL_MASK_FOREACH(channel, mask) \
    for (uint32_t __dac_mask = (mask), __dac_chan = DAC_CHAN_0; __dac_chan < DAC_CHAN_MAX; __dac_chan++) \
        for (dac_channel_t channel = (dac_channel_t)__dac_chan; __dac_mask & BIT(__dac_chan); __dac_mask &= ~BIT(__dac_chan))

/**
 * @brief The attenuation of the amplitude of the cosine wave generator. The max amplitude is VDD3P3_RTC.
 */
typedef enum {
    DAC_COSINE_ATTEN_DEFAULT    = 0x0,      /*!< No attenuation to the DAC cosine wave amplitude. Default. */
    DAC_COSINE_ATTEN_DB_0       = 0x0,      /*!< Original amplitude of the DAC cosine wave, equals to DAC_COSINE_ATTEN_DEFAULT */
    DAC_COSINE_ATTEN_DB_6       = 0x1,      /*!< 1/2 amplitude of the DAC cosine wave */
    DAC_COSINE_ATTEN_DB_12      = 0x2,      /*!< 1/4 amplitude of the DAC cosine wave */
    DAC_COSINE_ATTEN_DB_18      = 0x3,      /*!< 1/8 amplitude of the DAC cosine wave */
} dac_cosine_atten_t;

/**
 * @brief Set the phase of the cosine wave generator output.
 * @note  Only 0 or 180 are supported,
 *        it will be set to 0 as default if configured to an unsupported phase.
 */
typedef enum {
    DAC_COSINE_PHASE_0   = 0x02,   /*!< Phase shift +0° */
    DAC_COSINE_PHASE_180 = 0x03, /*!< Phase shift +180° */
} dac_cosine_phase_t;

#endif // SOC_DAC_SUPPORTED

#ifdef __cplusplus
}
#endif
