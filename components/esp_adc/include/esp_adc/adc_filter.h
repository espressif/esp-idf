/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_adc/adc_continuous.h"
#include "hal/adc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of adc iir filter handle
 */
typedef struct adc_iir_filter_t *adc_iir_filter_handle_t;

/**
 * @brief Filter Configuration
 */
typedef struct {
    adc_unit_t unit;                    ///< ADC unit
    adc_channel_t channel;              ///< An ADC channel to be filtered. Note for ESP32S2, you should only set only one channel in pattern table, per ADC unit. See programming guide for more details.
    adc_digi_iir_filter_coeff_t coeff;  ///< ADC filter coefficient
} adc_continuous_iir_filter_config_t;

/**
 * @brief New an ADC continuous mode IIR filter
 *
 * @param[in]   handle   ADC continuous mode driver handle
 * @param[in]   config   Filter configuration
 * @param[out]  ret_hdl  Returned IIR filter handle
 *
 * @return
 *        - ESP_OK
 *        - ESP_ERR_INVALID_ARG:    Invalid argument
 *        - ESP_ERR_INVALID_STATE:  Invalid state
 *        - ESP_ERR_NO_MEM:         No memory
 */
esp_err_t adc_new_continuous_iir_filter(adc_continuous_handle_t handle, const adc_continuous_iir_filter_config_t *config, adc_iir_filter_handle_t *ret_hdl);

/**
 * @brief Enable an IIR filter
 *
 * @param[in] filter_hdl  ADC IIR filter handle
 *
 * @return
 *        - ESP_OK
 *        - ESP_ERR_INVALID_ARG:    Invalid argument
 *        - ESP_ERR_INVALID_STATE:  Invalid state
 */
esp_err_t adc_continuous_iir_filter_enable(adc_iir_filter_handle_t filter_hdl);

/**
 * @brief Disable an IIR filter
 *
 * @param[in] filter_hdl  ADC IIR filter handle
 *
 * @return
 *        - ESP_OK
 *        - ESP_ERR_INVALID_ARG:    Invalid argument
 *        - ESP_ERR_INVALID_STATE:  Invalid state
 */
esp_err_t adc_continuous_iir_filter_disable(adc_iir_filter_handle_t filter_hdl);

/**
 * @brief Delete the IIR filter
 *
 * @param[in] filter_hdl  ADC IIR filter handle
 *
 * @return
 *        - ESP_OK
 *        - ESP_ERR_INVALID_ARG:    Invalid argument
 *        - ESP_ERR_INVALID_STATE:  Invalid state
 */
esp_err_t adc_del_continuous_iir_filter(adc_iir_filter_handle_t filter_hdl);

#ifdef __cplusplus
}
#endif
