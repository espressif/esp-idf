/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
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
 * @brief DAC channel output mode
 */
typedef enum {
    DAC_CHANNEL_OUTPUT_VOLTAGE = 0,       /*!< Voltage-output (Low-speed) DAC. Default. */
#if SOC_DAC_HS_RESOLUTION
    DAC_CHANNEL_OUTPUT_CURRENT,          /*!< Current-output (High-speed) DAC. Requires a small resistance load. */
#endif
} dac_channel_output_mode_t;

/**
 * @brief DAC analog buffer mode (voltage-output mode only).
 */
typedef enum {
    DAC_CHANNEL_BUFFER_FULL_SWING = 0, /*!< Analog buffer enabled, using a wider output voltage swing.
                                            But linearity may degrade near the limits, and saturation or
                                            clipping may occur. */
#if SOC_DAC_SUPPORT_BUFFER_CFG
    DAC_CHANNEL_BUFFER_REDUCED_SWING,  /*!< Analog buffer enabled, using a reduced output voltage swing to
                                            support linear operation over the full input code range.  */
    DAC_CHANNEL_BUFFER_BYPASS,         /*!< Analog buffer bypassed. Requires a very high-impedance load, but
                                            draws less current. */
#endif
} dac_channel_buffer_mode_t;

/**
 * @brief DAC channel (analog pad) configuration
 *
 * @note  ESP32 and ESP32-S2 only support the default configuration (voltage-output/low-speed DAC, full-swing buffer), i.e. zero-initialized values.
 *        ESP32-S31 supports configurable output and buffer modes.
 */
typedef struct {
    dac_channel_output_mode_t   output_mode;   /*!< Output mode: voltage-output or current-output */
    dac_channel_buffer_mode_t   buffer_mode;   /*!< Analog buffer mode */
} dac_channel_config_t;

/**
 * @brief DAC channel work mode in dma mode
 * @note  Only take effect when multiple channels enabled.
 * @note  Assume the samples in buffer are 'A B C D E F'
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
