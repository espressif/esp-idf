/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdint.h>
#include "esp_efuse_rtc_calib.h"
#include "../curve_fitting_coefficients.h"

#define COEFF_VERSION_NUM  1 // Currently C5 has one versions of curve calibration schemes
#define COEFF_GROUP_NUM    4
#define TERM_MAX           3

/**
 * @note Error Calculation
 * Coefficients for calculating the reading voltage error.
 * Four sets of coefficients for atten0 ~ atten3 respectively.
 *
 * For each item, first element is the Coefficient, second element is the Multiple. (Coefficient / Multiple) is the real coefficient.
 *
 * @note {0,0} stands for unused item
 * @note In case of the overflow, these coefficients are recorded as Absolute Value
 * @note For atten0 ~ 3, error = (K0 * X^0) + (K1 * X^1)
 * @note Above formula is rewritten from the original documentation, please note that the coefficients are re-ordered.
 */
const static uint64_t adc1_error_coef_atten[COEFF_VERSION_NUM][COEFF_GROUP_NUM][TERM_MAX][2] = {
    /* Coefficients of calibration version 1 */
    {
        {{8668885650149671,     1e16}, {15630376830615,   1e16}, {0, 1}},   //atten0
        {{1090569589734153,     1e15}, {13859487941542,   1e16}, {0, 1}},   //atten1
        {{14231790752153335,    1e16}, {122016745867,     1e14}, {0, 1}},   //atten2
        {{13204544579940347,    1e16}, {11762579610906,   1e16}, {7639928529, 1e16}},   //atten3
    },
};

/**
 * Term sign ADC1
 */
const static int32_t adc1_error_sign[COEFF_VERSION_NUM][COEFF_GROUP_NUM][TERM_MAX] = {
    /* Coefficient sign of calibration version 1 */
    {
        {-1,  1,  1}, //atten0
        {-1,  1,  1}, //atten1
        {-1,  1,  1}, //atten2
        {-1, -1,  1}, //atten3
    },
};

void curve_fitting_get_second_step_coeff(const adc_cali_curve_fitting_config_t *config, cali_chars_second_step_t *ctx)
{
    uint32_t adc_calib_ver = esp_efuse_rtc_calib_get_ver();
    assert((adc_calib_ver >= ESP_EFUSE_ADC_CALIB_VER_MIN) &&
           (adc_calib_ver <= ESP_EFUSE_ADC_CALIB_VER_MAX));

    ctx->term_num = 3;
    ctx->coeff = adc1_error_coef_atten[VER2IDX(adc_calib_ver)][config->atten];
    ctx->sign = adc1_error_sign[VER2IDX(adc_calib_ver)][config->atten];
}
