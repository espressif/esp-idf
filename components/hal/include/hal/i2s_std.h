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
    .mode = I2S_COMM_MODE_STD, \
    .data_bit_width = bits_per_sample, \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_DEFAULT, \
    .slot_mode = mono_or_stereo, \
    .ws_width = bits_per_sample, \
    .ws_pol = false, \
    .bit_shift = true, \
    .msb_right = false, \
}

/**
 * @brief PCM(short) format in 2 slots
 * @note  PCM(long) is sample as philip in 2 slots
 * @param bits_per_sample i2s data bit width
 * @param mono_or_stereo I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
 */
#define I2S_STD_PCM_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo)  { \
    .mode = I2S_COMM_MODE_STD, \
    .data_bit_width = bits_per_sample, \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_DEFAULT, \
    .slot_mode = mono_or_stereo, \
    .ws_width = 1, \
    .ws_pol = true, \
    .bit_shift = true, \
    .msb_right = false, \
}

/**
 * @brief MSB format in 2 slots
 * @param bits_per_sample i2s data bit width
 * @param mono_or_stereo I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
 */
#define I2S_STD_MSB_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo) { \
    .mode = I2S_COMM_MODE_STD, \
    .data_bit_width = bits_per_sample, \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_DEFAULT, \
    .slot_mode = mono_or_stereo, \
    .ws_width = bits_per_sample, \
    .ws_pol = false, \
    .bit_shift = false, \
    .msb_right = false, \
}

#else
/**
 * @brief Philip format in 2 slots
 * @param bits_per_sample i2s data bit width
 * @param mono_or_stereo I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
 */
#define I2S_STD_PHILIP_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo) { \
    .mode = I2S_COMM_MODE_STD, \
    .data_bit_width = bits_per_sample, \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_DEFAULT, \
    .slot_mode = mono_or_stereo, \
    .ws_width = bits_per_sample, \
    .ws_pol = false, \
    .bit_shift = true, \
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
    .mode = I2S_COMM_MODE_STD, \
    .data_bit_width = bits_per_sample, \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_DEFAULT, \
    .slot_mode = mono_or_stereo, \
    .ws_width = 1, \
    .ws_pol = true, \
    .bit_shift = true, \
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
    .mode = I2S_COMM_MODE_STD, \
    .data_bit_width = bits_per_sample, \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_DEFAULT, \
    .slot_mode = mono_or_stereo, \
    .ws_width = bits_per_sample, \
    .ws_pol = false, \
    .bit_shift = false, \
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
    .clk_src = I2S_CLK_D2CLK, \
    .mclk_multiple = I2S_MCLK_MULTIPLE_256, \
}

/**
 * @breif I2S slot configuration for standard mode
 */
typedef struct {
    /* General fields */
    i2s_comm_mode_t         mode;               /*!< I2S communication mode, this field is for identification (MUST match the communication mode in 'i2s_chan_config_t') */
    i2s_data_bit_width_t    data_bit_width;     /*!< I2S sample data bit width (valid data bits per sample) */
    i2s_slot_bit_width_t    slot_bit_width;     /*!< I2S slot bit width (total bits per slot) */
    i2s_slot_mode_t         slot_mode;          /*!< Set mono or stereo mode with I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO */

    /* Particular fields */
    uint32_t                ws_width;           /*!< WS signal width (i.e. the number of bclk ticks that ws signal is high) */
    bool                    ws_pol;             /*!< WS signal polarity, set true to enable high lever first */
    bool                    bit_shift;          /*!< Set to enbale bit shift in Philip mode */
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

#ifdef __cplusplus
}
#endif
