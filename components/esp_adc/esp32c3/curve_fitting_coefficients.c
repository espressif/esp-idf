/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
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
 * @note ADC1 and ADC2 use same coefficients
 */
const static uint64_t adc1_error_coef_atten[COEFF_GROUP_NUM][TERM_MAX][2] = {
    {{225966470500043, 1e15}, {7265418501948, 1e16}, {109410402681, 1e16}, {0, 0}, {0, 0}},                         //atten0
    {{4229623392600516, 1e16}, {731527490903, 1e16}, {88166562521, 1e16}, {0, 0}, {0, 0}},                          //atten1
    {{1017859239236435, 1e15}, {97159265299153, 1e16}, {149794028038, 1e16}, {0, 0}, {0, 0}},                       //atten2
    {{14912262772850453, 1e16}, {228549975564099, 1e16}, {356391935717, 1e16}, {179964582, 1e16}, {42046, 1e16}}    //atten3
};
/**
 * Term sign
 */
const static int32_t adc1_error_sign[COEFF_GROUP_NUM][TERM_MAX] = {
    {-1, -1, 1,  0,  0}, //atten0
    { 1, -1, 1,  0,  0}, //atten1
    {-1, -1, 1,  0,  0}, //atten2
    {-1, -1, 1, -1,  1}  //atten3
};

void curve_fitting_get_second_step_coeff(const adc_cali_curve_fitting_config_t *config, cali_chars_second_step_t *ctx)
{
    ctx->term_num = (config->atten == 3) ? 5 : 3;
    // On esp32c3, ADC1 and ADC2 share the second step coefficients
    // And if the target only has 1 ADC peripheral, just use the ADC1 directly
    ctx->coeff = adc1_error_coef_atten[config->atten];
    ctx->sign = adc1_error_sign[config->atten];
}
