/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * This file is specified for I2S PDM communication mode
 * Features:
 *      - Only support PDM TX/RX mode
 *      - Fixed to 2 slots
 *      - Data bit width only support 16 bits
 */
#pragma once

#include "hal/i2s_types.h"
#include "hal/gpio_types.h"
#include "driver/i2s_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_I2S_SUPPORTS_PDM_RX

#if SOC_I2S_SUPPORTS_PDM_RX_HP_FILTER
/**
 * @brief PDM format in 2 slots(RX)
 * @param bits_per_sample I2S data bit width, only support 16 bits for PDM mode
 * @param mono_or_stereo I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
 */
#define I2S_PDM_RX_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo) { \
    .data_bit_width = bits_per_sample, \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO, \
    .slot_mode = mono_or_stereo, \
    .slot_mask = (mono_or_stereo  == I2S_SLOT_MODE_MONO) ? \
                 I2S_PDM_SLOT_LEFT : I2S_PDM_SLOT_BOTH, \
    .hp_en = true, \
    .hp_cut_off_freq_hz = 35.5, \
    .amplify_num = 1, \
}
#else
/**
 * @brief PDM format in 2 slots(RX)
 * @param bits_per_sample I2S data bit width, only support 16 bits for PDM mode
 * @param mono_or_stereo I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
 */
#define I2S_PDM_RX_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo) { \
    .data_bit_width = bits_per_sample, \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO, \
    .slot_mode = mono_or_stereo, \
    .slot_mask = (mono_or_stereo  == I2S_SLOT_MODE_MONO) ? \
                 I2S_PDM_SLOT_LEFT : I2S_PDM_SLOT_BOTH, \
}
#endif  // SOC_I2S_SUPPORTS_PDM_RX_HP_FILTER

/**
 * @brief I2S default PDM RX clock configuration
 * @param rate sample rate
 */
#define I2S_PDM_RX_CLK_DEFAULT_CONFIG(rate) { \
    .sample_rate_hz = rate, \
    .clk_src = I2S_CLK_SRC_DEFAULT, \
    .mclk_multiple = I2S_MCLK_MULTIPLE_256, \
    .dn_sample_mode = I2S_PDM_DSR_8S, \
    .bclk_div = 8, \
}



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
#if SOC_I2S_SUPPORTS_PDM_RX_HP_FILTER
    bool                    hp_en;              /*!< High pass filter enable */
    float                   hp_cut_off_freq_hz; /*!< High pass filter cut-off frequency, range 23.3Hz ~ 185Hz, see cut-off frequency sheet above */
    uint32_t                amplify_num;        /*!< The amplification number of the final conversion result.
                                                 *   The data that have converted from PDM to PCM module, will time `amplify_num` additionally to amplify the final result.
                                                 *   Note that it's only a multiplier of the digital PCM data, not the gain of the analog signal
                                                 *   range 1~15, default 1 */
#endif  // SOC_I2S_SUPPORTS_PDM_RX_HP_FILTER

} i2s_pdm_rx_slot_config_t;

/**
 * @brief I2S clock configuration for PDM RX mode
 */
typedef struct {
    /* General fields */
    uint32_t                sample_rate_hz;     /*!< I2S sample rate */
    i2s_clock_src_t         clk_src;            /*!< Choose clock source */
    i2s_mclk_multiple_t     mclk_multiple;      /*!< The multiple of MCLK to the sample rate */
    /* Particular fields */
    i2s_pdm_dsr_t           dn_sample_mode;     /*!< Down-sampling rate mode */
    uint32_t                bclk_div;           /*!< The division from MCLK to BCLK. The typical and minimum value is I2S_PDM_RX_BCLK_DIV_MIN.
                                                 *   It will be set to I2S_PDM_RX_BCLK_DIV_MIN by default if it is smaller than I2S_PDM_RX_BCLK_DIV_MIN */
} i2s_pdm_rx_clk_config_t;

/**
 * @brief I2S PDM TX mode GPIO pins configuration
 */
typedef struct {
    gpio_num_t clk;                                     /*!< PDM clk pin, output */
    union {
        gpio_num_t din;                                 /*!< DATA pin 0, input */
        gpio_num_t dins[SOC_I2S_PDM_MAX_RX_LINES];      /*!< DATA pins, input, only take effect when corresponding I2S_PDM_RX_LINEx_SLOT_xxx is enabled in i2s_pdm_rx_slot_config_t::slot_mask */
    };
    struct {
        uint32_t   clk_inv: 1;     /*!< Set 1 to invert the clk output */
    } invert_flags;                /*!< GPIO pin invert flags */
} i2s_pdm_rx_gpio_config_t;

/**
 * @brief I2S PDM RX mode major configuration that including clock/slot/GPIO configuration
 */
typedef struct {
    i2s_pdm_rx_clk_config_t    clk_cfg;         /*!< PDM RX clock configurations, can be generated by macro I2S_PDM_RX_CLK_DEFAULT_CONFIG */
    i2s_pdm_rx_slot_config_t   slot_cfg;        /*!< PDM RX slot configurations, can be generated by macro I2S_PDM_RX_SLOT_DEFAULT_CONFIG */
    i2s_pdm_rx_gpio_config_t   gpio_cfg;        /*!< PDM RX slot configurations, specified by user */
} i2s_pdm_rx_config_t;

/**
 * @brief Initialize I2S channel to PDM RX mode
 * @note  Only allowed to be called when the channel state is REGISTERED, (i.e., channel has been allocated, but not initialized)
 *        and the state will be updated to READY if initialization success, otherwise the state will return to REGISTERED.
 *
 * @param[in]   handle      I2S RX channel handler
 * @param[in]   pdm_rx_cfg  Configurations for PDM RX mode, including clock, slot and GPIO
 *                          The clock configuration can be generated by the helper macro `I2S_PDM_RX_CLK_DEFAULT_CONFIG`
 *                          The slot configuration can be generated by the helper macro `I2S_PDM_RX_SLOT_DEFAULT_CONFIG`
 *
 * @return
 *      - ESP_OK    Initialize successfully
 *      - ESP_ERR_NO_MEM        No memory for storing the channel information
 *      - ESP_ERR_INVALID_ARG   NULL pointer or invalid configuration
 *      - ESP_ERR_INVALID_STATE This channel is not registered
 */
esp_err_t i2s_channel_init_pdm_rx_mode(i2s_chan_handle_t handle, const i2s_pdm_rx_config_t *pdm_rx_cfg);

/**
 * @brief Reconfigure the I2S clock for PDM RX mode
 * @note  Only allowed to be called when the channel state is READY, i.e., channel has been initialized, but not started
 *        this function won't change the state. `i2s_channel_disable` should be called before calling this function if I2S has started.
 * @note  The input channel handle has to be initialized to PDM RX mode, i.e., `i2s_channel_init_pdm_rx_mode` has been called before reconfiguring
 *
 * @param[in]   handle      I2S RX channel handler
 * @param[in]   clk_cfg     PDM RX mode clock configuration, can be generated by `I2S_PDM_RX_CLK_DEFAULT_CONFIG`
 * @return
 *      - ESP_OK    Set clock successfully
 *      - ESP_ERR_INVALID_ARG   NULL pointer, invalid configuration or not PDM mode
 *      - ESP_ERR_INVALID_STATE This channel is not initialized or not stopped
 */
esp_err_t i2s_channel_reconfig_pdm_rx_clock(i2s_chan_handle_t handle, const i2s_pdm_rx_clk_config_t *clk_cfg);

/**
 * @brief Reconfigure the I2S slot for PDM RX mode
 * @note  Only allowed to be called when the channel state is READY, i.e., channel has been initialized, but not started
 *        this function won't change the state. `i2s_channel_disable` should be called before calling this function if I2S has started.
 * @note  The input channel handle has to be initialized to PDM RX mode, i.e., `i2s_channel_init_pdm_rx_mode` has been called before reconfiguring
 *
 * @param[in]   handle      I2S RX channel handler
 * @param[in]   slot_cfg    PDM RX mode slot configuration, can be generated by `I2S_PDM_RX_SLOT_DEFAULT_CONFIG`
 * @return
 *      - ESP_OK    Set clock successfully
 *      - ESP_ERR_NO_MEM        No memory for DMA buffer
 *      - ESP_ERR_INVALID_ARG   NULL pointer, invalid configuration  or not PDM mode
 *      - ESP_ERR_INVALID_STATE This channel is not initialized or not stopped
 */
esp_err_t i2s_channel_reconfig_pdm_rx_slot(i2s_chan_handle_t handle, const i2s_pdm_rx_slot_config_t *slot_cfg);

/**
 * @brief Reconfigure the I2S GPIO for PDM RX mode
 * @note  Only allowed to be called when the channel state is READY, i.e., channel has been initialized, but not started
 *        this function won't change the state. `i2s_channel_disable` should be called before calling this function if I2S has started.
 * @note  The input channel handle has to be initialized to PDM RX mode, i.e., `i2s_channel_init_pdm_rx_mode` has been called before reconfiguring
 *
 * @param[in]   handle      I2S RX channel handler
 * @param[in]   gpio_cfg    PDM RX mode GPIO configuration, specified by user
 * @return
 *      - ESP_OK    Set clock successfully
 *      - ESP_ERR_INVALID_ARG   NULL pointer, invalid configuration  or not PDM mode
 *      - ESP_ERR_INVALID_STATE This channel is not initialized or not stopped
 */
esp_err_t i2s_channel_reconfig_pdm_rx_gpio(i2s_chan_handle_t handle, const i2s_pdm_rx_gpio_config_t *gpio_cfg);


#endif // SOC_I2S_SUPPORTS_PDM_RX


#if SOC_I2S_SUPPORTS_PDM_TX
#if SOC_I2S_HW_VERSION_2
/**
 * @brief PDM style in 2 slots(TX) for codec line mode
 * @param bits_per_sample I2S data bit width, only support 16 bits for PDM mode
 * @param mono_or_stereo I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
 */
#define I2S_PDM_TX_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo) { \
    .data_bit_width = bits_per_sample, \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO, \
    .slot_mode = mono_or_stereo, \
    .sd_prescale = 0, \
    .sd_scale = I2S_PDM_SIG_SCALING_MUL_1, \
    .hp_scale = I2S_PDM_SIG_SCALING_DIV_2, \
    .lp_scale = I2S_PDM_SIG_SCALING_MUL_1, \
    .sinc_scale = I2S_PDM_SIG_SCALING_MUL_1, \
    .line_mode = I2S_PDM_TX_ONE_LINE_CODEC, \
    .hp_en = true, \
    .hp_cut_off_freq_hz = 35.5, \
    .sd_dither = 0, \
    .sd_dither2 = 1, \
}

/**
 * @brief PDM style in 1 slots(TX) for DAC line mode
 * @note The noise might be different with different configurations, this macro provides a set of configurations
 *       that have relatively high SNR (Signal Noise Ratio), you can also adjust them to fit your case.
 * @param bits_per_sample I2S data bit width, only support 16 bits for PDM mode
 * @param mono_or_stereo I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
 */
#define I2S_PDM_TX_SLOT_DAC_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo) { \
    .data_bit_width = bits_per_sample, \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO, \
    .slot_mode = mono_or_stereo, \
    .sd_prescale = 0, \
    .sd_scale = I2S_PDM_SIG_SCALING_MUL_1, \
    .hp_scale = I2S_PDM_SIG_SCALING_MUL_1, \
    .lp_scale = I2S_PDM_SIG_SCALING_MUL_1, \
    .sinc_scale = I2S_PDM_SIG_SCALING_MUL_1, \
    .line_mode = ((mono_or_stereo) == I2S_SLOT_MODE_MONO ? \
                 I2S_PDM_TX_ONE_LINE_DAC : I2S_PDM_TX_TWO_LINE_DAC), \
    .hp_en = true, \
    .hp_cut_off_freq_hz = 35.5, \
    .sd_dither = 0, \
    .sd_dither2 = 1, \
}
#else // SOC_I2S_HW_VERSION_2
/**
 * @brief PDM style in 2 slots(TX) for codec line mode
 * @param bits_per_sample I2S data bit width, only support 16 bits for PDM mode
 * @param mono_or_stereo I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
 */
#define I2S_PDM_TX_SLOT_DEFAULT_CONFIG(bits_per_sample, mono_or_stereo) { \
    .data_bit_width = bits_per_sample, \
    .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO, \
    .slot_mode = mono_or_stereo, \
    .slot_mask = I2S_PDM_SLOT_BOTH, \
    .sd_prescale = 0, \
    .sd_scale = I2S_PDM_SIG_SCALING_MUL_1, \
    .hp_scale = I2S_PDM_SIG_SCALING_MUL_1, \
    .lp_scale = I2S_PDM_SIG_SCALING_MUL_1, \
    .sinc_scale = I2S_PDM_SIG_SCALING_MUL_1, \
}
#endif // SOC_I2S_HW_VERSION_2

/**
 * @brief I2S default PDM TX clock configuration for codec line mode
 * @note TX PDM can only be set to the following two up-sampling rate configurations:
 *       1: fp = 960, fs = sample_rate_hz / 100, in this case, Fpdm = 128*48000
 *       2: fp = 960, fs = 480, in this case, Fpdm = 128*Fpcm = 128*sample_rate_hz
 *       If the PDM receiver do not care the PDM serial clock, it's recommended set Fpdm = 128*48000.
 *       Otherwise, the second configuration should be adopted.
 * @param rate sample rate (not suggest to exceed 48000 Hz, otherwise more glitches and noise may appear)
 */
#define I2S_PDM_TX_CLK_DEFAULT_CONFIG(rate) { \
    .sample_rate_hz = rate, \
    .clk_src = I2S_CLK_SRC_DEFAULT, \
    .mclk_multiple = I2S_MCLK_MULTIPLE_256, \
    .up_sample_fp = 960, \
    .up_sample_fs = 480, \
    .bclk_div = 8, \
}

/**
 * @brief I2S default PDM TX clock configuration for DAC line mode
 * @note TX PDM can only be set to the following two up-sampling rate configurations:
 *       1: fp = 960, fs = sample_rate_hz / 100, in this case, Fpdm = 128*48000
 *       2: fp = 960, fs = 480, in this case, Fpdm = 128*Fpcm = 128*sample_rate_hz
 *       If the PDM receiver do not care the PDM serial clock, it's recommended set Fpdm = 128*48000.
 *       Otherwise, the second configuration should be adopted.
 * @note The noise might be different with different configurations, this macro provides a set of configurations
 *       that have relatively high SNR (Signal Noise Ratio), you can also adjust them to fit your case.
 * @param rate sample rate (not suggest to exceed 48000 Hz, otherwise more glitches and noise may appear)
 */
#define I2S_PDM_TX_CLK_DAC_DEFAULT_CONFIG(rate) { \
    .sample_rate_hz = rate, \
    .clk_src = I2S_CLK_SRC_DEFAULT, \
    .mclk_multiple = I2S_MCLK_MULTIPLE_256, \
    .up_sample_fp = 960, \
    .up_sample_fs = (rate) / 100, \
    .bclk_div = 13, \
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
 * @brief I2S slot configuration for PDM TX mode
 */
typedef struct {
    /* General fields */
    i2s_data_bit_width_t    data_bit_width;     /*!< I2S sample data bit width (valid data bits per sample), only support 16 bits for PDM mode */
    i2s_slot_bit_width_t    slot_bit_width;     /*!< I2S slot bit width (total bits per slot), only support 16 bits for PDM mode */
    i2s_slot_mode_t         slot_mode;          /*!< Set mono or stereo mode with I2S_SLOT_MODE_MONO or I2S_SLOT_MODE_STEREO
                                                 *   For PDM TX mode, mono means the data buffer only contains one slot data,
                                                 *   Stereo means the data buffer contains two slots data
                                                 */
    /* Particular fields */
#if SOC_I2S_HW_VERSION_1
    i2s_pdm_slot_mask_t     slot_mask;          /*!< Slot mask to choose left or right slot */
#endif
    uint32_t                sd_prescale;        /*!< Sigma-delta filter prescale */
    i2s_pdm_sig_scale_t     sd_scale;           /*!< Sigma-delta filter scaling value */
    i2s_pdm_sig_scale_t     hp_scale;           /*!< High pass filter scaling value */
    i2s_pdm_sig_scale_t     lp_scale;           /*!< Low pass filter scaling value */
    i2s_pdm_sig_scale_t     sinc_scale;         /*!< Sinc filter scaling value */
#if SOC_I2S_HW_VERSION_2
    i2s_pdm_tx_line_mode_t  line_mode;          /*!< PDM TX line mode, one-line codec, one-line dac, two-line dac mode can be selected */
    bool                    hp_en;              /*!< High pass filter enable */
    float                   hp_cut_off_freq_hz; /*!< High pass filter cut-off frequency, range 23.3Hz ~ 185Hz, see cut-off frequency sheet above */
    uint32_t                sd_dither;          /*!< Sigma-delta filter dither */
    uint32_t                sd_dither2;         /*!< Sigma-delta filter dither2 */
#endif // SOC_I2S_HW_VERSION_2
} i2s_pdm_tx_slot_config_t;

/**
 * @brief I2S clock configuration for PDM TX mode
 */
typedef struct {
    /* General fields */
    uint32_t                sample_rate_hz;     /*!< I2S sample rate, not suggest to exceed 48000 Hz, otherwise more glitches and noise may appear */
    i2s_clock_src_t         clk_src;            /*!< Choose clock source */
    i2s_mclk_multiple_t     mclk_multiple;      /*!< The multiple of MCLK to the sample rate */
    /* Particular fields */
    uint32_t                up_sample_fp;       /*!< Up-sampling param fp */
    uint32_t                up_sample_fs;       /*!< Up-sampling param fs, not allowed to be greater than 480 */
    uint32_t                bclk_div;           /*!< The division from MCLK to BCLK. The minimum value is I2S_PDM_TX_BCLK_DIV_MIN.
                                                 *   It will be set to I2S_PDM_TX_BCLK_DIV_MIN by default if it is smaller than I2S_PDM_TX_BCLK_DIV_MIN */
} i2s_pdm_tx_clk_config_t;

/**
 * @brief I2S PDM TX mode GPIO pins configuration
 */
typedef struct {
    gpio_num_t clk;                /*!< PDM clk pin, output */
    gpio_num_t dout;               /*!< DATA pin, output */
#if SOC_I2S_PDM_MAX_TX_LINES > 1
    gpio_num_t dout2;              /*!< The second data pin for the DAC dual-line mode,
                                    *   only take effect when the line mode is `I2S_PDM_TX_TWO_LINE_DAC`
                                    */
#endif
    struct {
        uint32_t   clk_inv: 1;     /*!< Set 1 to invert the clk output */
    } invert_flags;                /*!< GPIO pin invert flags */
} i2s_pdm_tx_gpio_config_t;

/**
 * @brief I2S PDM TX mode major configuration that including clock/slot/GPIO configuration
 */
typedef struct {
    i2s_pdm_tx_clk_config_t    clk_cfg;         /*!< PDM TX clock configurations, can be generated by macro I2S_PDM_TX_CLK_DEFAULT_CONFIG */
    i2s_pdm_tx_slot_config_t   slot_cfg;        /*!< PDM TX slot configurations, can be generated by macro I2S_PDM_TX_SLOT_DEFAULT_CONFIG */
    i2s_pdm_tx_gpio_config_t   gpio_cfg;        /*!< PDM TX GPIO configurations, specified by user */
} i2s_pdm_tx_config_t;

/**
 * @brief Initialize I2S channel to PDM TX mode
 * @note  Only allowed to be called when the channel state is REGISTERED, (i.e., channel has been allocated, but not initialized)
 *        and the state will be updated to READY if initialization success, otherwise the state will return to REGISTERED.
 *
 * @param[in]   handle      I2S TX channel handler
 * @param[in]   pdm_tx_cfg  Configurations for PDM TX mode, including clock, slot and GPIO
 *                          The clock configuration can be generated by the helper macro `I2S_PDM_TX_CLK_DEFAULT_CONFIG`
 *                          The slot configuration can be generated by the helper macro `I2S_PDM_TX_SLOT_DEFAULT_CONFIG`
 *
 * @return
 *      - ESP_OK    Initialize successfully
 *      - ESP_ERR_NO_MEM        No memory for storing the channel information
 *      - ESP_ERR_INVALID_ARG   NULL pointer or invalid configuration
 *      - ESP_ERR_INVALID_STATE This channel is not registered
 */
esp_err_t i2s_channel_init_pdm_tx_mode(i2s_chan_handle_t handle, const i2s_pdm_tx_config_t *pdm_tx_cfg);

/**
 * @brief Reconfigure the I2S clock for PDM TX mode
 * @note  Only allowed to be called when the channel state is READY, i.e., channel has been initialized, but not started
 *        this function won't change the state. `i2s_channel_disable` should be called before calling this function if I2S has started.
 * @note  The input channel handle has to be initialized to PDM TX mode, i.e., `i2s_channel_init_pdm_tx_mode` has been called before reconfiguring
 *
 * @param[in]   handle      I2S TX channel handler
 * @param[in]   clk_cfg     PDM TX mode clock configuration, can be generated by `I2S_PDM_TX_CLK_DEFAULT_CONFIG`
 * @return
 *      - ESP_OK    Set clock successfully
 *      - ESP_ERR_INVALID_ARG   NULL pointer, invalid configuration or not PDM mode
 *      - ESP_ERR_INVALID_STATE This channel is not initialized or not stopped
 */
esp_err_t i2s_channel_reconfig_pdm_tx_clock(i2s_chan_handle_t handle, const i2s_pdm_tx_clk_config_t *clk_cfg);

/**
 * @brief Reconfigure the I2S slot for PDM TX mode
 * @note  Only allowed to be called when the channel state is READY, i.e., channel has been initialized, but not started
 *        this function won't change the state. `i2s_channel_disable` should be called before calling this function if I2S has started.
 * @note  The input channel handle has to be initialized to PDM TX mode, i.e., `i2s_channel_init_pdm_tx_mode` has been called before reconfiguring
 *
 * @param[in]   handle      I2S TX channel handler
 * @param[in]   slot_cfg    PDM TX mode slot configuration, can be generated by `I2S_PDM_TX_SLOT_DEFAULT_CONFIG`
 * @return
 *      - ESP_OK    Set clock successfully
 *      - ESP_ERR_NO_MEM        No memory for DMA buffer
 *      - ESP_ERR_INVALID_ARG   NULL pointer, invalid configuration  or not PDM mode
 *      - ESP_ERR_INVALID_STATE This channel is not initialized or not stopped
 */
esp_err_t i2s_channel_reconfig_pdm_tx_slot(i2s_chan_handle_t handle, const i2s_pdm_tx_slot_config_t *slot_cfg);

/**
 * @brief Reconfigure the I2S GPIO for PDM TX mode
 * @note  Only allowed to be called when the channel state is READY, i.e., channel has been initialized, but not started
 *        this function won't change the state. `i2s_channel_disable` should be called before calling this function if I2S has started.
 * @note  The input channel handle has to be initialized to PDM TX mode, i.e., `i2s_channel_init_pdm_tx_mode` has been called before reconfiguring
 *
 * @param[in]   handle      I2S TX channel handler
 * @param[in]   gpio_cfg    PDM TX mode GPIO configuration, specified by user
 * @return
 *      - ESP_OK    Set clock successfully
 *      - ESP_ERR_INVALID_ARG   NULL pointer, invalid configuration  or not PDM mode
 *      - ESP_ERR_INVALID_STATE This channel is not initialized or not stopped
 */
esp_err_t i2s_channel_reconfig_pdm_tx_gpio(i2s_chan_handle_t handle, const i2s_pdm_tx_gpio_config_t *gpio_cfg);

#endif // SOC_I2S_SUPPORTS_PDM_TX

#ifdef __cplusplus
}
#endif
