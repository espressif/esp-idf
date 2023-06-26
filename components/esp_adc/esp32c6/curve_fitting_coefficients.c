/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

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
const uint64_t adc1_error_coef_atten[4][5][2] = {
                                                {{487166399931449,   1e16}, {6436483033201,   1e16}, {30410131806, 1e16}, {0, 0}, {0, 0}},   //atten0
                                                {{8665498165817785,  1e16}, {15239070452946,  1e16}, {13818878844, 1e16}, {0, 0}, {0, 0}},   //atten1
                                                {{12277821756674387, 1e16}, {22275554717885,  1e16}, {5924302667,  1e16}, {0, 0}, {0, 0}},   //atten2
                                                {{3801417550380255,  1e16}, {6020352420772,   1e16}, {12442478488, 1e16}, {0, 0}, {0, 0}}    //atten3
                                                };

/**
 * Term sign
 */
const int32_t adc1_error_sign[4][5] = {
                                        {-1,  1, 1,  0,  0}, //atten0
                                        {-1,  1, 1,  0,  0}, //atten1
                                        {-1,  1, 1,  0,  0}, //atten2
                                        {-1, -1, 1,  0,  0}  //atten3
                                    };
