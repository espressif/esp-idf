/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "hal/adc_types.h"
#include "../curve_fitting_coefficients.h"

#define COEFF_GROUP_NUM    4
#define TERM_MAX           5

/**
 * @note Error Calculation
 * Coefficients for calculating the reading voltage error.
 * Four sets of coefficients for atten0 ~ atten3 respectively.
 *
 * For each item, first element is the Coefficient, second element is the Multiple. (Coefficient / Multiple) is the real coefficient.
 *
 * @note {0,0} stands for unused item
 * @note In case of the overflow, these coefficients are recorded as Absolute Value
 * @note For atten0 ~ 2, error = (K0 * X^0) + (K1 * X^1) + (K2 * X^2); For atten3, error = (K0 * X^0) + (K1 * X^1)  + (K2 * X^2) + (K3 * X^3) + (K4 * X^4);
 * @note Above formula is rewritten from the original documentation, please note that the coefficients are re-ordered.
 */
const static uint64_t adc1_error_coef_atten[COEFF_GROUP_NUM][TERM_MAX][2] = {
    {{27856531419538344, 1e16}, {50871540569528, 1e16}, {9798249589, 1e15}, {0, 0}, {0, 0}},                       //ADC1 atten0
    {{29831022915028695, 1e16}, {49393185868806, 1e16}, {101379430548, 1e16}, {0, 0}, {0, 0}},                     //ADC1 atten1
    {{23285545746296417, 1e16}, {147640181047414, 1e16}, {208385525314, 1e16}, {0, 0}, {0, 0}},                    //ADC1 atten2
    {{644403418269478, 1e15}, {644334888647536, 1e16}, {1297891447611, 1e16}, {70769718, 1e15}, {13515, 1e15}}     //ADC1 atten3
};
const static uint64_t adc2_error_coef_atten[COEFF_GROUP_NUM][TERM_MAX][2] = {
    {{25668651654328927, 1e16}, {1353548869615, 1e16}, {36615265189, 1e16}, {0, 0}, {0, 0}},                       //ADC2 atten0
    {{23690184690298404, 1e16}, {66319894226185, 1e16}, {118964995959, 1e16}, {0, 0}, {0, 0}},                     //ADC2 atten1
    {{9452499397020617, 1e16}, {200996773954387, 1e16}, {259011467956, 1e16}, {0, 0}, {0, 0}},                     //ADC2 atten2
    {{12247719764336924, 1e16}, {755717904943462, 1e16}, {1478791187119, 1e16}, {79672528, 1e15}, {15038, 1e15}}   //ADC2 atten3
};
/**
 * Term sign
 */
const static int32_t adc1_error_sign[COEFF_GROUP_NUM][TERM_MAX] = {
    {-1, -1, 1, 0,  0},  //ADC1 atten0
    {-1, -1, 1, 0,  0},  //ADC1 atten1
    {-1, -1, 1, 0,  0},  //ADC1 atten2
    {-1, -1, 1, -1, 1}   //ADC1 atten3
};
const static int32_t adc2_error_sign[COEFF_GROUP_NUM][TERM_MAX] = {
    {-1,  1, 1,  0, 0},  //ADC2 atten0
    {-1, -1, 1,  0, 0},  //ADC2 atten1
    {-1, -1, 1,  0, 0},  //ADC2 atten2
    { 1, -1, 1, -1, 1}   //ADC2 atten3
};

void curve_fitting_get_second_step_coeff(const adc_cali_curve_fitting_config_t *config, cali_chars_second_step_t *ctx)
{
    ctx->term_num = (config->atten == 3) ? 5 : 3;
    ctx->coeff = config->unit_id == ADC_UNIT_1 ?
                 adc1_error_coef_atten[config->atten] :
                 adc2_error_coef_atten[config->atten];
    ctx->sign = config->unit_id == ADC_UNIT_1 ?
                adc1_error_sign[config->atten] :
                adc2_error_sign[config->atten];
}
