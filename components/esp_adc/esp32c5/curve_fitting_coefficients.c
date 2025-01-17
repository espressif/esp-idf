/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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
        {{2941017829027464,     1e16}, {7368674918527,   1e16}, {0, 0}},   //atten0
        {{3224276125615327,     1e16}, {5325658467636,   1e16}, {0, 0}},   //atten1
        {{3307554632960901,    1e16}, {409244304226,    1e15}, {0, 0}},   //atten2
        {{1463642578413965,    1e15}, {3349642363147,   1e15}, {11676836451, 1e16}},   //atten3
    },
};

/**
 * Term sign ADC1
 */
const static int32_t adc1_error_sign[COEFF_VERSION_NUM][COEFF_GROUP_NUM][TERM_MAX] = {
    /* Coefficient sign of calibration version 1 */
    {
        {1,  -1,  0}, //atten0
        {1,  -1,  0}, //atten1
        {1,  -1,  0}, //atten2
        {1,  -1,  1}, //atten3
    },
};

void curve_fitting_get_second_step_coeff(const adc_cali_curve_fitting_config_t *config, cali_chars_second_step_t *ctx)
{
    uint32_t adc_calib_ver = esp_efuse_rtc_calib_get_ver();
    assert((adc_calib_ver >= ESP_EFUSE_ADC_CALIB_VER_MIN) &&
           (adc_calib_ver <= ESP_EFUSE_ADC_CALIB_VER_MAX));

    ctx->term_num = 2;
    ctx->coeff = adc1_error_coef_atten[VER2IDX(adc_calib_ver)][config->atten];
    ctx->sign = adc1_error_sign[VER2IDX(adc_calib_ver)][config->atten];
}
