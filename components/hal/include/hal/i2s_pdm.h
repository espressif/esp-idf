/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * This file is specified for I2S PDM communication mode
 * Features:
 *      - Only support PDM tx/rx mode
 *      - Fixed to 2 slots
 *      - Data bit width only support 16 bits
 */
#pragma once

#include "hal/i2s_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_I2S_SUPPORTS_PDM_RX

/**
 * @brief PDM format in 2 slots(RX)
 * @param bits_per_sample i2s data bit width, only support 16 bits for PDM mode
 * @param mono_or_stereo I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
 */
#define I2S_PDM_RX_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo) { \
    .mode = I2S_COMM_MODE_PDM, \
    .data_bit_width = bits_per_sample, \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_DEFAULT, \
    .slot_mode = mono_or_stereo, \
}

/**
 * @brief i2s default pdm rx clock configuration
 * @param rate sample rate
 */
#define I2S_PDM_RX_CLK_DEFAULT_CONFIG(rate) { \
    .sample_rate_hz = rate, \
    .clk_src = I2S_CLK_D2CLK, \
    .mclk_multiple = I2S_MCLK_MULTIPLE_256, \
    .dn_sample_mode = I2S_PDM_DSR_8S \
}

/**
 * @breif I2S slot configuration for pdm rx mode
 */
typedef struct {
    /* General fields */
    i2s_comm_mode_t         mode;               /*!< I2S communication mode, this field is for identification (MUST match the communication mode in 'i2s_chan_config_t') */
    i2s_data_bit_width_t    data_bit_width;     /*!< I2S sample data bit width (valid data bits per sample), only support 16 bits for PDM mode */
    i2s_slot_bit_width_t    slot_bit_width;     /*!< I2S slot bit width (total bits per slot) , only support 16 bits for PDM mode */
    i2s_slot_mode_t         slot_mode;          /*!< Set mono or stereo mode with I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO */

} i2s_pdm_rx_slot_config_t;

/**
 * @breif I2S clock configuration for pdm rx mode
 */
typedef struct {
    /* General fields */
    uint32_t                sample_rate_hz;     /*!< I2S sample rate */
    i2s_clock_src_t         clk_src;            /*!< Choose clock source */
    i2s_mclk_multiple_t     mclk_multiple;      /*!< The multiple of mclk to the sample rate */

    /* Particular fields */
    i2s_pdm_dsr_t           dn_sample_mode;     /*!< Down-sampling rate mode */
} i2s_pdm_rx_clk_config_t;

#endif // SOC_I2S_SUPPORTS_PDM_RX


#if SOC_I2S_SUPPORTS_PDM_TX
#if SOC_I2S_HW_VERSION_2
/**
 * @brief PDM style in 2 slots(TX)
 * @param bits_per_sample i2s data bit width, only support 16 bits for PDM mode
 * @param mono_or_stereo I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
 */
#define I2S_PDM_TX_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo) { \
    .mode = I2S_COMM_MODE_PDM, \
    .data_bit_width = bits_per_sample, \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_DEFAULT, \
    .slot_mode = mono_or_stereo, \
    .sd_prescale = 0, \
    .sd_scale = I2S_PDM_SIG_SCALING_MUL_1, \
    .hp_scale = I2S_PDM_SIG_SCALING_MUL_1, \
    .lp_scale = I2S_PDM_SIG_SCALING_MUL_1, \
    .sinc_scale = I2S_PDM_SIG_SCALING_MUL_1, \
    .sd_en = true, \
    .hp_en = true, \
    .hp_cut_off_freq_hz = 49, \
    .sd_dither = 0, \
    .sd_dither2 = 0, \
}
#else
/**
 * @brief PDM style in 2 slots(TX)
 * @param bits_per_sample i2s data bit width, only support 16 bits for PDM mode
 * @param mono_or_stereo I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
 */
#define I2S_PDM_TX_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo) { \
    .mode = I2S_COMM_MODE_PDM, \
    .data_bit_width = bits_per_sample, \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_DEFAULT, \
    .dma_desc_num = 8, \
    .dma_frame_num = 200, \
    .auto_clear = false, \
    .slot_mode = mono_or_stereo, \
    .sd_prescale = 0, \
    .sd_scale = I2S_PDM_SIG_SCALING_MUL_1, \
    .hp_scale = I2S_PDM_SIG_SCALING_MUL_1, \
    .lp_scale = I2S_PDM_SIG_SCALING_MUL_1, \
    .sinc_scale = I2S_PDM_SIG_SCALING_MUL_1, \
}
#endif // SOC_I2S_HW_VERSION_2

/**
 * @brief i2s default pdm tx clock configuration
 * @note TX PDM can only be set to the following two up-sampling rate configurations:
 *       1: fp = 960, fs = sample_rate_hz / 100, in this case, Fpdm = 128*48000
 *       2: fp = 960, fs = 480, in this case, Fpdm = 128*Fpcm = 128*sample_rate_hz
 *       If the pdm receiver do not care the pdm serial clock, it's recommended set Fpdm = 128*48000.
 *       Otherwise, the second configuration should be applied.
 * @param rate sample rate
 */
#define I2S_PDM_TX_CLK_DEFAULT_CONFIG(rate) { \
    .sample_rate_hz = rate, \
    .clk_src = I2S_CLK_D2CLK, \
    .mclk_multiple = I2S_MCLK_MULTIPLE_256, \
    .up_sample_fp = 960, \
    .up_sample_fs = ((rate) / 100), \
}

/*
                                    High Pass Filter Cut-off Frequency Sheet
 +----------------+------------------+----------------+------------------+----------------+------------------+
 | param0, param5 | cut-off freq(Hz) | param0, param5 | cut-off freq(Hz) | param0, param5 | cut-off freq(Hz) |
 +----------------+------------------+----------------+------------------+----------------+------------------+
 |     (0, 0)     |       185        |    (3, 3)      |       115        |    (5, 5)      |       69         |
 |     (0, 1)     |       172        |    (1, 7)      |       106        |    (4, 7)      |       63         |
 |     (1, 1)     |       160        |    (2, 4)      |       104        |    (5, 6)      |       58         |
 |     (1, 2)     |       150        |    (4, 4)      |       92         |    (5, 7)      |       49         |
 |     (2, 2)     |       137        |    (2, 7)      |       91.5       |    (6, 6)      |       46         |
 |     (2, 3)     |       126        |    (4, 5)      |       81         |    (6, 7)      |       35.5       |
 |     (0, 3)     |       120        |    (3, 7)      |       77.2       |    (7, 7)      |       23.3       |
 +----------------+------------------+----------------+------------------+----------------+------------------+
 */

/**
 * @brief I2S slot configuration for pdm tx mode
 */
typedef struct {
    /* General fields */
    i2s_comm_mode_t         mode;               /*!< I2S communication mode, this field is for identification (MUST match the communication mode in 'i2s_chan_config_t') */
    i2s_data_bit_width_t    data_bit_width;     /*!< I2S sample data bit width (valid data bits per sample), only support 16 bits for PDM mode */
    i2s_slot_bit_width_t    slot_bit_width;     /*!< I2S slot bit width (total bits per slot), only support 16 bits for PDM mode */
    i2s_slot_mode_t         slot_mode;          /*!< Set mono or stereo mode with I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO */

    /* Particular fields */
    uint32_t                sd_prescale;        /*!< Sigma-delta filter prescale */
    i2s_pdm_sig_scale_t     sd_scale;           /*!< Sigma-delta filter scaling value */
    i2s_pdm_sig_scale_t     hp_scale;           /*!< High pass filter scaling value */
    i2s_pdm_sig_scale_t     lp_scale;           /*!< Low pass filter scaling value */
    i2s_pdm_sig_scale_t     sinc_scale;         /*!< Sinc filter scaling value */
#if SOC_I2S_HW_VERSION_2
    bool                    sd_en;              /*!< Sigma-delta filter enable */
    bool                    hp_en;              /*!< High pass filter enable */
    float                   hp_cut_off_freq_hz; /*!< High pass filter cut-off frequency, range 23.3Hz ~ 185Hz, see cut-off frequency sheet above */
    uint32_t                sd_dither;          /*!< Sigma-delta filter dither */
    uint32_t                sd_dither2;         /*!< Sigma-delta filter dither2 */
#endif // SOC_I2S_HW_VERSION_2
} i2s_pdm_tx_slot_config_t;

/**
 * @breif I2S clock configuration for pdm tx mode
 */
typedef struct {
    /* General fields */
    uint32_t                sample_rate_hz;     /*!< I2S sample rate */
    i2s_clock_src_t         clk_src;            /*!< Choose clock source */
    i2s_mclk_multiple_t     mclk_multiple;      /*!< The multiple of mclk to the sample rate */

    /* Particular fields */
    uint32_t                up_sample_fp;       /*!< Up-sampling param fp */
    uint32_t                up_sample_fs;       /*!< Up-sampling param fs */
} i2s_pdm_tx_clk_config_t;

#endif // SOC_I2S_SUPPORTS_PDM_TX

#ifdef __cplusplus
}
#endif
