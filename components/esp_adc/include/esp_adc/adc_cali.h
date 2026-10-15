/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "esp_bit_defs.h"
#include "hal/adc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ADC calibration handle
 */
typedef struct adc_cali_scheme_t *adc_cali_handle_t;

/**
 * @brief ADC calibration scheme
 */
typedef enum {
    ADC_CALI_SCHEME_VER_LINE_FITTING  = BIT(0),      ///< Line fitting scheme
    ADC_CALI_SCHEME_VER_CURVE_FITTING = BIT(1),      ///< Curve fitting scheme
} adc_cali_scheme_ver_t;

/**
 * @brief Check the supported ADC calibration scheme
 *
 * @param[out] scheme_mask    Supported ADC calibration scheme(s)
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_INVALID_ARG:   Invalid argument
 *        - ESP_ERR_NOT_SUPPORTED: No supported calibration scheme
 */
esp_err_t adc_cali_check_scheme(adc_cali_scheme_ver_t *scheme_mask);

/**
 * @brief Convert ADC raw data to calibrated voltage
 *
 * @note Schemes that apply channel-dependent corrections cannot do so here.
 *       Use `adc_cali_chan_raw_to_voltage()` when the conversion depends on
 *       the ADC channel.
 * @note On ESP32-S31 this function is not supported and returns
 *       `ESP_ERR_NOT_SUPPORTED`. Use `adc_cali_chan_raw_to_voltage()`.
 *
 * @param[in]  handle     ADC calibration handle
 * @param[in]  raw        ADC raw data
 * @param[out] voltage    Calibrated ADC voltage (in mV)
 *
 * @return
 *         - ESP_OK:                On success
 *         - ESP_ERR_INVALID_ARG:   Invalid argument
 *         - ESP_ERR_INVALID_STATE: Invalid state, scheme didn't registered
 *         - ESP_ERR_NOT_SUPPORTED: Scheme requires the channel-aware conversion API
 */
esp_err_t adc_cali_raw_to_voltage(adc_cali_handle_t handle, int raw, int *voltage);

/**
 * @brief Convert ADC raw data to calibrated voltage (channel-aware)
 *
 * Compared with `adc_cali_raw_to_voltage()`, this takes the ADC channel the raw
 * data was sampled from, so schemes that need it can apply channel-dependent
 * corrections.
 * @note On ESP32-S31, `raw` is a 17-bit comparator vector. Even-numbered
 *       channels are N-side and odd-numbered channels are P-side.
 *
 * @param[in]  handle     ADC calibration handle
 * @param[in]  channel    ADC channel the raw data was sampled from
 * @param[in]  raw        ADC raw data
 * @param[out] voltage    Calibrated ADC voltage (in mV)
 *
 * @return
 *         - ESP_OK:                On success
 *         - ESP_ERR_INVALID_ARG:   Invalid argument
 *         - ESP_ERR_INVALID_STATE: Invalid state, scheme didn't registered
 */
esp_err_t adc_cali_chan_raw_to_voltage(adc_cali_handle_t handle, adc_channel_t channel, int raw, int *voltage);

#ifdef __cplusplus
}
#endif
