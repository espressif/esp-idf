/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_types.h"
#include "esp_err.h"
#include "driver/i2s_types.h"
#include "hal/i2s_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief PDM format in 2 slots(RX)
 * @param bits_per_sample I2S data bit width, only support 16 bits for PDM mode
 * @param mono_or_stereo I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
 */
#define LP_I2S_PDM_RX_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo) { \
    .data_bit_width = bits_per_sample, \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO, \
    .slot_mode = mono_or_stereo, \
    .slot_mask = (mono_or_stereo  == I2S_SLOT_MODE_MONO) ? \
                 I2S_PDM_SLOT_LEFT : I2S_PDM_SLOT_BOTH, \
    .hp_en = true, \
    .hp_cut_off_freq_hz = 35.5, \
    .amplify_num = 1, \
}

/**
 * @brief LP I2S pin configurations
 */
typedef struct {
    int clk;                               /*!< clk pin number */
    int din;                               /*!< din pin number */
} lp_i2s_pdm_rx_gpio_config_t;

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
 * @brief I2S slot configuration for PDM RX mode
 */
typedef struct {
    /* General fields */
    i2s_data_bit_width_t    data_bit_width;     /*!< I2S sample data bit width (valid data bits per sample), only support 16 bits for PDM mode */
    i2s_slot_bit_width_t    slot_bit_width;     /*!< I2S slot bit width (total bits per slot) , only support 16 bits for PDM mode */
    i2s_slot_mode_t         slot_mode;          /*!< Set mono or stereo mode with I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO */
    /* Particular fields */
    i2s_pdm_slot_mask_t     slot_mask;          /*!< Choose the slots to activate */
    bool                    ws_pol;             /*!< WS signal polarity, set true to enable high lever first */
    bool                    hp_en;              /*!< High pass filter enable */
    float                   hp_cut_off_freq_hz; /*!< High pass filter cut-off frequency, range 23.3Hz ~ 185Hz, see cut-off frequency sheet above */
    uint32_t                amplify_num;        /*!< The amplification number of the final conversion result.
                                                 *   The data that have converted from PDM to PCM module, will time `amplify_num` additionally to amplify the final result.
                                                 *   Note that it's only a multiplier of the digital PCM data, not the gain of the analog signal
                                                 *   range 1~15, default 1 */
} lp_i2s_pdm_rx_slot_config_t;

/**
 * @brief LP I2S PDM configuration
 */
typedef struct {
    lp_i2s_pdm_rx_gpio_config_t   pin_cfg;    /*!< Pin configuration */
    lp_i2s_pdm_rx_slot_config_t   slot_cfg;   /*!< PDM mode slot configuration */
    /* LP I2S only support slave mode, not support to configure the clock */
} lp_i2s_pdm_rx_config_t;

/**
 * @brief Init LP I2S to PDM mode
 *
 * @param[in] handle   LP I2S channel handle
 * @param[in] pdm_cfg  PDM configuration
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_INVALID_ARG:   Invalid argument
 *        - ESP_ERR_INVALID_STATE: Invalid state
 */
esp_err_t lp_i2s_channel_init_pdm_rx_mode(lp_i2s_chan_handle_t handle, const lp_i2s_pdm_rx_config_t *pdm_cfg);

#ifdef __cplusplus
}
#endif
