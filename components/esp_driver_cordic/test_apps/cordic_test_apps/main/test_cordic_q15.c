/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "unity.h"
#include "soc/cordic_reg.h"
#include "hal/cordic_types.h"
#include "hal/cordic_ll.h"
#include "driver/cordic.h"
#include "driver/cordic_types.h"
#include "ccomp_timer.h"
#include "esp_random.h"
#include "test_utils.h"
#include "driver/gptimer.h"
#include "esp_private/cache_utils.h"
#define GLOBAL_IQ (15)
#include "IQmathLib.h"

#define PI 3.14159265358979323846
static uint32_t iq_pi_x = _IQ15(PI);

TEST_CASE("cordic install-uninstall test", "[cordic]")
{
    cordic_engine_handle_t engine = NULL;
    cordic_engine_config_t engine_config = {
        .clock_source = CORDIC_CLK_SRC_DEFAULT,
    };
    TEST_ESP_OK(cordic_new_engine(&engine_config, &engine));
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, cordic_new_engine(&engine_config, &engine));
    TEST_ESP_OK(cordic_delete_engine(engine));
}

TEST_CASE("test cosine and sine q15", "[cordic]")
{
    cordic_engine_handle_t engine = NULL;
    cordic_engine_config_t engine_config = {
        .clock_source = CORDIC_CLK_SRC_DEFAULT,
    };
    TEST_ESP_OK(cordic_new_engine(&engine_config, &engine));

    cordic_calculate_config_t calc_config = {
        .function = ESP_CORDIC_FUNC_COS,
        .iq_format = ESP_CORDIC_FORMAT_Q15,
        .iteration_count = 4,
        .scale_exp = 0,
    };

    // Test range: -π to π in Q15 format
    // In Q15 format: -0x8000 = -π, 0x0000 = 0, 0x7FFF ≈ π
    // Generate 32 test points uniformly distributed from -π to π
    uint32_t data_x[32] = {};
    for (size_t i = 0; i < 32; i++) {
        // Map i from [0, 31] to float range [-1.0, 1.0]
        // Linear interpolation: float_value = -1.0 + (i * 2.0 / 31)
        float float_value = -1.0f + (i * 2.0f / 31.0f);
        data_x[i] = cordic_convert_float_to_fixed(float_value, calc_config.iq_format);
    }
    uint32_t res1[32] = {};
    uint32_t res2[32] = {};

    cordic_input_buffer_desc_t input_buffer = {
        .p_data_arg1 = data_x,
        .p_data_arg2 = NULL,
    };
    cordic_output_buffer_desc_t output_buffer = {
        .p_data_res1 = res1,
        .p_data_res2 = res2,
    };

    TEST_ESP_OK(cordic_calculate_polling(engine, &calc_config, &input_buffer, &output_buffer, 32));

    // Print header
    printf("\n=== Comparison: Software vs Hardware (Q15 format, range: -π to π) ===\n");
    printf("Input(Q15) | Angle(rad) | SW_Cosine  | SW_Sine    | HW_Cosine  | HW_Sine    | Cos_Error  | Sin_Error\n");
    printf("-----------|------------|------------|------------|------------|------------|------------|----------\n");

    for (size_t i = 0; i < 32; i++) {
        // Convert Q15 input angle to radians
        // Q15 format: angle_rad = (int16_t)data_x / 32768.0 * π
        // Range: -π (data_x = -0x8000) to π (data_x = 0x7FFF)
        float angle_rad = cordic_convert_fixed_to_float(data_x[i], calc_config.iq_format) * M_PI;

        // Software calculation using standard math library
        float sw_cosine = cosf(angle_rad);
        float sw_sine = sinf(angle_rad);
        // Convert hardware results to float using cordic_convert_fixed_to_float
        float hw_cosine = cordic_convert_fixed_to_float(res1[i] & 0xFFFF, calc_config.iq_format);
        float hw_sine = cordic_convert_fixed_to_float(res2[i] & 0xFFFF, calc_config.iq_format);
        float cos_error = fabsf(sw_cosine - hw_cosine);
        float sin_error = fabsf(sw_sine - hw_sine);

        // Print comparison results
        printf("0x%04lx     | %10.6f | %10.6f | %10.6f | %10.6f | %10.6f | %10.6f | %10.6f",
               (data_x[i] & 0xFFFF), angle_rad, sw_cosine, sw_sine, hw_cosine, hw_sine, cos_error, sin_error);

        TEST_ASSERT_FLOAT_WITHIN(0.01f, sw_cosine, hw_cosine);
        TEST_ASSERT_FLOAT_WITHIN(0.01f, sw_sine, hw_sine);
        printf("\n");
    }
    TEST_ESP_OK(cordic_delete_engine(engine));
}

TEST_CASE("test phase and module q15", "[cordic]")
{
    cordic_engine_handle_t engine = NULL;
    cordic_engine_config_t engine_config = {
        .clock_source = CORDIC_CLK_SRC_DEFAULT,
    };
    TEST_ESP_OK(cordic_new_engine(&engine_config, &engine));

    cordic_calculate_config_t calc_config = {
        .function = ESP_CORDIC_FUNC_PHASE,
        .iq_format = ESP_CORDIC_FORMAT_Q15,
        .iteration_count = 4,
        .scale_exp = 0,
    };

    uint32_t data_x[60] = {};
    uint32_t data_y[60] = {};
    for (size_t i = 0; i < 60; i++) {
        // Generate x and y values in range (-1, 1)
        // Map i from [0, 59] to float range [-1.0, 1.0]
        // Use different patterns for x and y to cover more combinations
        float x_float = -1.0f + (i * 2.0f / 59.0f);
        float y_float = -1.0f + (((i * 3) % 60) * 2.0f / 59.0f);
        data_x[i] = cordic_convert_float_to_fixed(x_float, calc_config.iq_format);
        data_y[i] = cordic_convert_float_to_fixed(y_float, calc_config.iq_format);
    }
    uint32_t res1[60] = {};
    uint32_t res2[60] = {};

    cordic_input_buffer_desc_t input_buffer = {
        .p_data_arg1 = data_x,
        .p_data_arg2 = data_y,
    };
    cordic_output_buffer_desc_t output_buffer = {
        .p_data_res1 = res1,
        .p_data_res2 = res2,
    };

    TEST_ESP_OK(cordic_calculate_polling(engine, &calc_config, &input_buffer, &output_buffer, 60));

    // Print header
    printf("\n=== Comparison: Software vs Hardware (Phase and Module Q15 format, range: -1 to 1) ===\n");
    printf("Input_X(Q15) | Input_Y(Q15) | SW_Phase   | SW_Module  | HW_Phase   | HW_Module  | Phase_Err  | Module_Err\n");
    printf("-------------|-------------|------------|------------|------------|------------|------------|-----------\n");

    for (size_t i = 0; i < 60; i++) {
        // Convert Q15 format inputs to float
        // Q15 format: float_value = (int16_t)q15_value / 32768.0f
        float x_float = cordic_convert_fixed_to_float(data_x[i], calc_config.iq_format);
        float y_float = cordic_convert_fixed_to_float(data_y[i], calc_config.iq_format);

        // Software calculation: phase = atan2(y, x) / π, module = sqrt(x^2 + y^2)
        float sw_phase = atan2f(y_float, x_float) / M_PI;
        float sw_module = sqrtf(x_float * x_float + y_float * y_float);
        // Convert hardware results to float using cordic_convert_fixed_to_float
        uint16_t hw_module_q15 = (uint16_t)(res2[i] & 0xFFFF);
        if (hw_module_q15 == 0x7FFF) {
            continue; // Skip this entry, do not print
        }
        float hw_phase = cordic_convert_fixed_to_float(res1[i] & 0xFFFF, calc_config.iq_format);
        float hw_module = cordic_convert_fixed_to_float(res2[i] & 0xFFFF, calc_config.iq_format);
        float phase_error = fabsf(sw_phase - hw_phase);
        float module_error = fabsf(sw_module - hw_module);

        // Print comparison results
        printf("%10.6f | %10.6f | %10.6f | %10.6f | %10.6f | %10.6f | %10.6f | %10.6f",
               x_float, y_float, sw_phase, sw_module, hw_phase, hw_module, phase_error, module_error);

        TEST_ASSERT_FLOAT_WITHIN(0.001f, sw_phase, hw_phase);
        TEST_ASSERT_FLOAT_WITHIN(0.001f, sw_module, hw_module);
        printf("\n");
    }
    TEST_ESP_OK(cordic_delete_engine(engine));
}

TEST_CASE("test arctan q15", "[cordic]")
{
    cordic_engine_handle_t engine = NULL;
    cordic_engine_config_t engine_config = {
        .clock_source = CORDIC_CLK_SRC_DEFAULT,
    };
    TEST_ESP_OK(cordic_new_engine(&engine_config, &engine));

    cordic_calculate_config_t calc_config = {
        .function = ESP_CORDIC_FUNC_ARCTAN,
        .iq_format = ESP_CORDIC_FORMAT_Q15,
        .iteration_count = 4,
        .scale_exp = 0,
    };

    // Test range: data_x in (-1, 1) in Q15 format
    // In Q15 format: -0x8000 = -1.0, 0x0000 = 0.0, 0x7FFF ≈ 0.999969
    // Generate 32 test points uniformly distributed from -1 to 1
    uint32_t data_x[32] = {};
    for (size_t i = 0; i < 32; i++) {
        // Map i from [0, 31] to float range [-1.0, 1.0] (avoiding exact -1 and 1)
        float x_float = -1.0f + (i * 2.0f / 31.0f);
        data_x[i] = cordic_convert_float_to_fixed(x_float, calc_config.iq_format);
    }
    uint32_t res1[32] = {};
    uint32_t res2[32] = {};

    cordic_input_buffer_desc_t input_buffer = {
        .p_data_arg1 = data_x,
        .p_data_arg2 = NULL,
    };
    cordic_output_buffer_desc_t output_buffer = {
        .p_data_res1 = res1,
        .p_data_res2 = res2,
    };

    TEST_ESP_OK(cordic_calculate_polling(engine, &calc_config, &input_buffer, &output_buffer, 32));

    // Print header
    printf("\n=== Comparison: Software vs Hardware (Arctan Q15 format, input range: -1 to 1) ===\n");
    printf("Input(Q15) | X_Float    | SW_Arctan  | HW_Arctan  | Error      | Arctan(rad)\n");
    printf("-----------|------------|------------|------------|------------|------------\n");

    for (size_t i = 0; i < 32; i++) {
        float x_float = cordic_convert_fixed_to_float(data_x[i], calc_config.iq_format);
        float sw_arctan_rad = atanf(x_float);
        // Normalize to [-1, 1] range (divide by π for Q15 format output)
        // Q15 arctan output is typically normalized: arctan(x) / π
        float sw_arctan_normalized = sw_arctan_rad / M_PI;
        // Convert hardware result to float using cordic_convert_fixed_to_float
        float hw_arctan_normalized = cordic_convert_fixed_to_float(res1[i] & 0xFFFF, calc_config.iq_format);
        float arctan_error = fabsf(sw_arctan_normalized - hw_arctan_normalized);

        // Print comparison results
        printf("0x%04lx     | %10.6f | %10.6f | %10.6f | %10.6f | %10.6f",
               data_x[i], x_float, sw_arctan_normalized, hw_arctan_normalized, arctan_error, sw_arctan_rad);

        // Highlight significant errors
        TEST_ASSERT_FLOAT_WITHIN(0.001f, sw_arctan_normalized, hw_arctan_normalized);
        printf("\n");
    }
    TEST_ESP_OK(cordic_delete_engine(engine));
}

TEST_CASE("test hyperbolic cosine and sine q15", "[cordic]")
{
    cordic_engine_handle_t engine = NULL;
    cordic_engine_config_t engine_config = {
        .clock_source = CORDIC_CLK_SRC_DEFAULT,
    };
    TEST_ESP_OK(cordic_new_engine(&engine_config, &engine));

    cordic_calculate_config_t calc_config = {
        .function = ESP_CORDIC_FUNC_COSH,
        .iq_format = ESP_CORDIC_FORMAT_Q15,
        .iteration_count = 4,
        .scale_exp = 1,
    };

    // Test range: data_x in (-0.559, 0.559) in Q15 format
    // In Q15 format: -0.559 * 32768 = -18320.512, 0.559 * 32768 = 18320.512
    // Generate 32 test points uniformly distributed from -0.559 to 0.559
    uint32_t data_x[32] = {};
    for (size_t i = 0; i < 32; i++) {
        // Map i from [0, 31] to float range [-0.559, 0.559]
        float x_float = -0.559f + (i * (0.559f - (-0.559f)) / 31.0f);
        data_x[i] = cordic_convert_float_to_fixed(x_float, calc_config.iq_format);
    }
    uint32_t res1[32] = {};
    uint32_t res2[32] = {};

    cordic_input_buffer_desc_t input_buffer = {
        .p_data_arg1 = data_x,
        .p_data_arg2 = NULL,
    };
    cordic_output_buffer_desc_t output_buffer = {
        .p_data_res1 = res1,
        .p_data_res2 = res2,
    };

    TEST_ESP_OK(cordic_calculate_polling(engine, &calc_config, &input_buffer, &output_buffer, 32));

    // Print header
    printf("\n=== Comparison: Software vs Hardware (Hyperbolic Cosine and Sine Q15 format, input range: -0.559 to 0.559) ===\n");
    printf("X_Float      | SW_Cosh    | SW_Sinh    | HW_Cosh    | HW_Sinh    | Cosh_Err   | Sinh_Err\n");
    printf("-------------|------------|------------|------------|------------|------------|----------\n");

    for (size_t i = 0; i < 32; i++) {
        // Convert Q15 format input to float
        // Q15 format: float_value = (int16_t)q15_value / 32768.0f
        float x_float = cordic_convert_fixed_to_float(data_x[i], calc_config.iq_format) * 2.0f; // multi 2

        // Software calculation using standard math library
        float sw_cosh = cosh(x_float);
        float sw_sinh = sinh(x_float);
        // Convert hardware results to float using cordic_convert_fixed_to_float
        float hw_cosh_temp = cordic_convert_fixed_to_float(res1[i] & 0xFFFF, calc_config.iq_format);
        float hw_sinh_temp = cordic_convert_fixed_to_float(res2[i] & 0xFFFF, calc_config.iq_format);
        float hw_cosh = hw_cosh_temp * 2.0f; // multi 2
        float hw_sinh = hw_sinh_temp * 2.0f; // multi 2

        // Calculate errors
        float cosh_error = fabsf(sw_cosh - hw_cosh);
        float sinh_error = fabsf(sw_sinh - hw_sinh);

        // Print comparison results
        printf("%10.6f   | %10.6f | %10.6f | %10.6f | %10.6f | %10.6f | %10.6f",
               x_float, sw_cosh, sw_sinh, hw_cosh, hw_sinh, cosh_error, sinh_error);

        TEST_ASSERT_FLOAT_WITHIN(0.001f, sw_cosh, hw_cosh);
        TEST_ASSERT_FLOAT_WITHIN(0.001f, sw_sinh, hw_sinh);
        printf("\n");
    }
    TEST_ESP_OK(cordic_delete_engine(engine));
}

TEST_CASE("test arctanh q15", "[cordic]")
{
    cordic_engine_handle_t engine = NULL;
    cordic_engine_config_t engine_config = {
        .clock_source = CORDIC_CLK_SRC_DEFAULT,
    };
    TEST_ESP_OK(cordic_new_engine(&engine_config, &engine));

    cordic_calculate_config_t calc_config = {
        .function = ESP_CORDIC_FUNC_ARCHTANH,
        .iq_format = ESP_CORDIC_FORMAT_Q15,
        .iteration_count = 4,
        .scale_exp = 1,
    };

    // Test range: data_x in (-0.403, 0.403) in Q15 format
    // In Q15 format: -0.403 * 32768 = -13205.504, 0.403 * 32768 = 13205.504
    // Generate 32 test points uniformly distributed from -0.403 to 0.403
    uint32_t data_x[32] = {};
    for (size_t i = 0; i < 32; i++) {
        // Map i from [0, 31] to float range [-0.403, 0.403]
        float x_float = -0.403f + (i * (0.403f - (-0.403f)) / 31.0f);
        data_x[i] = cordic_convert_float_to_fixed(x_float, calc_config.iq_format);
    }
    uint32_t res1[32] = {};
    uint32_t res2[32] = {};

    cordic_input_buffer_desc_t input_buffer = {
        .p_data_arg1 = data_x,
        .p_data_arg2 = NULL,
    };
    cordic_output_buffer_desc_t output_buffer = {
        .p_data_res1 = res1,
        .p_data_res2 = res2,
    };

    TEST_ESP_OK(cordic_calculate_polling(engine, &calc_config, &input_buffer, &output_buffer, 32));

    // Print header
    printf("\n=== Comparison: Software vs Hardware (Arctanh Q15 format, input range: -0.403 to 0.403) ===\n");
    printf("Input(Q15) | X_Float    | SW_Arctanh | HW_Arctanh | Error      | Arctanh(rad)\n");
    printf("-----------|------------|------------|------------|------------|------------\n");

    for (size_t i = 0; i < 32; i++) {
        // Convert Q15 format input to float
        // Q15 format: float_value = (int16_t)q15_value / 32768.0f
        float x_float = cordic_convert_fixed_to_float(data_x[i], calc_config.iq_format) * 2.0f; // multi 2

        // Software calculation: arctanh(x) in radians
        float sw_arctanh_rad = atanh(x_float);
        // Normalize to [-1, 1] range (divide by π for Q15 format output)
        // Q15 arctanh output is typically normalized: arctanh(x) / π
        float sw_arctanh_normalized = sw_arctanh_rad;
        // Convert hardware result to float using cordic_convert_fixed_to_float
        float hw_arctanh_temp = cordic_convert_fixed_to_float(res1[i] & 0xFFFF, calc_config.iq_format);
        float hw_arctanh_normalized = hw_arctanh_temp * 2.0f; // multi 2
        float arctanh_error = fabsf(sw_arctanh_normalized - hw_arctanh_normalized);

        // Print comparison results
        printf("0x%04lx     | %10.6f | %10.6f | %10.6f | %10.6f | %10.6f",
               data_x[i] & 0xFFFF, x_float, sw_arctanh_normalized, hw_arctanh_normalized, arctanh_error, sw_arctanh_rad);

        TEST_ASSERT_FLOAT_WITHIN(0.001f, sw_arctanh_normalized, hw_arctanh_normalized);
        printf("\n");
    }
    TEST_ESP_OK(cordic_delete_engine(engine));
}

TEST_CASE("test natural logarithm q15", "[cordic]")
{
    cordic_engine_handle_t engine = NULL;
    cordic_engine_config_t engine_config = {
        .clock_source = CORDIC_CLK_SRC_DEFAULT,
    };
    TEST_ESP_OK(cordic_new_engine(&engine_config, &engine));

    cordic_calculate_config_t calc_config = {
        .function = ESP_CORDIC_FUNC_LOGE,
        .iq_format = ESP_CORDIC_FORMAT_Q15,
        .iteration_count = 4,
        .scale_exp = 1,
    };

    // Test range: x in (0.1069, 9.3573)
    // Hardware parameter: (x-1)/(x+1) * (2^-1), which should be in [-0.403, 0.403]
    // Hardware output: (ln x) * (2^-2), so we need to multiply by 4 to get ln(x)
    // Generate 32 test points uniformly distributed from 0.1069 to 9.3573
    float x_values[32] = {};
    uint32_t data_x[32] = {};
    for (size_t i = 0; i < 32; i++) {
        // Map i from [0, 31] to x range [0.1069, 9.3573]
        float x = 0.1069f + (i * (9.3573f - 0.1069f) / 31.0f);
        x_values[i] = x;

        // Calculate hardware parameter: (x-1)/(x+1) * 0.5
        float hw_param = ((x - 1.0f) / (x + 1.0f)) * 0.5f;

        // Convert to Q15 format
        data_x[i] = cordic_convert_float_to_fixed(hw_param, calc_config.iq_format);
    }
    uint32_t res1[32] = {};
    uint32_t res2[32] = {};

    cordic_input_buffer_desc_t input_buffer = {
        .p_data_arg1 = data_x,
        .p_data_arg2 = NULL,
    };
    cordic_output_buffer_desc_t output_buffer = {
        .p_data_res1 = res1,
        .p_data_res2 = res2,
    };

    TEST_ESP_OK(cordic_calculate_polling(engine, &calc_config, &input_buffer, &output_buffer, 32));

    // Print header
    printf("\n=== Comparison: Software vs Hardware (Natural Logarithm Q15 format, x range: 0.1069 to 9.3573) ===\n");
    printf("X_Value     | HW_Param   | SW_LnX     | HW_LnX     | Error      | LnX\n");
    printf("------------|------------|------------|------------|------------|------------\n");

    for (size_t i = 0; i < 32; i++) {
        float x = x_values[i];

        // Calculate hardware parameter: (x-1)/(x+1) * 0.5
        float hw_param = ((x - 1.0f) / (x + 1.0f)) * 0.5f;

        // Software calculation: ln(x)
        float sw_lnx = logf(x);
        // Convert hardware result to float using cordic_convert_fixed_to_float
        float hw_lnx_scaled = cordic_convert_fixed_to_float(res1[i] & 0xFFFF, calc_config.iq_format);
        float hw_lnx = hw_lnx_scaled * 4.0f;
        float lnx_error = fabsf(sw_lnx - hw_lnx);

        // Print comparison results
        printf("%10.6f   | %10.6f | %10.6f | %10.6f | %10.6f | %10.6f",
               x, hw_param, sw_lnx, hw_lnx, lnx_error, sw_lnx);
        TEST_ASSERT_FLOAT_WITHIN(0.001f, sw_lnx, hw_lnx);
        printf("\n");
    }
    TEST_ESP_OK(cordic_delete_engine(engine));
}

TEST_CASE("test square root q15", "[cordic]")
{
    cordic_engine_handle_t engine = NULL;
    cordic_engine_config_t engine_config = {
        .clock_source = CORDIC_CLK_SRC_DEFAULT,
    };
    TEST_ESP_OK(cordic_new_engine(&engine_config, &engine));

    cordic_calculate_config_t calc_config = {
        .function = ESP_CORDIC_FUNC_SQUARE_ROOT,
        .iq_format = ESP_CORDIC_FORMAT_Q15,
        .iteration_count = 4,
        .scale_exp = 0,
    };

    // Test range: x in (0.1069, 1)
    // Hardware input: x (directly)
    // Hardware output: sqrt(x)
    // Generate 32 test points uniformly distributed from 0.1069 to 0.999 (avoiding exact 1)
    float x_values[32] = {};
    uint32_t data_x[32] = {};
    for (size_t i = 0; i < 32; i++) {
        // Map i from [0, 31] to x range (0.1069, 1)
        float x = 0.1069f + (i * (0.999f - 0.1069f) / 31.0f);
        x_values[i] = x;

        // Convert x to Q15 format
        data_x[i] = cordic_convert_float_to_fixed(x, calc_config.iq_format);
    }
    uint32_t res1[32] = {};
    uint32_t res2[32] = {};

    cordic_input_buffer_desc_t input_buffer = {
        .p_data_arg1 = data_x,
        .p_data_arg2 = NULL,
    };
    cordic_output_buffer_desc_t output_buffer = {
        .p_data_res1 = res1,
        .p_data_res2 = res2,
    };

    TEST_ESP_OK(cordic_calculate_polling(engine, &calc_config, &input_buffer, &output_buffer, 32));

    // Print header
    printf("\n=== Comparison: Software vs Hardware (Square Root Q15 format, x range: 0.1069 to 1) ===\n");
    printf("X_Value     | SW_SqrtX   | HW_SqrtX   | Error\n");
    printf("------------|------------|------------|----------\n");

    for (size_t i = 0; i < 32; i++) {
        float x = x_values[i];

        float sw_sqrtx = sqrtf(x);
        // Convert hardware result to float using cordic_convert_fixed_to_float
        float hw_sqrtx = cordic_convert_fixed_to_float(res1[i] & 0xFFFF, calc_config.iq_format);
        float sqrtx_error = fabsf(sw_sqrtx - hw_sqrtx);

        // Print comparison results
        printf("%10.6f   | %10.6f | %10.6f | %10.6f",
               x, sw_sqrtx, hw_sqrtx, sqrtx_error);

        TEST_ASSERT_FLOAT_WITHIN(0.001f, sw_sqrtx, hw_sqrtx);
        printf("\n");
    }
    TEST_ESP_OK(cordic_delete_engine(engine));
}

TEST_CASE("test arctan scale 2 q15", "[cordic]")
{
    cordic_engine_handle_t engine = NULL;
    cordic_engine_config_t engine_config = {
        .clock_source = CORDIC_CLK_SRC_DEFAULT,
    };
    TEST_ESP_OK(cordic_new_engine(&engine_config, &engine));

    cordic_calculate_config_t calc_config = {
        .function = ESP_CORDIC_FUNC_ARCTAN,
        .iq_format = ESP_CORDIC_FORMAT_Q15,
        .iteration_count = 4,
        .scale_exp = 2,  // Scale = 2: hardware input is (x) * 2^(-2) = x / 4
    };

    // Test range: hardware input (x * 2^(-2)) should be in [-1, 1]
    // So actual x range should be [-4, 4] in Q15 format
    // Note: Q15 format range is [-1, 1], but we need to input values that represent [-4, 4]
    // Hardware will scale: x_hw = x * 2^(-2) = x / 4, so x should be in [-4, 4] range
    // Generate 32 test points uniformly distributed from -4 to 4
    uint32_t data_x[32] = {};
    for (size_t i = 0; i < 32; i++) {
        // Map i from [0, 31] to float range [-4, 4]
        float x_float = -4.0f + (i * 8.0f / 31.0f);
        // Convert to Q15 format: x_float is in [-4, 4], but Q15 range is [-1, 1]
        // So we need to represent x_float / 4 in Q15 format
        float x_normalized = x_float / 4.0f;  // Normalize to [-1, 1] range
        data_x[i] = cordic_convert_float_to_fixed(x_normalized, calc_config.iq_format);
    }
    uint32_t res1[32] = {};
    uint32_t res2[32] = {};

    cordic_input_buffer_desc_t input_buffer = {
        .p_data_arg1 = data_x,
        .p_data_arg2 = NULL,
    };
    cordic_output_buffer_desc_t output_buffer = {
        .p_data_res1 = res1,
        .p_data_res2 = res2,
    };

    TEST_ESP_OK(cordic_calculate_polling(engine, &calc_config, &input_buffer, &output_buffer, 32));

    // Print header
    printf("\n=== Comparison: Software vs Hardware (Arctan Q15 format, scale=2, input range: -4 to 4) ===\n");
    printf("Input(Q15) | X_Float    | SW_Arctan  | HW_Arctan  | Error      | Arctan(rad)\n");
    printf("-----------|------------|------------|------------|------------|------------\n");

    for (size_t i = 0; i < 32; i++) {
        // Convert Q15 input to float using cordic_convert_fixed_to_float
        float x_normalized = cordic_convert_fixed_to_float(data_x[i], calc_config.iq_format);  // This is in [-1, 1] range
        // Reconstruct original x value: x_float = x_normalized * 4
        float x_float = x_normalized * 4.0f;  // Original x in [-4, 4] range
        // Hardware scales input: x_scaled = x_float * 2^(-2) = x_float / 4 = x_normalized
        // float x_scaled = x_float / 4.0f;  // This equals x_normalized

        // Software calculation: arctan(x_scaled) / π
        float sw_arctan_rad = atanf(x_float);
        float sw_arctan_normalized = sw_arctan_rad / M_PI;

        // Hardware output is already arctan(x * 2^(-2)) / π = arctan(x_scaled) / π
        // Convert Q15 result to float using cordic_convert_fixed_to_float
        // Note: res1[i] * 4 is used for scale adjustment, but we need to convert the raw value first
        float hw_arctan_temp = cordic_convert_fixed_to_float(res1[i] & 0xFFFF, calc_config.iq_format);
        float hw_arctan_normalized = hw_arctan_temp * 4;

        float arctan_error = fabsf(sw_arctan_normalized - hw_arctan_normalized);

        // Print comparison results
        printf("0x%04lx     | %10.6f | %10.6f | %10.6f | %10.6f | %10.6f",
               data_x[i] & 0xFFFF, x_float, sw_arctan_normalized, hw_arctan_normalized, arctan_error, sw_arctan_rad);

        // Highlight significant errors
        TEST_ASSERT_FLOAT_WITHIN(0.001f, sw_arctan_normalized, hw_arctan_normalized);
        printf("\n");
    }
    TEST_ESP_OK(cordic_delete_engine(engine));
}

TEST_CASE("test cordic performance should quicker than software iqmath", "[cordic]")
{
    cordic_engine_handle_t engine = NULL;
    cordic_engine_config_t engine_config = {
        .clock_source = CORDIC_CLK_SRC_DEFAULT,
    };
    TEST_ESP_OK(cordic_new_engine(&engine_config, &engine));

    cordic_calculate_config_t calc_config = {
        .function = ESP_CORDIC_FUNC_COS,
        .iq_format = ESP_CORDIC_FORMAT_Q15,
        .iteration_count = 4,
        .scale_exp = 0,
    };

    const int test_seed = 999;
    srand(test_seed);
    uint32_t iq_radians[36] = {};
    uint32_t data_x[36] = {};
    for (size_t i = 0; i < 36; i++) {
        uint32_t rand_value = rand() % (0xFFFF + 1);
        data_x[i] = rand_value;
        iq_radians[i] = _IQmpy(rand_value, iq_pi_x);
    }

    uint32_t res1[36] = {};
    uint32_t res2[36] = {};

    cordic_input_buffer_desc_t input_buffer = {
        .p_data_arg1 = data_x,
        .p_data_arg2 = NULL,
    };
    cordic_output_buffer_desc_t output_buffer = {
        .p_data_res1 = res1,
        .p_data_res2 = res2,
    };

    ccomp_timer_start();
    TEST_ESP_OK(cordic_calculate_polling(engine, &calc_config, &input_buffer, &output_buffer, 36));

    int64_t cordic_time = ccomp_timer_stop();
    printf("CORDIC time: %lld us\n", cordic_time);

    // Use volatile variables to prevent compiler optimization
    volatile uint32_t dummy_cos = 0;
    volatile uint32_t dummy_sin = 0;

    ccomp_timer_start();
    for (uint32_t i = 0; i < 36; i++) {
        // Assign results to volatile variables to prevent compiler optimization
        dummy_cos = _IQcos(iq_radians[i]);
        dummy_sin = _IQsin(iq_radians[i]);
    }
    int64_t iqmath_time = ccomp_timer_stop();
    printf("IQMATH time: %lld us\n", iqmath_time);

    IDF_LOG_PERFORMANCE("CORDIC_TIME", "Cordic time sine cosine is %lld us", cordic_time);
    TEST_ASSERT_LESS_THAN(iqmath_time, cordic_time);

    // Use the volatile variables to prevent dead code elimination
    (void)dummy_cos;
    (void)dummy_sin;
    TEST_ESP_OK(cordic_delete_engine(engine));

}

#if CONFIG_CORDIC_ONESHOT_CTRL_FUNC_IN_IRAM

/*---------------------------------------------------------------
        CORDIC work with ISR
---------------------------------------------------------------*/
typedef struct {
    TaskHandle_t task_handle;             //Task handle
    cordic_engine_handle_t engine_handle; // engine handle
    uint32_t res1;
    uint32_t res2;
} test_cordic_isr_ctx_t;

static bool IRAM_ATTR s_alarm_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    test_cordic_isr_ctx_t *test_ctx = (test_cordic_isr_ctx_t *)user_data;
    BaseType_t high_task_wakeup;

    /**
     * This test would disable the cache, so must put code between cache disable and cache enable in IRAM
     */
    uint32_t data_x = 0x123;
    uint32_t res1 = 0;
    uint32_t res2 = 0;

    cordic_input_buffer_desc_t input_buffer = {
        .p_data_arg1 = &data_x,
        .p_data_arg2 = NULL,
    };
    cordic_output_buffer_desc_t output_buffer = {
        .p_data_res1 = &res1,
        .p_data_res2 = &res2,
    };

    cordic_calculate_config_t calc_config = {
        .function = ESP_CORDIC_FUNC_COS,
        .iq_format = ESP_CORDIC_FORMAT_Q15,
        .iteration_count = 4,
        .scale_exp = 0,
    };
    spi_flash_disable_interrupts_caches_and_other_cpu();
    TEST_ESP_OK(cordic_calculate_polling(test_ctx->engine_handle, &calc_config, &input_buffer, &output_buffer, 1));
    spi_flash_enable_interrupts_caches_and_other_cpu();

    test_ctx->res1 = res1;
    test_ctx->res2 = res2;

    vTaskNotifyGiveFromISR(test_ctx->task_handle, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

TEST_CASE("cordic can calculation in ISR", "[cordic]")
{
    static test_cordic_isr_ctx_t isr_test_ctx = {};
    isr_test_ctx.engine_handle = NULL;
    isr_test_ctx.task_handle = xTaskGetCurrentTaskHandle();

    cordic_engine_config_t engine_config = {
        .clock_source = CORDIC_CLK_SRC_DEFAULT,
    };
    TEST_ESP_OK(cordic_new_engine(&engine_config, &isr_test_ctx.engine_handle));

    //-------------GPTimer Init & Config---------------//
    gptimer_handle_t timer = NULL;
    gptimer_config_t timer_config = {
        .resolution_hz = 1 * 1000 * 1000,
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
    };
    TEST_ESP_OK(gptimer_new_timer(&timer_config, &timer));

    gptimer_event_callbacks_t cbs = {
        .on_alarm = s_alarm_callback,
    };
    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = 100000, // 100ms
    };
    TEST_ESP_OK(gptimer_set_alarm_action(timer, &alarm_config));
    TEST_ESP_OK(gptimer_register_event_callbacks(timer, &cbs, &isr_test_ctx));

    printf("start timer\r\n");
    TEST_ESP_OK(gptimer_enable(timer));
    TEST_ESP_OK(gptimer_start(timer));
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
    esp_rom_printf(DRAM_STR("CORDIC calculation done, cos is 0x%lx, sine is 0x%lx\n"), isr_test_ctx.res1, isr_test_ctx.res2);
    TEST_ESP_OK(gptimer_stop(timer));

    TEST_ESP_OK(gptimer_disable(timer));
    TEST_ESP_OK(gptimer_del_timer(timer));
    TEST_ESP_OK(cordic_delete_engine(isr_test_ctx.engine_handle));

}

#endif
