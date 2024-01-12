/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "hal/adc_types.h"
#include "esp_err.h"

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
 * @brief Initialize and calibrate the ADC for use by ULP FSM
 *
 * @param cfg           Configuration parameters
 * @return esp_err_t    ESP_OK for successful.
 */
esp_err_t ulp_adc_init(const ulp_adc_cfg_t *cfg);

/**
 * @brief Deinitialize ADC after use with ULP, allowing it to be reclaimed
 *
 * @return esp_err_t    ESP_OK for successful.
*/
esp_err_t ulp_adc_deinit(void);

#ifdef __cplusplus
}
#endif
