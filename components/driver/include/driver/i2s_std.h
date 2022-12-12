/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * This file is specified for I2S standard communication mode
 * Features:
 *      - Philips/MSB/PCM are supported in standard mode
 *      - Fixed to 2 slots
 */
#pragma once

#include "hal/i2s_types.h"
#include "hal/gpio_types.h"
#include "driver/i2s_common.h"

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_IDF_TARGET_ESP32
/**
 * @brief Philips format in 2 slots
 * @param bits_per_sample i2s data bit width
 * @param mono_or_stereo I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
 */
#define I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo) { \
    .data_bit_width = bits_per_sample, \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO, \
    .slot_mode = mono_or_stereo, \
    .slot_mask = (mono_or_stereo == I2S_SLOT_MODE_MONO) ? \
                I2S_STD_SLOT_LEFT : I2S_STD_SLOT_BOTH, \
    .ws_width = bits_per_sample, \
    .ws_pol = false, \
    .bit_shift = true, \
    .msb_right = (bits_per_sample <= I2S_DATA_BIT_WIDTH_16BIT) ? \
                true : false, \
}

/**
 * @brief PCM(short) format in 2 slots
 * @note  PCM(long) is same as philips in 2 slots
 * @param bits_per_sample i2s data bit width
 * @param mono_or_stereo I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
 */
#define I2S_STD_PCM_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo)  { \
    .data_bit_width = bits_per_sample, \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO, \
    .slot_mode = mono_or_stereo, \
    .slot_mask = (mono_or_stereo == I2S_SLOT_MODE_MONO) ? \
                I2S_STD_SLOT_LEFT : I2S_STD_SLOT_BOTH, \
    .ws_width = 1, \
    .ws_pol = true, \
    .bit_shift = true, \
    .msb_right = (bits_per_sample <= I2S_DATA_BIT_WIDTH_16BIT) ? \
                true : false, \
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
    .slot_mask = (mono_or_stereo == I2S_SLOT_MODE_MONO) ? \
                I2S_STD_SLOT_LEFT : I2S_STD_SLOT_BOTH, \
    .ws_width = bits_per_sample, \
    .ws_pol = false, \
    .bit_shift = false, \
    .msb_right = (bits_per_sample <= I2S_DATA_BIT_WIDTH_16BIT) ? \
                true : false, \
}

#elif CONFIG_IDF_TARGET_ESP32S2
/**
 * @brief Philips format in 2 slots
 * @param bits_per_sample i2s data bit width
 * @param mono_or_stereo I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
 */
#define I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo) { \
    .data_bit_width = bits_per_sample, \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO, \
    .slot_mode = mono_or_stereo, \
    .slot_mask = (mono_or_stereo == I2S_SLOT_MODE_MONO) ? \
                I2S_STD_SLOT_LEFT : I2S_STD_SLOT_BOTH, \
    .ws_width = bits_per_sample, \
    .ws_pol = false, \
    .bit_shift = true, \
    .msb_right = true, \
}

/**
 * @brief PCM(short) format in 2 slots
 * @note  PCM(long) is same as philips in 2 slots
 * @param bits_per_sample i2s data bit width
 * @param mono_or_stereo I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
 */
#define I2S_STD_PCM_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo)  { \
    .data_bit_width = bits_per_sample, \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO, \
    .slot_mode = mono_or_stereo, \
    .slot_mask = (mono_or_stereo == I2S_SLOT_MODE_MONO) ? \
                I2S_STD_SLOT_LEFT : I2S_STD_SLOT_BOTH, \
    .ws_width = 1, \
    .ws_pol = true, \
    .bit_shift = true, \
    .msb_right = true, \
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
    .slot_mask = (mono_or_stereo == I2S_SLOT_MODE_MONO) ? \
                I2S_STD_SLOT_LEFT : I2S_STD_SLOT_BOTH, \
    .ws_width = bits_per_sample, \
    .ws_pol = false, \
    .bit_shift = false, \
    .msb_right = true, \
}

#else
/**
 * @brief Philips format in 2 slots
 * @param bits_per_sample i2s data bit width
 * @param mono_or_stereo I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
 */
#define I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo) { \
    .data_bit_width = bits_per_sample, \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO, \
    .slot_mode = mono_or_stereo, \
    .slot_mask = I2S_STD_SLOT_BOTH, \
    .ws_width = bits_per_sample, \
    .ws_pol = false, \
    .bit_shift = true, \
    .left_align = false, \
    .big_endian = false, \
    .bit_order_lsb = false \
}

/**
 * @brief PCM(short) format in 2 slots
 * @note  PCM(long) is same as philips in 2 slots
 * @param bits_per_sample i2s data bit width
 * @param mono_or_stereo I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
 */
#define I2S_STD_PCM_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo)  { \
    .data_bit_width = bits_per_sample, \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO, \
    .slot_mode = mono_or_stereo, \
    .slot_mask = I2S_STD_SLOT_BOTH, \
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
    .data_bit_width = bits_per_sample, \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO, \
    .slot_mode = mono_or_stereo, \
    .slot_mask = I2S_STD_SLOT_BOTH, \
    .ws_width = bits_per_sample, \
    .ws_pol = false, \
    .bit_shift = false, \
    .left_align = false, \
    .big_endian = false, \
    .bit_order_lsb = false \
}
#endif

/** @cond */
#define I2S_STD_PHILIP_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo) \
        I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo)  // Alias
/** @endcond */

/**
 * @brief i2s default standard clock configuration
 * @note Please set the mclk_multiple to I2S_MCLK_MULTIPLE_384 while using 24 bits data width
 *       Otherwise the sample rate might be imprecise since the bclk division is not a integer
 * @param rate sample rate
 */
#define I2S_STD_CLK_DEFAULT_CONFIG(rate) { \
    .sample_rate_hz = rate, \
    .clk_src = I2S_CLK_SRC_DEFAULT, \
    .mclk_multiple = I2S_MCLK_MULTIPLE_256, \
}

/**
 * @brief I2S slot configuration for standard mode
 */
typedef struct {
    /* General fields */
    i2s_data_bit_width_t    data_bit_width;     /*!< I2S sample data bit width (valid data bits per sample) */
    i2s_slot_bit_width_t    slot_bit_width;     /*!< I2S slot bit width (total bits per slot) */
    i2s_slot_mode_t         slot_mode;          /*!< Set mono or stereo mode with I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
                                                 *   In TX direction, mono means the written buffer contains only one slot data
                                                 *   and stereo means the written buffer contains both left and right data
                                                 */

    /* Particular fields */
    i2s_std_slot_mask_t     slot_mask;          /*!< Select the left, right or both slot */
    uint32_t                ws_width;           /*!< WS signal width (i.e. the number of bclk ticks that ws signal is high) */
    bool                    ws_pol;             /*!< WS signal polarity, set true to enable high lever first */
    bool                    bit_shift;          /*!< Set to enable bit shift in Philips mode */
#if SOC_I2S_HW_VERSION_1    // For esp32/esp32-s2
    bool                    msb_right;          /*!< Set to place right channel data at the MSB in the FIFO */
#else
    bool                    left_align;         /*!< Set to enable left alignment */
    bool                    big_endian;         /*!< Set to enable big endian */
    bool                    bit_order_lsb;      /*!< Set to enable lsb first */
#endif
} i2s_std_slot_config_t;

/**
 * @brief I2S clock configuration for standard mode
 */
typedef struct {
    /* General fields */
    uint32_t                sample_rate_hz;     /*!< I2S sample rate */
    i2s_clock_src_t         clk_src;            /*!< Choose clock source */
    i2s_mclk_multiple_t     mclk_multiple;      /*!< The multiple of mclk to the sample rate
                                                 *   Default is 256 in the helper macro, it can satisfy most of cases,
                                                 *   but please set this field a multiple of '3' (like 384) when using 24-bit data width,
                                                 *   otherwise the sample rate might be inaccurate
                                                 */
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
    struct {
        uint32_t   mclk_inv: 1;    /*!< Set 1 to invert the mclk output */
        uint32_t   bclk_inv: 1;    /*!< Set 1 to invert the bclk input/output */
        uint32_t   ws_inv: 1;      /*!< Set 1 to invert the ws input/output */
    } invert_flags;                /*!< GPIO pin invert flags */
} i2s_std_gpio_config_t;

/**
 * @brief I2S standard mode major configuration that including clock/slot/gpio configuration
 */
typedef struct {
    i2s_std_clk_config_t    clk_cfg;    /*!< Standard mode clock configuration, can be generated by macro I2S_STD_CLK_DEFAULT_CONFIG */
    i2s_std_slot_config_t   slot_cfg;   /*!< Standard mode slot configuration, can be generated by macros I2S_STD_[mode]_SLOT_DEFAULT_CONFIG, [mode] can be replaced with PHILIPS/MSB/PCM */
    i2s_std_gpio_config_t   gpio_cfg;   /*!< Standard mode gpio configuration, specified by user */
} i2s_std_config_t;

/**
 * @brief Initialize i2s channel to standard mode
 * @note  Only allowed to be called when the channel state is REGISTERED, (i.e., channel has been allocated, but not initialized)
 *        and the state will be updated to READY if initialization success, otherwise the state will return to REGISTERED.
 *
 * @param[in]   handle      I2S channel handler
 * @param[in]   std_cfg     Configurations for standard mode, including clock, slot and gpio
 *                          The clock configuration can be generated by the helper macro `I2S_STD_CLK_DEFAULT_CONFIG`
 *                          The slot configuration can be generated by the helper macro `I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG`,
 *                          `I2S_STD_PCM_SLOT_DEFAULT_CONFIG` or `I2S_STD_MSB_SLOT_DEFAULT_CONFIG`
 *
 * @return
 *      - ESP_OK    Initialize successfully
 *      - ESP_ERR_NO_MEM        No memory for storing the channel information
 *      - ESP_ERR_INVALID_ARG   NULL pointer or invalid configuration
 *      - ESP_ERR_INVALID_STATE This channel is not registered
 */
esp_err_t i2s_channel_init_std_mode(i2s_chan_handle_t handle, const i2s_std_config_t *std_cfg);

/**
 * @brief Reconfigure the I2S clock for standard mode
 * @note  Only allowed to be called when the channel state is READY, i.e., channel has been initialized, but not started
 *        this function won't change the state. 'i2s_channel_disable' should be called before calling this function if i2s has started.
 * @note  The input channel handle has to be initialized to standard mode, i.e., 'i2s_channel_init_std_mode' has been called before reconfigring
 *
 * @param[in]   handle      I2S channel handler
 * @param[in]   clk_cfg     Standard mode clock configuration, can be generated by `I2S_STD_CLK_DEFAULT_CONFIG`
 * @return
 *      - ESP_OK    Set clock successfully
 *      - ESP_ERR_INVALID_ARG   NULL pointer, invalid configuration or not standard mode
 *      - ESP_ERR_INVALID_STATE This channel is not initialized or not stopped
 */
esp_err_t i2s_channel_reconfig_std_clock(i2s_chan_handle_t handle, const i2s_std_clk_config_t *clk_cfg);

/**
 * @brief Reconfigure the I2S slot for standard mode
 * @note  Only allowed to be called when the channel state is READY, i.e., channel has been initialized, but not started
 *        this function won't change the state. 'i2s_channel_disable' should be called before calling this function if i2s has started.
 * @note  The input channel handle has to be initialized to standard mode, i.e., 'i2s_channel_init_std_mode' has been called before reconfigring
 *
 * @param[in]   handle      I2S channel handler
 * @param[in]   slot_cfg    Standard mode slot configuration, can be generated by `I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG`,
 *                          `I2S_STD_PCM_SLOT_DEFAULT_CONFIG` and `I2S_STD_MSB_SLOT_DEFAULT_CONFIG`.
 * @return
 *      - ESP_OK    Set clock successfully
 *      - ESP_ERR_NO_MEM        No memory for DMA buffer
 *      - ESP_ERR_INVALID_ARG   NULL pointer, invalid configuration  or not standard mode
 *      - ESP_ERR_INVALID_STATE This channel is not initialized or not stopped
 */
esp_err_t i2s_channel_reconfig_std_slot(i2s_chan_handle_t handle, const i2s_std_slot_config_t *slot_cfg);

/**
 * @brief Reconfigure the I2S gpio for standard mode
 * @note  Only allowed to be called when the channel state is READY, i.e., channel has been initialized, but not started
 *        this function won't change the state. 'i2s_channel_disable' should be called before calling this function if i2s has started.
 * @note  The input channel handle has to be initialized to standard mode, i.e., 'i2s_channel_init_std_mode' has been called before reconfigring
 *
 * @param[in]   handle      I2S channel handler
 * @param[in]   gpio_cfg    Standard mode gpio configuration, specified by user
 * @return
 *      - ESP_OK    Set clock successfully
 *      - ESP_ERR_INVALID_ARG   NULL pointer, invalid configuration  or not standard mode
 *      - ESP_ERR_INVALID_STATE This channel is not initialized or not stopped
 */
esp_err_t i2s_channel_reconfig_std_gpio(i2s_chan_handle_t handle, const i2s_std_gpio_config_t *gpio_cfg);

#ifdef __cplusplus
}
#endif
