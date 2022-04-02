/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * This file is specified for I2S standard communication mode
 * Features:
 *      - Philip/MSB/PCM are supported in standard mode
 *      - Fixed to 2 slots
 */
#pragma once

#include "hal/i2s_types.h"
#include "hal/gpio_types.h"
#include "driver/i2s_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_I2S_HW_VERSION_1 // For esp32/esp32-s2
/**
 * @brief Philip format in 2 slots
 * @param bits_per_sample i2s data bit width
 * @param mono_or_stereo I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
 */
#define I2S_STD_PHILIP_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo) { \
    .data_bit_width = bits_per_sample, \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO, \
    .slot_mode = mono_or_stereo, \
    .ws_width = bits_per_sample, \
    .ws_pol = false, \
    .bit_shift = true, \
    .slot_sel = (mono_or_stereo == I2S_SLOT_MODE_MONO) ? \
                I2S_STD_SLOT_ONLY_LEFT : I2S_STD_SLOT_LEFT_RIGHT, \
    .msb_right = false, \
}

/**
 * @brief PCM(short) format in 2 slots
 * @note  PCM(long) is sample as philip in 2 slots
 * @param bits_per_sample i2s data bit width
 * @param mono_or_stereo I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
 */
#define I2S_STD_PCM_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo)  { \
    .data_bit_width = bits_per_sample, \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO, \
    .slot_mode = mono_or_stereo, \
    .ws_width = 1, \
    .ws_pol = true, \
    .bit_shift = true, \
    .slot_sel = (mono_or_stereo == I2S_SLOT_MODE_MONO) ? \
                I2S_STD_SLOT_ONLY_LEFT : I2S_STD_SLOT_LEFT_RIGHT, \
    .msb_right = false, \
}

/**
 * @brief MSB format in 2 slots
 * @param bits_per_sample i2s data bit width
 * @param mono_or_stereo I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
 */
#define I2S_STD_MSB_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo) { \
    .data_bit_width = bits_per_sample, \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO, \
    .slot_mode = mono_or_stereo, \
    .ws_width = bits_per_sample, \
    .ws_pol = false, \
    .bit_shift = false, \
    .slot_sel = (mono_or_stereo == I2S_SLOT_MODE_MONO) ? \
                I2S_STD_SLOT_ONLY_LEFT : I2S_STD_SLOT_LEFT_RIGHT, \
    .msb_right = false, \
}

#else
/**
 * @brief Philip format in 2 slots
 * @param bits_per_sample i2s data bit width
 * @param mono_or_stereo I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
 */
#define I2S_STD_PHILIP_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo) { \
    .data_bit_width = bits_per_sample, \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO, \
    .slot_mode = mono_or_stereo, \
    .ws_width = bits_per_sample, \
    .ws_pol = false, \
    .bit_shift = true, \
    .slot_sel = (mono_or_stereo == I2S_SLOT_MODE_MONO) ? \
                I2S_STD_SLOT_ONLY_LEFT : I2S_STD_SLOT_LEFT_RIGHT, \
    .left_align = false, \
    .big_endian = false, \
    .bit_order_lsb = false \
}

/**
 * @brief PCM(short) format in 2 slots
 * @note  PCM(long) is sample as philip in 2 slots
 * @param bits_per_sample i2s data bit width
 * @param mono_or_stereo I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
 */
#define I2S_STD_PCM_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo)  { \
    .data_bit_width = bits_per_sample, \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO, \
    .slot_mode = mono_or_stereo, \
    .ws_width = 1, \
    .ws_pol = true, \
    .bit_shift = true, \
    .slot_sel = (mono_or_stereo == I2S_SLOT_MODE_MONO) ? \
                I2S_STD_SLOT_ONLY_LEFT : I2S_STD_SLOT_LEFT_RIGHT, \
    .left_align = false, \
    .big_endian = false, \
    .bit_order_lsb = false \
}

/**
 * @brief MSB format in 2 slots
 * @param bits_per_sample i2s data bit width
 * @param mono_or_stereo I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
 */
#define I2S_STD_MSB_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo) { \
    .data_bit_width = bits_per_sample, \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO, \
    .slot_mode = mono_or_stereo, \
    .ws_width = bits_per_sample, \
    .ws_pol = false, \
    .bit_shift = false, \
    .slot_sel = (mono_or_stereo == I2S_SLOT_MODE_MONO) ? \
                I2S_STD_SLOT_ONLY_LEFT : I2S_STD_SLOT_LEFT_RIGHT, \
    .left_align = false, \
    .big_endian = false, \
    .bit_order_lsb = false \
}
#endif

/**
 * @brief i2s default standard clock configuration
 * @note Please set the mclk_multiple to I2S_MCLK_MULTIPLE_384 while using 24 bits data width
 *       Otherwise the sample rate might be imprecise since the bclk division is not a integer
 * @param rate sample rate
 */
#define I2S_STD_CLK_DEFAULT_CONFIG(rate) { \
    .sample_rate_hz = rate, \
    .clk_src = I2S_CLK_PLL_160M, \
    .mclk_multiple = I2S_MCLK_MULTIPLE_256, \
}

/**
 * @breif I2S slot configuration for standard mode
 */
typedef struct {
    /* General fields */
    i2s_data_bit_width_t    data_bit_width;     /*!< I2S sample data bit width (valid data bits per sample) */
    i2s_slot_bit_width_t    slot_bit_width;     /*!< I2S slot bit width (total bits per slot) */
    i2s_slot_mode_t         slot_mode;          /*!< Set mono or stereo mode with I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO */

    /* Particular fields */
    uint32_t                ws_width;           /*!< WS signal width (i.e. the number of bclk ticks that ws signal is high) */
    bool                    ws_pol;             /*!< WS signal polarity, set true to enable high lever first */
    bool                    bit_shift;          /*!< Set to enbale bit shift in Philip mode */
    i2s_std_slot_sel_t      slot_sel;           /*!< Select the left, right or both slot */
#if SOC_I2S_HW_VERSION_1    // For esp32/esp32-s2
    bool                    msb_right;          /*!< Set to place right channel data at the MSB in the FIFO */
#else
    bool                    left_align;         /*!< Set to enable left alignment */
    bool                    big_endian;         /*!< Set to enable big endian */
    bool                    bit_order_lsb;      /*!< Set to enable lsb first */
#endif
} i2s_std_slot_config_t;

/**
 * @breif I2S clock configuration for standard mode
 */
typedef struct {
    /* General fields */
    uint32_t                sample_rate_hz;     /*!< I2S sample rate */
    i2s_clock_src_t         clk_src;            /*!< Choose clock source */
    i2s_mclk_multiple_t     mclk_multiple;      /*!< The multiple of mclk to the sample rate */
} i2s_std_clk_config_t;

/**
 * @brief I2S standard mode GPIO pins configuration
 */
typedef struct {
    gpio_num_t mclk;               /*!< MCK pin, output */
    gpio_num_t bclk;               /*!< BCK pin, input in slave role, output in master role */
    gpio_num_t ws;                 /*!< WS pin, input in slave role, output in master role */
    gpio_num_t dout;               /*!< DATA pin, output */
    gpio_num_t din;                /*!< DATA pin, input */
} i2s_std_gpio_config_t;

typedef struct {
    i2s_std_clk_config_t    clk_cfg;    /*!< Standard mode clock configuration */
    i2s_std_slot_config_t   slot_cfg;   /*!< Standard mode slot configuration */
    i2s_std_gpio_config_t   gpio_cfg;   /*!< Standard mode gpio configuration */
} i2s_std_config_t;

esp_err_t i2s_init_std_channel(i2s_chan_handle_t handle, const i2s_std_config_t *std_cfg);

esp_err_t i2s_reconfig_std_clock(i2s_chan_handle_t handle, const i2s_std_clk_config_t *clk_cfg);

esp_err_t i2s_reconfig_std_slot(i2s_chan_handle_t handle, const i2s_std_slot_config_t *slot_cfg);

esp_err_t i2s_reconfig_std_gpio(i2s_chan_handle_t handle, const i2s_std_gpio_config_t *gpio_cfg);

#ifdef __cplusplus
}
#endif
