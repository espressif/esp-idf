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

TEST_CASE("test cosine and sine q31", "[cordic]")
{
    cordic_engine_handle_t engine = NULL;
    cordic_engine_config_t engine_config = {
        .clock_source = CORDIC_CLK_SRC_DEFAULT,
    };
    TEST_ESP_OK(cordic_new_engine(&engine_config, &engine));

    cordic_calculate_config_t calc_config = {
        .function = ESP_CORDIC_FUNC_COS,
        .iq_format = ESP_CORDIC_FORMAT_Q31,
        .iteration_count = 4,
        .scale_exp = 0,
    };

    uint32_t data_x[32] = {};
    for (size_t i = 0; i < 32; i++) {
        // Map i from [0, 31] to float range [-1.0, 1.0]
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
    printf("\n=== Comparison: Software vs Hardware (Q31 format) ===\n");
    printf("Input(Q31)   | SW_Cosine  | SW_Sine    | HW_Cosine  | HW_Sine    | Cos_Error  | Sin_Error\n");
    printf("-------------|------------|------------|------------|------------|------------|----------\n");

    for (size_t i = 0; i < 32; i++) {
        float angle_rad = cordic_convert_fixed_to_float(data_x[i], calc_config.iq_format) * M_PI;
        float sw_cosine = cosf(angle_rad);
        float sw_sine = sinf(angle_rad);
        // Convert hardware results to float using cordic_convert_fixed_to_float
        float hw_cosine = cordic_convert_fixed_to_float(res1[i], calc_config.iq_format);
        float hw_sine = cordic_convert_fixed_to_float(res2[i], calc_config.iq_format);
        float cos_error = fabsf(sw_cosine - hw_cosine);
        float sin_error = fabsf(sw_sine - hw_sine);

        // Print comparison results
        printf("0x%08lx   | %10.6f | %10.6f | %10.6f | %10.6f | %10.6f | %10.6f",
               data_x[i], sw_cosine, sw_sine, hw_cosine, hw_sine, cos_error, sin_error);

        TEST_ASSERT_FLOAT_WITHIN(0.0001f, sw_cosine, hw_cosine);
        TEST_ASSERT_FLOAT_WITHIN(0.0001f, sw_sine, hw_sine);
        printf("\n");
    }
    TEST_ESP_OK(cordic_delete_engine(engine));
}

TEST_CASE("test phase and module q31", "[cordic]")
{
    cordic_engine_handle_t engine = NULL;
    cordic_engine_config_t engine_config = {
        .clock_source = CORDIC_CLK_SRC_DEFAULT,
    };
    TEST_ESP_OK(cordic_new_engine(&engine_config, &engine));

    cordic_calculate_config_t calc_config = {
        .function = ESP_CORDIC_FUNC_PHASE,
        .iq_format = ESP_CORDIC_FORMAT_Q31,
        .iteration_count = 4,
        .scale_exp = 0,
    };

    // Test range: data_x and data_y in (-1, 1) in Q31 format
    // In Q31 format: -0x80000000 = -1.0, 0x00000000 = 0.0, 0x7FFFFFFF ≈ 0.999999999
    // Generate 60 test points uniformly distributed in (-1, 1) x (-1, 1)
    uint32_t data_x[60] = {};
    uint32_t data_y[60] = {};
    for (size_t i = 0; i < 60; i++) {
        // Generate x and y values in range (-1, 1)
        // Map i from [0, 59] to float range [-1.0, 1.0] (avoiding exact -1 and 1)
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
    printf("\n=== Comparison: Software vs Hardware (Phase and Module Q31 format, range: -1 to 1) ===\n");
    printf("X_Float      | Y_Float     | SW_Phase   | SW_Module  | HW_Phase   | HW_Module  | Phase_Err  | Module_Err\n");
    printf("-------------|-------------|------------|------------|------------|------------|------------|-----------\n");

    for (size_t i = 0; i < 60; i++) {
        float x_float = cordic_convert_fixed_to_float(data_x[i], calc_config.iq_format);
        float y_float = cordic_convert_fixed_to_float(data_y[i], calc_config.iq_format);
        float sw_phase = atan2f(y_float, x_float) / M_PI;
        float sw_module = sqrtf(x_float * x_float + y_float * y_float);
        // Convert hardware results to float using cordic_convert_fixed_to_float
        uint32_t hw_module_q31 = res2[i];
        if (hw_module_q31 == 0x7FFFFFFF) {
            continue; // Skip this entry, do not print
        }
        float hw_phase = cordic_convert_fixed_to_float(res1[i], calc_config.iq_format);
        float hw_module = cordic_convert_fixed_to_float(res2[i], calc_config.iq_format);
        float phase_error = fabsf(sw_phase - hw_phase);
        float module_error = fabsf(sw_module - hw_module);

        // Print comparison results
        printf("%10.6f | %10.6f | %10.6f | %10.6f | %10.6f | %10.6f | %10.6f | %10.6f",
               x_float, y_float, sw_phase, sw_module, hw_phase, hw_module, phase_error, module_error);

        TEST_ASSERT_FLOAT_WITHIN(0.0001f, sw_phase, hw_phase);
        TEST_ASSERT_FLOAT_WITHIN(0.0001f, sw_module, hw_module);
        printf("\n");
    }
    TEST_ESP_OK(cordic_delete_engine(engine));
}

TEST_CASE("test arctan q31", "[cordic]")
{
    cordic_engine_handle_t engine = NULL;
    cordic_engine_config_t engine_config = {
        .clock_source = CORDIC_CLK_SRC_DEFAULT,
    };
    TEST_ESP_OK(cordic_new_engine(&engine_config, &engine));

    cordic_calculate_config_t calc_config = {
        .function = ESP_CORDIC_FUNC_ARCTAN,
        .iq_format = ESP_CORDIC_FORMAT_Q31,
        .iteration_count = 4,
        .scale_exp = 0,
    };

    // Test range: data_x in (-1, 1) in Q31 format
    // In Q31 format: -0x80000000 = -1.0, 0x00000000 = 0.0, 0x7FFFFFFF ≈ 0.999999999
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
    printf("\n=== Comparison: Software vs Hardware (Arctan Q31 format, input range: -1 to 1) ===\n");
    printf("X_Float      | SW_Arctan  | HW_Arctan  | Error      | Arctan(rad)\n");
    printf("-------------|------------|------------|------------|------------\n");

    for (size_t i = 0; i < 32; i++) {
        float x_float = cordic_convert_fixed_to_float(data_x[i], calc_config.iq_format);
        float sw_arctan_rad = atanf(x_float);
        // Normalize to [-1, 1] range (divide by π for Q31 format output)
        // Q31 arctan output is typically normalized: arctan(x) / π
        float sw_arctan_normalized = sw_arctan_rad / M_PI;
        // Convert hardware result to float using cordic_convert_fixed_to_float
        float hw_arctan_normalized = cordic_convert_fixed_to_float(res1[i], calc_config.iq_format);
        float arctan_error = fabsf(sw_arctan_normalized - hw_arctan_normalized);

        // Print comparison results
        printf("%10.6f   | %10.6f | %10.6f | %10.6f",
               x_float, sw_arctan_normalized, hw_arctan_normalized, arctan_error);

        TEST_ASSERT_FLOAT_WITHIN(0.0001f, sw_arctan_normalized, hw_arctan_normalized);
        printf("\n");
    }
    TEST_ESP_OK(cordic_delete_engine(engine));
}

TEST_CASE("test hyperbolic cosine and sine q31", "[cordic]")
{
    cordic_engine_handle_t engine = NULL;
    cordic_engine_config_t engine_config = {
        .clock_source = CORDIC_CLK_SRC_DEFAULT,
    };
    TEST_ESP_OK(cordic_new_engine(&engine_config, &engine));

    cordic_calculate_config_t calc_config = {
        .function = ESP_CORDIC_FUNC_COSH,
        .iq_format = ESP_CORDIC_FORMAT_Q31,
        .iteration_count = 4,
        .scale_exp = 1,
    };

    // Test range: data_x in (-0.559, 0.559) in Q31 format
    // In Q31 format: -0.559 * 2147483648 = -1200611353.6, 0.559 * 2147483648 = 1200611353.6
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
    printf("\n=== Comparison: Software vs Hardware (Hyperbolic Cosine and Sine Q31 format, input range: -0.559 to 0.559) ===\n");
    printf("X_Float      | SW_Cosh    | SW_Sinh    | HW_Cosh    | HW_Sinh    | Cosh_Err   | Sinh_Err\n");
    printf("-------------|------------|------------|------------|------------|------------|----------\n");

    for (size_t i = 0; i < 32; i++) {
        float x_float = cordic_convert_fixed_to_float(data_x[i], calc_config.iq_format) * 2.0f;
        float sw_cosh = cosh(x_float);
        float sw_sinh = sinh(x_float);
        // Convert hardware results to float using cordic_convert_fixed_to_float
        float hw_cosh_temp = cordic_convert_fixed_to_float(res1[i], calc_config.iq_format);
        float hw_sinh_temp = cordic_convert_fixed_to_float(res2[i], calc_config.iq_format);
        float hw_cosh = hw_cosh_temp * 2.0f;
        float hw_sinh = hw_sinh_temp * 2.0f;
        float cosh_error = fabsf(sw_cosh - hw_cosh);
        float sinh_error = fabsf(sw_sinh - hw_sinh);

        // Print comparison results
        printf("%10.6f   | %10.6f | %10.6f | %10.6f | %10.6f | %10.6f | %10.6f",
               x_float, sw_cosh, sw_sinh, hw_cosh, hw_sinh, cosh_error, sinh_error);

        TEST_ASSERT_FLOAT_WITHIN(0.0001f, sw_cosh, hw_cosh);
        TEST_ASSERT_FLOAT_WITHIN(0.0001f, sw_sinh, hw_sinh);
        printf("\n");
    }
    TEST_ESP_OK(cordic_delete_engine(engine));
}

TEST_CASE("test arctanh q31", "[cordic]")
{
    cordic_engine_handle_t engine = NULL;
    cordic_engine_config_t engine_config = {
        .clock_source = CORDIC_CLK_SRC_DEFAULT,
    };
    TEST_ESP_OK(cordic_new_engine(&engine_config, &engine));

    cordic_calculate_config_t calc_config = {
        .function = ESP_CORDIC_FUNC_ARCHTANH,
        .iq_format = ESP_CORDIC_FORMAT_Q31,
        .iteration_count = 4,
        .scale_exp = 1,
    };

    // Test range: data_x in (-0.403, 0.403) in Q31 format
    // In Q31 format: -0.403 * 2147483648 ≈ -865536000, 0.403 * 2147483648 ≈ 865536000
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
    printf("\n=== Comparison: Software vs Hardware (Arctanh Q31 format, input range: -0.403 to 0.403) ===\n");
    printf("X_Float      | SW_Arctanh | HW_Arctanh | Error      | Arctanh(rad)\n");
    printf("-------------|------------|------------|------------|------------\n");

    for (size_t i = 0; i < 32; i++) {
        float x_float = cordic_convert_fixed_to_float(data_x[i], calc_config.iq_format) * 2.0f;
        float sw_arctanh_rad = atanh(x_float);
        // Normalize to [-1, 1] range (divide by π for Q31 format output)
        // Q31 arctanh output is typically normalized: arctanh(x) / π
        float sw_arctanh_normalized = sw_arctanh_rad;
        // Convert hardware result to float using cordic_convert_fixed_to_float
        float hw_arctanh_temp = cordic_convert_fixed_to_float(res1[i], calc_config.iq_format);
        float hw_arctanh_normalized = hw_arctanh_temp * 2.0f;
        float arctanh_error = fabsf(sw_arctanh_normalized - hw_arctanh_normalized);

        // Print comparison results
        printf("%10.6f   | %10.6f | %10.6f | %10.6f | %10.6f",
               x_float, sw_arctanh_normalized, hw_arctanh_normalized, arctanh_error, sw_arctanh_rad);

        TEST_ASSERT_FLOAT_WITHIN(0.0001f, sw_arctanh_normalized, hw_arctanh_normalized);
        printf("\n");
    }
    TEST_ESP_OK(cordic_delete_engine(engine));
}

TEST_CASE("test natural logarithm q31", "[cordic]")
{
    cordic_engine_handle_t engine = NULL;
    cordic_engine_config_t engine_config = {
        .clock_source = CORDIC_CLK_SRC_DEFAULT,
    };
    TEST_ESP_OK(cordic_new_engine(&engine_config, &engine));

    cordic_calculate_config_t calc_config = {
        .function = ESP_CORDIC_FUNC_LOGE,
        .iq_format = ESP_CORDIC_FORMAT_Q31,
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

        // Convert to Q31 format
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
    printf("\n=== Comparison: Software vs Hardware (Natural Logarithm Q31 format, x range: 0.1069 to 9.3573) ===\n");
    printf("X_Value     | HW_Param   | SW_LnX     | HW_LnX     | Error      | LnX\n");
    printf("------------|------------|------------|------------|------------|------------\n");

    for (size_t i = 0; i < 32; i++) {
        float x = x_values[i];

        // Calculate hardware parameter: (x-1)/(x+1) * 0.5
        float hw_param = ((x - 1.0f) / (x + 1.0f)) * 0.5f;

        // Software calculation: ln(x)
        float sw_lnx = logf(x);
        // Convert hardware result to float using cordic_convert_fixed_to_float
        float hw_lnx_scaled = cordic_convert_fixed_to_float(res1[i], calc_config.iq_format);
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

TEST_CASE("test square root q31", "[cordic]")
{
    cordic_engine_handle_t engine = NULL;
    cordic_engine_config_t engine_config = {
        .clock_source = CORDIC_CLK_SRC_DEFAULT,
    };
    TEST_ESP_OK(cordic_new_engine(&engine_config, &engine));

    cordic_calculate_config_t calc_config = {
        .function = ESP_CORDIC_FUNC_SQUARE_ROOT,
        .iq_format = ESP_CORDIC_FORMAT_Q31,
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

        // Convert x to Q31 format
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
    printf("\n=== Comparison: Software vs Hardware (Square Root Q31 format, x range: 0.1069 to 1) ===\n");
    printf("X_Value     | SW_SqrtX   | HW_SqrtX   | Error\n");
    printf("------------|------------|------------|----------\n");

    for (size_t i = 0; i < 32; i++) {
        float x = x_values[i];
        float sw_sqrtx = sqrtf(x);
        // Convert hardware result to float using cordic_convert_fixed_to_float
        float hw_sqrtx = cordic_convert_fixed_to_float(res1[i], calc_config.iq_format);
        float sqrtx_error = fabsf(sw_sqrtx - hw_sqrtx);

        // Print comparison results
        printf("%10.6f   | %10.6f | %10.6f | %10.6f",
               x, sw_sqrtx, hw_sqrtx, sqrtx_error);
        TEST_ASSERT_FLOAT_WITHIN(0.00001f, sw_sqrtx, hw_sqrtx);
        printf("\n");
    }
    TEST_ESP_OK(cordic_delete_engine(engine));
}

TEST_CASE("test arctan scale 2 q31", "[cordic]")
{
    cordic_engine_handle_t engine = NULL;
    cordic_engine_config_t engine_config = {
        .clock_source = CORDIC_CLK_SRC_DEFAULT,
    };
    TEST_ESP_OK(cordic_new_engine(&engine_config, &engine));

    cordic_calculate_config_t calc_config = {
        .function = ESP_CORDIC_FUNC_ARCTAN,
        .iq_format = ESP_CORDIC_FORMAT_Q31,
        .iteration_count = 4,
        .scale_exp = 2,  // Scale = 2: hardware input is (x) * 2^(-2) = x / 4
    };

    // Test range: hardware input (x * 2^(-2)) should be in [-1, 1]
    // So actual x range should be [-4, 4] in Q31 format
    // Note: Q31 format range is [-1, 1], but we need to input values that represent [-4, 4]
    // Hardware will scale: x_hw = x * 2^(-2) = x / 4, so x should be in [-4, 4] range
    // Generate 32 test points uniformly distributed from -4 to 4
    uint32_t data_x[32] = {};
    for (size_t i = 0; i < 32; i++) {
        // Map i from [0, 31] to float range [-4, 4]
        float x_float = -4.0f + (i * 8.0f / 31.0f);
        // Convert to Q31 format: x_float is in [-4, 4], but Q31 range is [-1, 1]
        // So we need to represent x_float / 4 in Q31 format
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
    printf("\n=== Comparison: Software vs Hardware (Arctan Q31 format, scale=2, input range: -4 to 4) ===\n");
    printf("Input(Q31)   | X_Float    | SW_Arctan  | HW_Arctan  | Error      | Arctan(rad)\n");
    printf("-------------|------------|------------|------------|------------|------------\n");

    for (size_t i = 0; i < 32; i++) {
        // Convert Q31 input to float using cordic_convert_fixed_to_float
        float x_normalized = cordic_convert_fixed_to_float(data_x[i], calc_config.iq_format);  // This is in [-1, 1] range
        // Reconstruct original x value: x_float = x_normalized * 4
        float x_float = x_normalized * 4.0f;  // Original x in [-4, 4] range
        // Hardware scales input: x_scaled = x_float * 2^(-2) = x_float / 4 = x_normalized
        // float x_scaled = x_float / 4.0f;  // This equals x_normalized

        // Software calculation: arctan(x_scaled) / π
        float sw_arctan_rad = atanf(x_float);
        float sw_arctan_normalized = sw_arctan_rad / M_PI;

        // Hardware output is already arctan(x * 2^(-2)) / π = arctan(x_scaled) / π
        // Convert Q31 result to float using cordic_convert_fixed_to_float
        float hw_arctan_temp = cordic_convert_fixed_to_float(res1[i], calc_config.iq_format);
        float hw_arctan_normalized = hw_arctan_temp * 4;

        float arctan_error = fabsf(sw_arctan_normalized - hw_arctan_normalized);

        // Print comparison results
        printf("0x%08lx   | %10.6f | %10.6f | %10.6f | %10.6f | %10.6f",
               data_x[i], x_float, sw_arctan_normalized, hw_arctan_normalized, arctan_error, sw_arctan_rad);

        // Highlight significant errors
        TEST_ASSERT_FLOAT_WITHIN(0.001f, sw_arctan_normalized, hw_arctan_normalized);
        printf("\n");
    }
    TEST_ESP_OK(cordic_delete_engine(engine));
}

TEST_CASE("test reconfig from cos q15 to log q31", "[cordic]")
{
    cordic_engine_handle_t engine = NULL;

    // Step 1: Create engine with COS function and Q15 format
    cordic_engine_config_t engine_config = {
        .clock_source = CORDIC_CLK_SRC_DEFAULT,
    };
    TEST_ESP_OK(cordic_new_engine(&engine_config, &engine));

    cordic_calculate_config_t calc_config_cos = {
        .function = ESP_CORDIC_FUNC_COS,
        .iq_format = ESP_CORDIC_FORMAT_Q15,
        .iteration_count = 4,
        .scale_exp = 0,
    };

    // Test COS function with Q15 format
    uint32_t data_x_cos[16] = {};
    for (size_t i = 0; i < 16; i++) {
        // Map i from [0, 15] to float range [-1.0, 1.0]
        float float_value = -1.0f + (i * 2.0f / 15.0f);
        data_x_cos[i] = cordic_convert_float_to_fixed(float_value, calc_config_cos.iq_format);
    }
    uint32_t res1_cos[16] = {};
    uint32_t res2_cos[16] = {};

    cordic_input_buffer_desc_t input_buffer_cos = {
        .p_data_arg1 = data_x_cos,
        .p_data_arg2 = NULL,
    };
    cordic_output_buffer_desc_t output_buffer_cos = {
        .p_data_res1 = res1_cos,
        .p_data_res2 = res2_cos,
    };

    TEST_ESP_OK(cordic_calculate_polling(engine, &calc_config_cos, &input_buffer_cos, &output_buffer_cos, 16));

    // Print header for COS test
    printf("\n=== Step 1: COS function with Q15 format ===\n");
    printf("Input(Q15) | Angle(rad) | SW_Cosine  | HW_Cosine  | Cos_Error\n");
    printf("-----------|------------|------------|------------|----------\n");

    for (size_t i = 0; i < 16; i++) {
        // Convert Q15 input angle to radians
        float angle_rad = cordic_convert_fixed_to_float(data_x_cos[i] & 0xFFFF, calc_config_cos.iq_format) * M_PI;
        float sw_cosine = cosf(angle_rad);
        float hw_cosine = cordic_convert_fixed_to_float(res1_cos[i] & 0xFFFF, calc_config_cos.iq_format);
        float cos_error = fabsf(sw_cosine - hw_cosine);

        printf("0x%04lx     | %10.6f | %10.6f | %10.6f | %10.6f",
               data_x_cos[i] & 0xFFFF, angle_rad, sw_cosine, hw_cosine, cos_error);
        TEST_ASSERT_FLOAT_WITHIN(0.01f, sw_cosine, hw_cosine);
        printf("\n");
    }

    // Step 2: Use NATURAL_LOG function with Q31 format (no reconfig needed, just use new calc_config)
    cordic_calculate_config_t calc_config_log = {
        .function = ESP_CORDIC_FUNC_LOGE,
        .iq_format = ESP_CORDIC_FORMAT_Q31,
        .iteration_count = 4,
        .scale_exp = 1,
    };

    // Test NATURAL_LOG function with Q31 format
    // Test range: x in (0.1069, 9.3573)
    float x_values[16] = {};
    uint32_t data_x_log[16] = {};
    for (size_t i = 0; i < 16; i++) {
        // Map i from [0, 15] to x range [0.1069, 9.3573]
        float x = 0.1069f + (i * (9.3573f - 0.1069f) / 15.0f);
        x_values[i] = x;

        // Calculate hardware parameter: (x-1)/(x+1) * 0.5
        float hw_param = ((x - 1.0f) / (x + 1.0f)) * 0.5f;

        // Convert to Q31 format
        data_x_log[i] = cordic_convert_float_to_fixed(hw_param, calc_config_log.iq_format);
    }
    uint32_t res1_log[16] = {};
    uint32_t res2_log[16] = {};

    cordic_input_buffer_desc_t input_buffer_log = {
        .p_data_arg1 = data_x_log,
        .p_data_arg2 = NULL,
    };
    cordic_output_buffer_desc_t output_buffer_log = {
        .p_data_res1 = res1_log,
        .p_data_res2 = res2_log,
    };

    TEST_ESP_OK(cordic_calculate_polling(engine, &calc_config_log, &input_buffer_log, &output_buffer_log, 16));

    // Print header for LOG test
    printf("\n=== Step 2: NATURAL_LOG function with Q31 format (after reconfig) ===\n");
    printf("X_Value     | HW_Param   | SW_LnX     | HW_LnX     | Error\n");
    printf("------------|------------|------------|------------|----------\n");

    for (size_t i = 0; i < 16; i++) {
        float x = x_values[i];

        // Calculate hardware parameter: (x-1)/(x+1) * 0.5
        float hw_param = ((x - 1.0f) / (x + 1.0f)) * 0.5f;

        // Software calculation: ln(x)
        float sw_lnx = logf(x);
        // Convert hardware result to float using cordic_convert_fixed_to_float
        float hw_lnx_scaled = cordic_convert_fixed_to_float(res1_log[i], calc_config_log.iq_format);
        float hw_lnx = hw_lnx_scaled * 4.0f;
        float lnx_error = fabsf(sw_lnx - hw_lnx);

        printf("%10.6f   | %10.6f | %10.6f | %10.6f | %10.6f",
               x, hw_param, sw_lnx, hw_lnx, lnx_error);
        TEST_ASSERT_FLOAT_WITHIN(0.001f, sw_lnx, hw_lnx);
        printf("\n");
    }

    cordic_delete_engine(engine);
}
