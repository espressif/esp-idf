/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
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
 * @param[in]  handle     ADC calibration handle
 * @param[in]  raw        ADC raw data
 * @param[out] voltage    Calibrated ADC voltage (in mV)
 *
 * @return
 *         - ESP_OK:                On success
 *         - ESP_ERR_INVALID_ARG:   Invalid argument
 *         - ESP_ERR_INVALID_STATE: Invalid state, scheme didn't registered
 */
esp_err_t adc_cali_raw_to_voltage(adc_cali_handle_t handle, int raw, int *voltage);


#ifdef __cplusplus
}
#endif
