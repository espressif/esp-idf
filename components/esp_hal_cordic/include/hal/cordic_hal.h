/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include "hal/cordic_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cordic_dev_t *cordic_soc_handle_t; // cordic SOC layer handle

// Forward declaration for function pointer types
typedef struct cordic_hal_context_t cordic_hal_context_t;

// Function pointer type for setting CORDIC arguments
typedef void (*cordic_hal_set_argument_func_t)(cordic_hal_context_t *hal, const cordic_input_buffer_desc_t *input_buffer, size_t index);

// Function pointer type for getting CORDIC results
typedef void (*cordic_hal_get_result_func_t)(cordic_hal_context_t *hal, cordic_output_buffer_desc_t *output_buffer, size_t index);

// Function pointer arrays for setting arguments
extern const cordic_hal_set_argument_func_t cordic_hal_set_argument_funcs[ESP_CORDIC_IQ_SIZE_MAX][2];
extern const cordic_hal_get_result_func_t cordic_hal_get_result_funcs[ESP_CORDIC_IQ_SIZE_MAX];

/**
 * Context that should be maintained by both the driver and the HAL
 */
typedef struct cordic_hal_context_t {
    cordic_soc_handle_t dev; // cordic SOC layer handle (i.e. register base address)
} cordic_hal_context_t;

/**
 * @brief Initialize the cordic codec HAL driver
 *
 * @param hal: cordic codec HAL context
 */
void cordic_hal_init(cordic_hal_context_t *hal);

/**
 * @brief Deinitialize the cordic codec HAL driver
 *
 * @param hal: cordic codec HAL context
 */
void cordic_hal_deinit(cordic_hal_context_t *hal);

/**
 * @brief Set CORDIC arguments for Q15 format with two arguments (PHASE_FUNC, MODULUS_FUNC)
 *
 * @param hal: cordic HAL context
 * @param input_buffer: input buffer descriptor (HAL layer type)
 * @param index: data index
 */
void cordic_hal_set_argument_q15_two_args(cordic_hal_context_t *hal, const cordic_input_buffer_desc_t *input_buffer, size_t index);

/**
 * @brief Set CORDIC arguments for Q15 format with one argument
 *
 * @param hal: cordic HAL context
 * @param input_buffer: input buffer descriptor (HAL layer type)
 * @param index: data index
 */
void cordic_hal_set_argument_q15_one_arg(cordic_hal_context_t *hal, const cordic_input_buffer_desc_t *input_buffer, size_t index);

/**
 * @brief Set CORDIC arguments for Q31 format with two arguments (PHASE_FUNC, MODULUS_FUNC)
 *
 * @param hal: cordic HAL context
 * @param input_buffer: input buffer descriptor (HAL layer type)
 * @param index: data index
 */
void cordic_hal_set_argument_q31_two_args(cordic_hal_context_t *hal, const cordic_input_buffer_desc_t *input_buffer, size_t index);

/**
 * @brief Set CORDIC arguments for Q31 format with one argument
 *
 * @param hal: cordic HAL context
 * @param input_buffer: input buffer descriptor (HAL layer type)
 * @param index: data index
 */
void cordic_hal_set_argument_q31_one_arg(cordic_hal_context_t *hal, const cordic_input_buffer_desc_t *input_buffer, size_t index);

/**
 * @brief Get CORDIC results for Q15 format
 *
 * @param hal: cordic HAL context
 * @param output_buffer: output buffer descriptor (HAL layer type)
 * @param index: data index
 */
void cordic_hal_get_result_q15(cordic_hal_context_t *hal, cordic_output_buffer_desc_t *output_buffer, size_t index);

/**
 * @brief Get CORDIC results for Q31 format
 *
 * @param hal: cordic HAL context
 * @param output_buffer: output buffer descriptor (HAL layer type)
 * @param index: data index
 */
void cordic_hal_get_result_q31(cordic_hal_context_t *hal, cordic_output_buffer_desc_t *output_buffer, size_t index);

#ifdef __cplusplus
}
#endif
