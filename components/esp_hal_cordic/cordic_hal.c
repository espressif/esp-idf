/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include "hal/cordic_hal.h"
#include "hal/cordic_ll.h"

// Function pointer arrays for setting arguments
// Index: [format][argument_count]
// format: 0 = Q15, 1 = Q31
// argument_count: 0 = one_arg, 1 = two_args
const cordic_hal_set_argument_func_t cordic_hal_set_argument_funcs[ESP_CORDIC_IQ_SIZE_MAX][2] = {
    [ESP_CORDIC_FORMAT_Q15] = {
        [0] = cordic_hal_set_argument_q15_one_arg,
        [1] = cordic_hal_set_argument_q15_two_args,
    },
    [ESP_CORDIC_FORMAT_Q31] = {
        [0] = cordic_hal_set_argument_q31_one_arg,
        [1] = cordic_hal_set_argument_q31_two_args,
    },
};

// Function pointer array for getting results
// Index: [format]
// format: 0 = Q15, 1 = Q31
const cordic_hal_get_result_func_t cordic_hal_get_result_funcs[ESP_CORDIC_IQ_SIZE_MAX] = {
    [ESP_CORDIC_FORMAT_Q15] = cordic_hal_get_result_q15,
    [ESP_CORDIC_FORMAT_Q31] = cordic_hal_get_result_q31,
};

void cordic_hal_init(cordic_hal_context_t *hal)
{
    hal->dev = CORDIC_LL_GET_HW();
}

void cordic_hal_deinit(cordic_hal_context_t *hal)
{
    hal->dev = NULL;
}

void cordic_hal_set_argument_q15_two_args(cordic_hal_context_t *hal, const cordic_input_buffer_desc_t *input_buffer, size_t index)
{
    // Pack two 16-bit values into one 32-bit register
    cordic_ll_set_calculate_argument_1(hal->dev, (uint32_t)(input_buffer->p_data_arg2[index] << 16) | input_buffer->p_data_arg1[index]);
}

// Q15 format with one argument
void cordic_hal_set_argument_q15_one_arg(cordic_hal_context_t *hal, const cordic_input_buffer_desc_t *input_buffer, size_t index)
{
    cordic_ll_set_calculate_argument_1(hal->dev, input_buffer->p_data_arg1[index]);
}

void cordic_hal_set_argument_q31_two_args(cordic_hal_context_t *hal, const cordic_input_buffer_desc_t *input_buffer, size_t index)
{
    // Set two separate 32-bit registers
    cordic_ll_set_calculate_argument_1(hal->dev, input_buffer->p_data_arg1[index]);
    cordic_ll_set_calculate_argument_2(hal->dev, input_buffer->p_data_arg2[index]);
}

void cordic_hal_set_argument_q31_one_arg(cordic_hal_context_t *hal, const cordic_input_buffer_desc_t *input_buffer, size_t index)
{
    cordic_ll_set_calculate_argument_1(hal->dev, input_buffer->p_data_arg1[index]);
}

void cordic_hal_get_result_q15(cordic_hal_context_t *hal, cordic_output_buffer_desc_t *output_buffer, size_t index)
{
    // For Q15_SIZE: extract two 16-bit values from one 32-bit register
    uint32_t res = cordic_ll_get_calculate_result_1(hal->dev);
    output_buffer->p_data_res1[index] = (res & 0x0000FFFF);
    if (output_buffer->p_data_res2 != NULL) {
        output_buffer->p_data_res2[index] = (res >> 16);
    }
}

void cordic_hal_get_result_q31(cordic_hal_context_t *hal, cordic_output_buffer_desc_t *output_buffer, size_t index)
{
    // For Q31_SIZE: get two separate 32-bit results
    uint32_t res = cordic_ll_get_calculate_result_1(hal->dev);
    output_buffer->p_data_res1[index] = res;
    if (output_buffer->p_data_res2 != NULL) {
        uint32_t res2 = cordic_ll_get_calculate_result_2(hal->dev);
        output_buffer->p_data_res2[index] = res2;
    }
}
