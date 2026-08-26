/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdlib.h>
#include <stdint.h>
#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_CORDIC_SUPPORTED
/**
 * @brief CORDIC group clock source
 */
typedef soc_periph_cordic_clk_src_t    cordic_clock_source_t;
#else
/**
 * @brief default type
 */
typedef int                             cordic_clock_source_t;
#endif

/**
 * @brief Input buffer descriptor for CORDIC operations
 */
typedef struct {
    uint32_t *p_data_arg1;                /*!< Pointer to input data argument 1 buffer */
    uint32_t *p_data_arg2;                /*!< Pointer to input data argument 2 buffer (can be NULL for single argument functions) */
} cordic_input_buffer_desc_t;

/**
 * @brief Output buffer descriptor for CORDIC operations
 */
typedef struct {
    uint32_t *p_data_res1;             /*!< Pointer to output result 1 buffer */
    uint32_t *p_data_res2;             /*!< Pointer to output result 2 buffer (can be NULL for single result) */
} cordic_output_buffer_desc_t;

/**
 * @brief CORDIC calculation function type
 */
typedef enum {
    ESP_CORDIC_FUNC_COS                = 0,   ///< Cosine function (can have both cos, sin results)
    ESP_CORDIC_FUNC_SIN,                      ///< Sine function (can have both sin, cos results)
    ESP_CORDIC_FUNC_PHASE,                    ///< Phase function (requires two arguments) (can have both phase, modulus results)
    ESP_CORDIC_FUNC_MODULUS,                  ///< Modulus function (requires two arguments) (can have both phase, modulus results)
    ESP_CORDIC_FUNC_ARCTAN,                   ///< Arctangent function
    ESP_CORDIC_FUNC_COSH,                     ///< Hyperbolic cosine function (can have both cosh, sinh results)
    ESP_CORDIC_FUNC_SINH,                     ///< Hyperbolic sine function (can have both sinh, cosh results)
    ESP_CORDIC_FUNC_ARCHTANH,                 ///< Hyperbolic arctangent function
    ESP_CORDIC_FUNC_LOGE,                     ///< Natural logarithm function
    ESP_CORDIC_FUNC_SQUARE_ROOT,              ///< Square root function
    ESP_CORDIC_FUNC_MAX,                      ///< Maximum function type (for boundary checking)
} cordic_func_t;

/**
 * @brief CORDIC data format size
 */
typedef enum {
    ESP_CORDIC_FORMAT_Q15 = 0,                  ///< Q15 format: 16-bit fixed-point (range: -1.0 to 1.0)
    ESP_CORDIC_FORMAT_Q31 = 1,                  ///< Q31 format: 32-bit fixed-point (range: -1.0 to 1.0)
    ESP_CORDIC_IQ_SIZE_MAX,                     ///< Maximum size type (for boundary checking)
} cordic_iq_format_t;

#ifdef __cplusplus
}
#endif
