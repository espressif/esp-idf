/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "hal/adc_types.h"
#include "esp_err.h"
#include "esp_adc/adc_oneshot.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    adc_unit_t adc_n;        // ADC Unit
    adc_channel_t channel;   // ADC channel
    adc_atten_t atten;       // ADC channel attenuation
    adc_bitwidth_t width;    // ADC bit width, only used for ADC unit 1
    adc_ulp_mode_t ulp_mode; // ADC ULP Mode
} ulp_adc_cfg_t;      // ULP FSM ADC configuration parameters

/**
 * @brief ULP ADC channel configuration parameters
 */
typedef adc_oneshot_chan_cfg_t ulp_adc_chan_cfg_t;

/**
 * @brief Initialize and calibrate the ADC for use by ULP FSM
 *
 * @note Initializes the ADC unit for ULP use and configures the channel
 *       specified in cfg->channel. To monitor multiple channels, call
 *       ulp_adc_config_channel() afterwards to configure additional channels.
 *
 * @param cfg           Configuration parameters
 * @return esp_err_t    ESP_OK for successful.
 */
esp_err_t ulp_adc_init(const ulp_adc_cfg_t *cfg);

/**
 * @brief Configure and calibrate an additional ADC channel for use by ULP
 *
 * Configures a channel on an ADC unit that has already been initialized by
 * ulp_adc_init(). This can be called multiple times to configure multiple
 * channels on the same ADC unit.
 *
 * @note HW calibration parameters are stored per attenuation, but only one set
 *       is active in the ADC unit at a time. If channels use different
 *       attenuations, readings on channels whose attenuation differs from the
 *       last configured one may be less accurate.
 *
 * @param adc_n         ADC unit (must already be initialized via ulp_adc_init())
 * @param channel       ADC channel to configure
 * @param chan_cfg      Channel configuration (attenuation and bit width)
 * @return esp_err_t    ESP_OK for successful.
 */
esp_err_t ulp_adc_config_channel(adc_unit_t adc_n, adc_channel_t channel, const ulp_adc_chan_cfg_t *chan_cfg);

/**
 * @brief Deinitialize ADC after use with ULP, allowing it to be reclaimed
 *
 * @return esp_err_t    ESP_OK for successful.
*/
esp_err_t ulp_adc_deinit(void);

#ifdef __cplusplus
}
#endif
