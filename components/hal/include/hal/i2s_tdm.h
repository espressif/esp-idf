/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * This file is specified for I2S TDM communication mode
 * Features:
 *      - More than 2 slots
 */
#pragma once

#include "hal/i2s_types.h"

#if SOC_I2S_SUPPORTS_TDM

#ifdef __cplusplus
extern "C" {
#endif

#define I2S_TDM_AUTO_SLOT_NUM    (0)       // Auto means total slot number will be equal to the maximum active slot number
#define I2S_TDM_AUTO_WS_WIDTH    (0)       // Auto means ws signal width will be equal to the half width of a frame

/**
 * @brief Philip format in active slot that enabled by mask
 * @param bits_per_sample i2s data bit width
 * @param mono_or_stereo I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
 * @param mask active slot mask
 */
#define I2S_TDM_PHILIP_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo, mask) { \
    .mode = I2S_COMM_MODE_TDM, \
    .data_bit_width = (bits_per_sample), \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_DEFAULT, \
    .slot_mode = mono_or_stereo, \
    .ws_width = I2S_TDM_AUTO_WS_WIDTH, \
    .ws_pol = false, \
    .bit_shift = true, \
    .left_align = false, \
    .big_endian = false, \
    .bit_order_lsb = false, \
    .skip_mask = false, \
    .slot_mask = (mask), \
    .total_slot = I2S_TDM_AUTO_SLOT_NUM \
}

/**
 * @brief MSB format in active slot enabled that by mask
 * @param bits_per_sample i2s data bit width
 * @param mono_or_stereo I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
 * @param mask active slot mask
 */
#define I2S_TDM_MSB_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo, mask) { \
    .mode = I2S_COMM_MODE_TDM, \
    .data_bit_width = (bits_per_sample), \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_DEFAULT, \
    .slot_mode = mono_or_stereo, \
    .ws_width = I2S_TDM_AUTO_WS_WIDTH, \
    .ws_pol = false, \
    .bit_shift = false, \
    .left_align = false, \
    .big_endian = false, \
    .bit_order_lsb = false, \
    .skip_mask = false ,\
    .slot_mask = (mask), \
    .total_slot = I2S_TDM_AUTO_SLOT_NUM \
}

/**
 * @brief PCM(short) format in active slot that enabled by mask
 * @param bits_per_sample i2s data bit width
 * @param mono_or_stereo I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
 * @param mask active slot mask
 */
#define I2S_TDM_PCM_SHORT_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo, mask) { \
    .mode = I2S_COMM_MODE_TDM, \
    .data_bit_width = (bits_per_sample), \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_DEFAULT, \
    .slot_mode = mono_or_stereo, \
    .ws_width = 1, \
    .ws_pol = true, \
    .bit_shift = true, \
    .left_align = false, \
    .big_endian = false, \
    .bit_order_lsb = false, \
    .skip_mask = false, \
    .slot_mask = (mask), \
    .total_slot = I2S_TDM_AUTO_SLOT_NUM \
}

/**
 * @brief PCM(long) format in active slot that enabled by mask
 * @param bits_per_sample i2s data bit width
 * @param mono_or_stereo I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
 * @param mask active slot mask
 */
#define I2S_TDM_PCM_LONG_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo, mask) { \
    .mode = I2S_COMM_MODE_TDM, \
    .data_bit_width = (bits_per_sample), \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_DEFAULT, \
    .slot_mode = mono_or_stereo, \
    .ws_width = (bits_per_sample), \
    .ws_pol = true, \
    .bit_shift = true, \
    .left_align = false, \
    .big_endian = false, \
    .bit_order_lsb = false, \
    .skip_mask = false, \
    .slot_mask = (mask), \
    .total_slot = I2S_TDM_AUTO_SLOT_NUM \
}

/**
 * @brief i2s default tdm clock configuration
 * @note Please set the mclk_multiple to I2S_MCLK_MULTIPLE_384 while the data width in slot configuration is set to 24 bits
 *       Otherwise the sample rate might be imprecise since the bclk division is not a integer
 * @param rate sample rate
 */
#define I2S_TDM_CLK_DEFAULT_CONFIG(rate) { \
    .sample_rate_hz = rate, \
    .clk_src = I2S_CLK_D2CLK, \
    .mclk_multiple = I2S_MCLK_MULTIPLE_256, \
}

/**
 * @breif I2S slot configuration for tdm mode
 */
typedef struct {
    /* General fields */
    i2s_comm_mode_t         mode;               /*!< I2S communication mode, this field is for identification (MUST match the communication mode in 'i2s_chan_config_t') */
    i2s_data_bit_width_t    data_bit_width;     /*!< I2S sample data bit width (valid data bits per sample) */
    i2s_slot_bit_width_t    slot_bit_width;     /*!< I2S slot bit width (total bits per slot) */
    i2s_slot_mode_t         slot_mode;          /*!< Set mono or stereo mode with I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO */

    /* Particular fields */
    uint32_t                ws_width;           /*!< WS signal width ((i.e. the number of bclk ticks that ws signal is high)) */
    bool                    ws_pol;             /*!< WS signal polarity, set true to enable high lever first */
    bool                    bit_shift;          /*!< Set true to enable bit shift in Philip mode */

    bool                    left_align;         /*!< Set true to enable left alignment */
    bool                    big_endian;         /*!< Set true to enable big endian */
    bool                    bit_order_lsb;      /*!< Set true to enable lsb first */

    bool                    skip_mask;          /*!< Set true to enable skip mask. If it is enabled, only the data of the enabled channels will be sent, otherwise all data stored in DMA TX buffer will be sent */
    i2s_tdm_slot_mask_t     slot_mask;          /*!< Slot mask. Activating slots by setting 1 to corresponding bits. When the activated slots is not consecutive, those data in unactivated slots will be ignored */
    uint32_t                total_slot;         /*!< I2S total number of slots. If it is smaller than the biggest activated channel number, it will be set to this number automatically. */
} i2s_tdm_slot_config_t;

/**
 * @breif I2S clock configuration for tdm mode
 */
typedef struct {
    /* General fields */
    uint32_t                sample_rate_hz;     /*!< I2S sample rate */
    i2s_clock_src_t         clk_src;            /*!< Choose clock source */
    i2s_mclk_multiple_t     mclk_multiple;      /*!< The multiple of mclk to the sample rate */
} i2s_tdm_clk_config_t;

#ifdef __cplusplus
}
#endif

#endif // SOC_I2S_SUPPORTS_TDM
