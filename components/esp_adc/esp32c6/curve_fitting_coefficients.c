/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdint.h>
#include "esp_efuse_rtc_calib.h"
#include "../curve_fitting_coefficients.h"

#define COEFF_VERSION_NUM  2 // Currently C6 has two versions of curve calibration schemes
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
 * @note For atten0 ~ 3, error = (K0 * X^0) + (K1 * X^1) + (K2 * X^2)
 * @note Above formula is rewritten from the original documentation, please note that the coefficients are re-ordered.
 */
const static uint64_t adc1_error_coef_atten[COEFF_VERSION_NUM][COEFF_GROUP_NUM][TERM_MAX][2] = {
    /* Coefficients of calibration version 1 */
    {
        {{487166399931449,   1e15}, {6436483033201,   1e16}, {30410131806, 1e16}},   //atten0
        {{8665498165817785,  1e16}, {15239070452946,  1e16}, {13818878844, 1e16}},   //atten1
        {{12277821756674387, 1e16}, {22275554717885,  1e16}, {5924302667,  1e16}},   //atten2
        {{3801417550380255,  1e16}, {6020352420772,   1e16}, {12442478488, 1e16}},   //atten3
    },
    /* Coefficients of calibration version 2 */
    {
        {{0, 0}, {0, 0}, {0, 0}},                                                    //atten0
        {{0, 0}, {0, 0}, {0, 0}},                                                    //atten1
        {{12217864764388775, 1e16}, {1954123107752,   1e16}, {6409679727,  1e16}},   //atten2
        {{3915910437042445, 1e16}, {31536470857564,  1e16}, {12493873014, 1e16}},    //atten3
    },
};

/**
 * Term sign
 */
const static int32_t adc1_error_sign[COEFF_VERSION_NUM][COEFF_GROUP_NUM][TERM_MAX] = {
    /* Coefficient sign of calibration version 1 */
    {
        {-1,  1,  1}, //atten0
        {-1,  1,  1}, //atten1
        {-1,  1,  1}, //atten2
        {-1, -1,  1}, //atten3
    },
    /* Coefficient sign of calibration version 2 */
    {
        { 0,  0,  0}, //atten0
        { 0,  0,  0}, //atten1
        {-1, -1,  1}, //atten2
        {-1, -1,  1}, //atten3
    },
};

void curve_fitting_get_second_step_coeff(const adc_cali_curve_fitting_config_t *config, cali_chars_second_step_t *ctx)
{
    uint32_t adc_calib_ver = esp_efuse_rtc_calib_get_ver();
    assert((adc_calib_ver >= ESP_EFUSE_ADC_CALIB_VER_MIN) &&
           (adc_calib_ver <= ESP_EFUSE_ADC_CALIB_VER_MAX));
    if (adc_calib_ver == ESP_EFUSE_ADC_CALIB_VER2 && config->atten < 2) {
        ctx->term_num = 0;
        ctx->coeff = NULL;
        ctx->sign = NULL;
    } else {
        ctx->term_num = 3;
        ctx->coeff = adc1_error_coef_atten[VER2IDX(adc_calib_ver)][config->atten];
        ctx->sign = adc1_error_sign[VER2IDX(adc_calib_ver)][config->atten];
    }
}
