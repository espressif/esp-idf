/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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
#include "hal/gpio_types.h"
#include "driver/i2s_common.h"

#if SOC_I2S_SUPPORTS_TDM

#ifdef __cplusplus
extern "C" {
#endif

#define I2S_TDM_AUTO_SLOT_NUM    (0)       // Auto means total slot number will be equal to the maximum active slot number
#define I2S_TDM_AUTO_WS_WIDTH    (0)       // Auto means WS signal width will be equal to the half width of a frame

/**
 * @brief Philips format in active slot that enabled by mask
 * @param bits_per_sample I2S data bit width
 * @param mono_or_stereo I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
 * @param mask active slot mask
 */
#define I2S_TDM_PHILIPS_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo, mask) { \
    .data_bit_width = (bits_per_sample), \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO, \
    .slot_mode = mono_or_stereo, \
    .slot_mask = (mask), \
    .ws_width = I2S_TDM_AUTO_WS_WIDTH, \
    .ws_pol = false, \
    .bit_shift = true, \
    .left_align = false, \
    .big_endian = false, \
    .bit_order_lsb = false, \
    .skip_mask = false, \
    .total_slot = I2S_TDM_AUTO_SLOT_NUM \
}

/**
 * @brief MSB format in active slot enabled that by mask
 * @param bits_per_sample I2S data bit width
 * @param mono_or_stereo I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
 * @param mask active slot mask
 */
#define I2S_TDM_MSB_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo, mask) { \
    .data_bit_width = (bits_per_sample), \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO, \
    .slot_mode = mono_or_stereo, \
    .slot_mask = (mask), \
    .ws_width = I2S_TDM_AUTO_WS_WIDTH, \
    .ws_pol = false, \
    .bit_shift = false, \
    .left_align = false, \
    .big_endian = false, \
    .bit_order_lsb = false, \
    .skip_mask = false ,\
    .total_slot = I2S_TDM_AUTO_SLOT_NUM \
}

/**
 * @brief PCM(short) format in active slot that enabled by mask
 * @param bits_per_sample I2S data bit width
 * @param mono_or_stereo I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
 * @param mask active slot mask
 */
#define I2S_TDM_PCM_SHORT_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo, mask) { \
    .data_bit_width = (bits_per_sample), \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO, \
    .slot_mode = mono_or_stereo, \
    .slot_mask = (mask), \
    .ws_width = 1, \
    .ws_pol = true, \
    .bit_shift = true, \
    .left_align = false, \
    .big_endian = false, \
    .bit_order_lsb = false, \
    .skip_mask = false, \
    .total_slot = I2S_TDM_AUTO_SLOT_NUM \
}

/**
 * @brief PCM(long) format in active slot that enabled by mask
 * @param bits_per_sample I2S data bit width
 * @param mono_or_stereo I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
 * @param mask active slot mask
 */
#define I2S_TDM_PCM_LONG_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo, mask) { \
    .data_bit_width = (bits_per_sample), \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO, \
    .slot_mode = mono_or_stereo, \
    .slot_mask = (mask), \
    .ws_width = (bits_per_sample), \
    .ws_pol = true, \
    .bit_shift = true, \
    .left_align = false, \
    .big_endian = false, \
    .bit_order_lsb = false, \
    .skip_mask = false, \
    .total_slot = I2S_TDM_AUTO_SLOT_NUM \
}

/** @cond */
#define I2S_TDM_PHILIP_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo, mask)  \
        I2S_TDM_PHILIPS_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo, mask)  // Alias
/** @endcond */

/**
 * @brief I2S default TDM clock configuration
 * @note Please set the mclk_multiple to I2S_MCLK_MULTIPLE_384 while the data width in slot configuration is set to 24 bits
 *       Otherwise the sample rate might be imprecise since the BCLK division is not a integer
 * @param rate sample rate
 */
#define I2S_TDM_CLK_DEFAULT_CONFIG(rate) { \
    .sample_rate_hz = rate, \
    .clk_src = I2S_CLK_SRC_DEFAULT, \
    .ext_clk_freq_hz = 0, \
    .mclk_multiple = I2S_MCLK_MULTIPLE_256, \
    .bclk_div = 8, \
}

/**
 * @brief I2S slot configuration for TDM mode
 */
typedef struct {
    /* General fields */
    i2s_data_bit_width_t    data_bit_width;     /*!< I2S sample data bit width (valid data bits per sample) */
    i2s_slot_bit_width_t    slot_bit_width;     /*!< I2S slot bit width (total bits per slot) */
    i2s_slot_mode_t         slot_mode;          /*!< Set mono or stereo mode with I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO */

    /* Particular fields */
    i2s_tdm_slot_mask_t     slot_mask;          /*!< Slot mask. Activating slots by setting 1 to corresponding bits. When the activated slots is not consecutive, those data in inactivated slots will be ignored */
    uint32_t                ws_width;           /*!< WS signal width (i.e. the number of BCLK ticks that WS signal is high) */
    bool                    ws_pol;             /*!< WS signal polarity, set true to enable high lever first */
    bool                    bit_shift;          /*!< Set true to enable bit shift in Philips mode */

    bool                    left_align;         /*!< Set true to enable left alignment */
    bool                    big_endian;         /*!< Set true to enable big endian */
    bool                    bit_order_lsb;      /*!< Set true to enable lsb first */

    bool                    skip_mask;          /*!< Set true to enable skip mask. If it is enabled, only the data of the enabled channels will be sent, otherwise all data stored in DMA TX buffer will be sent */
    uint32_t                total_slot;         /*!< I2S total number of slots. If it is smaller than the biggest activated channel number, it will be set to this number automatically. */
} i2s_tdm_slot_config_t;

/**
 * @brief I2S clock configuration for TDM mode
 */
typedef struct {
    /* General fields */
    uint32_t                sample_rate_hz;     /*!< I2S sample rate */
    i2s_clock_src_t         clk_src;            /*!< Choose clock source, see `soc_periph_i2s_clk_src_t` for the supported clock sources.
                                                 *   selected `I2S_CLK_SRC_EXTERNAL` (if supports) to enable the external source clock inputted via MCLK pin,
                                                 *   please make sure the frequency inputted is equal or greater than `sample_rate_hz * mclk_multiple`
                                                 */
    uint32_t                ext_clk_freq_hz;    /*!< External clock source frequency in Hz, only take effect when `clk_src = I2S_CLK_SRC_EXTERNAL`, otherwise this field will be ignored
                                                 *   Please make sure the frequency inputted is equal or greater than BCLK, i.e. `sample_rate_hz * slot_bits * slot_num`
                                                 */
    i2s_mclk_multiple_t     mclk_multiple;      /*!< The multiple of MCLK to the sample rate, only take effect for master role */
    uint32_t                bclk_div;           /*!< The division from MCLK to BCLK, only take effect for slave role, it shouldn't be smaller than 8. Increase this field when data sent by slave lag behind */
} i2s_tdm_clk_config_t;

/**
 * @brief I2S TDM mode GPIO pins configuration
 */
typedef struct {
    gpio_num_t mclk;               /*!< MCK pin, output by default, input if the clock source is selected to `I2S_CLK_SRC_EXTERNAL` */
    gpio_num_t bclk;               /*!< BCK pin, input in slave role, output in master role */
    gpio_num_t ws;                 /*!< WS pin, input in slave role, output in master role */
    gpio_num_t dout;               /*!< DATA pin, output */
    gpio_num_t din;                /*!< DATA pin, input */
    struct {
        uint32_t   mclk_inv: 1;    /*!< Set 1 to invert the MCLK input/output */
        uint32_t   bclk_inv: 1;    /*!< Set 1 to invert the BCLK input/output */
        uint32_t   ws_inv: 1;      /*!< Set 1 to invert the WS input/output */
    } invert_flags;                /*!< GPIO pin invert flags */
} i2s_tdm_gpio_config_t;

/**
 * @brief I2S TDM mode major configuration that including clock/slot/GPIO configuration
 */
typedef struct {
    i2s_tdm_clk_config_t    clk_cfg;    /*!< TDM mode clock configuration, can be generated by macro I2S_TDM_CLK_DEFAULT_CONFIG */
    i2s_tdm_slot_config_t   slot_cfg;   /*!< TDM mode slot configuration, can be generated by macros I2S_TDM_[mode]_SLOT_DEFAULT_CONFIG, [mode] can be replaced with PHILIPS/MSB/PCM_SHORT/PCM_LONG */
    i2s_tdm_gpio_config_t   gpio_cfg;   /*!< TDM mode GPIO configuration, specified by user */
} i2s_tdm_config_t;

/**
 * @brief Initialize I2S channel to TDM mode
 * @note  Only allowed to be called when the channel state is REGISTERED, (i.e., channel has been allocated, but not initialized)
 *        and the state will be updated to READY if initialization success, otherwise the state will return to REGISTERED.
 *
 * @param[in]   handle      I2S channel handler
 * @param[in]   tdm_cfg     Configurations for TDM mode, including clock, slot and GPIO
 *                          The clock configuration can be generated by the helper macro `I2S_TDM_CLK_DEFAULT_CONFIG`
 *                          The slot configuration can be generated by the helper macro `I2S_TDM_PHILIPS_SLOT_DEFAULT_CONFIG`,
 *                          `I2S_TDM_PCM_SHORT_SLOT_DEFAULT_CONFIG`, `I2S_TDM_PCM_LONG_SLOT_DEFAULT_CONFIG` or `I2S_TDM_MSB_SLOT_DEFAULT_CONFIG`
 *
 * @return
 *      - ESP_OK    Initialize successfully
 *      - ESP_ERR_NO_MEM        No memory for storing the channel information
 *      - ESP_ERR_INVALID_ARG   NULL pointer or invalid configuration
 *      - ESP_ERR_INVALID_STATE This channel is not registered
 */
esp_err_t i2s_channel_init_tdm_mode(i2s_chan_handle_t handle, const i2s_tdm_config_t *tdm_cfg);

/**
 * @brief Reconfigure the I2S clock for TDM mode
 * @note  Only allowed to be called when the channel state is READY, i.e., channel has been initialized, but not started
 *        this function won't change the state. `i2s_channel_disable` should be called before calling this function if I2S has started.
 * @note  The input channel handle has to be initialized to TDM mode, i.e., `i2s_channel_init_tdm_mode` has been called before reconfiguring
 *
 * @param[in]   handle      I2S channel handler
 * @param[in]   clk_cfg     Standard mode clock configuration, can be generated by `I2S_TDM_CLK_DEFAULT_CONFIG`
 * @return
 *      - ESP_OK    Set clock successfully
 *      - ESP_ERR_INVALID_ARG   NULL pointer, invalid configuration or not TDM mode
 *      - ESP_ERR_INVALID_STATE This channel is not initialized or not stopped
 */
esp_err_t i2s_channel_reconfig_tdm_clock(i2s_chan_handle_t handle, const i2s_tdm_clk_config_t *clk_cfg);

/**
 * @brief Reconfigure the I2S slot for TDM mode
 * @note  Only allowed to be called when the channel state is READY, i.e., channel has been initialized, but not started
 *        this function won't change the state. `i2s_channel_disable` should be called before calling this function if I2S has started.
 * @note  The input channel handle has to be initialized to TDM mode, i.e., `i2s_channel_init_tdm_mode` has been called before reconfiguring
 *
 * @param[in]   handle      I2S channel handler
 * @param[in]   slot_cfg    Standard mode slot configuration, can be generated by `I2S_TDM_PHILIPS_SLOT_DEFAULT_CONFIG`,
 *                          `I2S_TDM_PCM_SHORT_SLOT_DEFAULT_CONFIG`, `I2S_TDM_PCM_LONG_SLOT_DEFAULT_CONFIG` or `I2S_TDM_MSB_SLOT_DEFAULT_CONFIG`.
 * @return
 *      - ESP_OK    Set clock successfully
 *      - ESP_ERR_NO_MEM        No memory for DMA buffer
 *      - ESP_ERR_INVALID_ARG   NULL pointer, invalid configuration  or not TDM mode
 *      - ESP_ERR_INVALID_STATE This channel is not initialized or not stopped
 */
esp_err_t i2s_channel_reconfig_tdm_slot(i2s_chan_handle_t handle, const i2s_tdm_slot_config_t *slot_cfg);

/**
 * @brief Reconfigure the I2S GPIO for TDM mode
 * @note  Only allowed to be called when the channel state is READY, i.e., channel has been initialized, but not started
 *        this function won't change the state. `i2s_channel_disable` should be called before calling this function if I2S has started.
 * @note  The input channel handle has to be initialized to TDM mode, i.e., `i2s_channel_init_tdm_mode` has been called before reconfiguring
 *
 * @param[in]   handle      I2S channel handler
 * @param[in]   gpio_cfg    Standard mode GPIO configuration, specified by user
 * @return
 *      - ESP_OK    Set clock successfully
 *      - ESP_ERR_INVALID_ARG   NULL pointer, invalid configuration  or not TDM mode
 *      - ESP_ERR_INVALID_STATE This channel is not initialized or not stopped
 */
esp_err_t i2s_channel_reconfig_tdm_gpio(i2s_chan_handle_t handle, const i2s_tdm_gpio_config_t *gpio_cfg);

#ifdef __cplusplus
}
#endif

#endif // SOC_I2S_SUPPORTS_TDM
