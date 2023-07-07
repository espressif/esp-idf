/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
#include "esp_adc/adc_cali_scheme.h"

#ifdef __cplusplus
extern "C" {
#endif

#define COEFF_GROUP_NUM    4
#define TERM_MAX           5

typedef struct {
    uint8_t  term_num;                                        ///< Term number of the algorithm formula
    const uint64_t (*coeff)[COEFF_GROUP_NUM][TERM_MAX][2];    ///< Coeff of each term. See `adc_error_coef_atten` for details (and the magic number 2)
    const int32_t  (*sign)[COEFF_GROUP_NUM][TERM_MAX];        ///< Sign of each term
} cali_chars_second_step_t;

/**
 * @brief Assign the second step coefficients for curve calibration
 *
 * @param config the curve fitting configuration
 * @param ctx   the context pointer of the second step configuration structure
 */
void curve_fitting_get_second_step_coeff(const adc_cali_curve_fitting_config_t *config, cali_chars_second_step_t *ctx);

#ifdef __cplusplus
}
#endif
