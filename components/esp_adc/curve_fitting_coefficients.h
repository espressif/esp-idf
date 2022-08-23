/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>

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
 * @note In case of the overflow, these coeffcients are recorded as Absolute Value
 * @note For atten0 ~ 2, error = (K0 * X^0) + (K1 * X^1) + (K2 * X^2); For atten3, error = (K0 * X^0) + (K1 * X^1)  + (K2 * X^2) + (K3 * X^3) + (K4 * X^4);
 * @note Above formula is rewritten from the original documentation, please note that the coefficients are re-ordered.
 * @note ADC1 and ADC2 use same coeffients
 */
extern const uint64_t adc1_error_coef_atten[COEFF_GROUP_NUM][TERM_MAX][2];
extern const uint64_t adc2_error_coef_atten[COEFF_GROUP_NUM][TERM_MAX][2];

/**
 * Term sign
 */
extern const int32_t adc1_error_sign[COEFF_GROUP_NUM][TERM_MAX];
extern const int32_t adc2_error_sign[COEFF_GROUP_NUM][TERM_MAX];
