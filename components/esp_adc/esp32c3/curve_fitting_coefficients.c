/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
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
 * @note In case of the overflow, these coeffcients are recorded as Absolute Value
 * @note For atten0 ~ 2, error = (K0 * X^0) + (K1 * X^1) + (K2 * X^2); For atten3, error = (K0 * X^0) + (K1 * X^1)  + (K2 * X^2) + (K3 * X^3) + (K4 * X^4);
 * @note Above formula is rewritten from the original documentation, please note that the coefficients are re-ordered.
 * @note ADC1 and ADC2 use same coeffients
 */
const uint64_t adc1_error_coef_atten[4][5][2] = {
                                                {{225966470500043, 1e15}, {7265418501948, 1e16}, {109410402681, 1e16}, {0, 0}, {0, 0}},                         //atten0
                                                {{4229623392600516, 1e16}, {731527490903, 1e16}, {88166562521, 1e16}, {0, 0}, {0, 0}},                          //atten1
                                                {{1017859239236435, 1e15}, {97159265299153, 1e16}, {149794028038, 1e16}, {0, 0}, {0, 0}},                       //atten2
                                                {{14912262772850453, 1e16}, {228549975564099, 1e16}, {356391935717, 1e16}, {179964582, 1e16}, {42046, 1e16}}    //atten3
                                                };
/**
 * Term sign
 */
const int32_t adc1_error_sign[4][5] = {
                                        {-1, -1, 1,  0,  0}, //atten0
                                        { 1, -1, 1,  0,  0}, //atten1
                                        {-1, -1, 1,  0,  0}, //atten2
                                        {-1, -1, 1, -1,  1}  //atten3
                                    };
