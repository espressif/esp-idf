/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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
#include "hal/dac_types.h"
#include "esp_bit_defs.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_DAC_SUPPORTED
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
} dac_continuous_channel_mode_t;

/**
 * @brief DAC DMA (digital controller) clock source
 *
 */
typedef soc_periph_dac_digi_clk_src_t       dac_continuous_digi_clk_src_t;

/**
 * @brief DAC cosine wave generator clock source
 *
 */
typedef soc_periph_dac_cosine_clk_src_t     dac_cosine_clk_src_t;

#endif // SOC_DAC_SUPPORTED

#ifdef __cplusplus
}
#endif
