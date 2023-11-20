/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "driver/dac_types.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_DAC_SUPPORTED

typedef struct dac_cosine_s     *dac_cosine_handle_t;   /*!< DAC cosine wave channel handle */

/**
 * @brief DAC cosine channel configurations
 *
 */
typedef struct {
    dac_channel_t               chan_id;        /*!< The cosine wave channel id */
    uint32_t                    freq_hz;        /*!< The frequency of cosine wave, unit: Hz.
                                                *   The cosine wave generator is driven by RTC_FAST clock which is divide from RC_FAST,
                                                *   With the default RTC clock, the minimum frequency of cosine wave is about 130 Hz,
                                                *   Although it can support up to several MHz frequency theoretically,
                                                *   the waveform will distort at high frequency due to the hardware limitation.
                                                *   Typically not suggest to set the frequency higher than 200 KHz
                                                */
    dac_cosine_clk_src_t        clk_src;        /*!< The clock source of the cosine wave generator, currently only support `DAC_COSINE_CLK_SRC_DEFAULT` */
    dac_cosine_atten_t          atten;          /*!< The attenuation of cosine wave amplitude */
    dac_cosine_phase_t          phase;          /*!< The phase of cosine wave, can only support DAC_COSINE_PHASE_0 or DAC_COSINE_PHASE_180, default as 0 while setting an unsupported phase */
    int8_t                      offset;         /*!< The DC offset of cosine wave */
    struct {
        bool                    force_set_freq: 1; /*!< Force to set the cosine wave frequency */
    } flags;                                    /*!< Flags of cosine mode */
} dac_cosine_config_t;

/**
 * @brief Allocate a new DAC cosine wave channel
 * @note  Since there is only one cosine wave generator,
 *        only the first channel can set the frequency of the cosine wave.
 *        Normally, the latter one is not allowed to set a different frequency,
 *        but the it can be forced to set by setting the bit `force_set_freq` in the configuration,
 *        notice that another channel will be affected as well when the frequency is updated.
 *
 * @param[in]  cos_cfg      The configuration of cosine wave channel
 * @param[out] ret_handle   The returned cosine wave channel handle
 * @return
 *      - ESP_ERR_INVALID_ARG  The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The DAC channel has been registered already
 *      - ESP_ERR_NO_MEM        No memory for the DAC cosine wave channel resources
 *      - ESP_OK                Allocate the new DAC cosine wave channel success
 */
esp_err_t dac_cosine_new_channel(const dac_cosine_config_t *cos_cfg, dac_cosine_handle_t *ret_handle);

/**
 * @brief Delete the DAC cosine wave channel
 *
 * @param[in] handle        The DAC cosine wave channel handle
 * @return
 *      - ESP_ERR_INVALID_ARG  The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The channel has already been deregistered
 *      - ESP_OK                Delete the cosine wave channel success
 */
esp_err_t dac_cosine_del_channel(dac_cosine_handle_t handle);

/**
 * @brief Start outputting the cosine wave on the channel
 *
 * @param[in] handle            The DAC cosine wave channel handle
 * @return
 *      - ESP_ERR_INVALID_ARG  The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The channel has been started already
 *      - ESP_OK                Start the cosine wave success
 */
esp_err_t dac_cosine_start(dac_cosine_handle_t handle);

/**
 * @brief Stop outputting the cosine wave on the channel
 *
 * @param[in] handle            The DAC cosine wave channel handle
 * @return
 *      - ESP_ERR_INVALID_ARG  The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The channel has been stopped already
 *      - ESP_OK                Stop the cosine wave success
 */
esp_err_t dac_cosine_stop(dac_cosine_handle_t handle);

#endif // SOC_DAC_SUPPORTED

#ifdef __cplusplus
}
#endif
