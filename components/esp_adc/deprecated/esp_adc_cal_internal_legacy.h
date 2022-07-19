/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3
#define ESP_ADC_CAL_CURVE_FITTING_SUPPORTED    1

#define COEFF_GROUP_NUM    4
#define TERM_MAX           5
#endif

#if ESP_ADC_CAL_CURVE_FITTING_SUPPORTED
/**
 * Calculation parameters used for curve fitting calibration algorithm error
 *
 * @note For atten0 ~ 2, error = (K0 * X^0) + (K1 * X^1) + (K2 * X^2); For atten3, error = (K0 * X^0) + (K1 * X^1)  + (K2 * X^2) + (K3 * X^3) + (K4 * X^4);
 *       Where X is the `v_cali_input`.
 */
typedef struct {
    uint64_t v_cali_input;                                      //Input to calculate the error
    uint8_t  term_num;                                          //Term number of the algorithm formula
    const uint64_t (*coeff)[COEFF_GROUP_NUM][TERM_MAX][2];      //Coeff of each term. See `adc_error_coef_atten` for details (and the magic number 2)
    const int32_t  (*sign)[COEFF_GROUP_NUM][TERM_MAX];          //Sign of each term
} esp_adc_error_calc_param_t;

/**
 * Calculate the curve fitting error
 *
 * @param param   see `esp_adc_error_calc_param_t`
 * @param atten   ADC attenuation
 */
int32_t esp_adc_cal_get_reading_error(const esp_adc_error_calc_param_t *param, uint8_t atten);

#endif  //#if ESP_ADC_CAL_CURVE_FITTING_SUPPORTED


#ifdef __cplusplus
}
#endif
