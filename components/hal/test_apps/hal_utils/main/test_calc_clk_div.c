/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "unity.h"
#include "hal/hal_utils.h"


TEST_CASE("test_integral_division", "[clk_div]")
{
    uint32_t int_div = 0;
    hal_utils_clk_info_t clk_info = {
        .src_freq_hz = 80 * 1000 * 1000,
        .exp_freq_hz = 57 * 1000 * 1000,
        .max_integ = 256,
        .min_integ = 1,
        .round_opt = 0,  // round down
    };
    // Round down test
    uint32_t real_freq = 0;
    real_freq = hal_utils_calc_clk_div_integer(&clk_info, &int_div);
    TEST_ASSERT_EQUAL_UINT32(1, int_div);
    TEST_ASSERT_EQUAL_UINT32(clk_info.src_freq_hz, real_freq);

    // Normal round test
    clk_info.round_opt = 2;
    real_freq = hal_utils_calc_clk_div_integer(&clk_info, &int_div);
    TEST_ASSERT_EQUAL_UINT32(2, int_div);
    TEST_ASSERT_EQUAL_UINT32(clk_info.src_freq_hz / 2, real_freq);

    clk_info.exp_freq_hz = 60 * 1000 * 1000;
    real_freq = hal_utils_calc_clk_div_integer(&clk_info, &int_div);
    TEST_ASSERT_EQUAL_UINT32(2, int_div);
    TEST_ASSERT_EQUAL_UINT32(clk_info.src_freq_hz / 2, real_freq);

    clk_info.exp_freq_hz = 63 * 1000 * 1000;
    real_freq = hal_utils_calc_clk_div_integer(&clk_info, &int_div);
    TEST_ASSERT_EQUAL_UINT32(1, int_div);
    TEST_ASSERT_EQUAL_UINT32(clk_info.src_freq_hz, real_freq);

    // Round up test
    clk_info.round_opt = 1;
    real_freq = hal_utils_calc_clk_div_integer(&clk_info, &int_div);
    TEST_ASSERT_EQUAL_UINT32(2, int_div);
    TEST_ASSERT_EQUAL_UINT32(clk_info.src_freq_hz / 2, real_freq);

    // Integral division
    clk_info.exp_freq_hz = 40 * 1000 * 1000;
    real_freq = hal_utils_calc_clk_div_integer(&clk_info, &int_div);
    TEST_ASSERT_EQUAL_UINT32(2, int_div);
    TEST_ASSERT_EQUAL_UINT32(clk_info.src_freq_hz / 2, real_freq);
    clk_info.round_opt = 0;
    real_freq = hal_utils_calc_clk_div_integer(&clk_info, &int_div);
    TEST_ASSERT_EQUAL_UINT32(2, int_div);
    TEST_ASSERT_EQUAL_UINT32(clk_info.src_freq_hz / 2, real_freq);
    clk_info.round_opt = 2;
    real_freq = hal_utils_calc_clk_div_integer(&clk_info, &int_div);
    TEST_ASSERT_EQUAL_UINT32(2, int_div);
    TEST_ASSERT_EQUAL_UINT32(clk_info.src_freq_hz / 2, real_freq);
}

TEST_CASE("test_fractal_division", "[clk_div]")
{
    hal_utils_clk_div_t clk_div = {};
    hal_utils_clk_info_t clk_info = {
        .src_freq_hz = 160 * 1000 * 1000,
        .exp_freq_hz = 16 * 1024 * 1024,
        .max_integ = 256,
        .min_integ = 1,
        .max_fract = 512,
    };
    uint32_t real_freq = 0;
    // Fractal division with error
    real_freq = hal_utils_calc_clk_div_frac_fast(&clk_info, &clk_div);
    TEST_ASSERT_EQUAL_UINT32(9, clk_div.integer);
    TEST_ASSERT_UINT32_WITHIN(clk_info.exp_freq_hz * 0.001, clk_info.exp_freq_hz, real_freq);
    real_freq = hal_utils_calc_clk_div_frac_accurate(&clk_info, &clk_div);
    TEST_ASSERT_EQUAL_UINT32(9, clk_div.integer);
    TEST_ASSERT_UINT32_WITHIN(clk_info.exp_freq_hz * 0.0001, clk_info.exp_freq_hz, real_freq);

    // Fractal division with no error
    clk_info.exp_freq_hz = 50 * 1000 * 1000;
    real_freq = hal_utils_calc_clk_div_frac_fast(&clk_info, &clk_div);
    TEST_ASSERT_EQUAL_UINT32(3, clk_div.integer);
    TEST_ASSERT_EQUAL_UINT32(clk_info.exp_freq_hz, real_freq);
    real_freq = hal_utils_calc_clk_div_frac_accurate(&clk_info, &clk_div);
    TEST_ASSERT_EQUAL_UINT32(3, clk_div.integer);
    TEST_ASSERT_EQUAL_UINT32(clk_info.exp_freq_hz, real_freq);

    // Integral division
    clk_info.exp_freq_hz = 40 * 1000 * 1000;
    real_freq = hal_utils_calc_clk_div_frac_fast(&clk_info, &clk_div);
    TEST_ASSERT_EQUAL_UINT32(4, clk_div.integer);
    TEST_ASSERT_EQUAL_UINT32(0, clk_div.numerator);
    TEST_ASSERT_EQUAL_UINT32(clk_info.exp_freq_hz, real_freq);
    real_freq = hal_utils_calc_clk_div_frac_accurate(&clk_info, &clk_div);
    TEST_ASSERT_EQUAL_UINT32(4, clk_div.integer);
    TEST_ASSERT_EQUAL_UINT32(0, clk_div.numerator);
    TEST_ASSERT_EQUAL_UINT32(clk_info.exp_freq_hz, real_freq);
}

TEST_CASE("test_fault_division", "[clk_div]")
{
    hal_utils_clk_div_t clk_div = {};
    hal_utils_clk_info_t clk_info = {
        .src_freq_hz = 160 * 1000 * 1000,
        .exp_freq_hz = 1250 * 1000,
        .max_integ = 128,
        .min_integ = 2,
        .max_fract = 512,
    };
    uint32_t real_freq = 0;
    // Equal to the max integer
    real_freq = hal_utils_calc_clk_div_frac_fast(&clk_info, &clk_div);
    TEST_ASSERT_FALSE(real_freq);
    real_freq = hal_utils_calc_clk_div_frac_accurate(&clk_info, &clk_div);
    TEST_ASSERT_FALSE(real_freq);

    // Exceed the max integer
    clk_info.exp_freq_hz = 1000 * 1000;
    real_freq = hal_utils_calc_clk_div_frac_fast(&clk_info, &clk_div);
    TEST_ASSERT_FALSE(real_freq);
    real_freq = hal_utils_calc_clk_div_frac_accurate(&clk_info, &clk_div);
    TEST_ASSERT_FALSE(real_freq);

    // Blow the min integer
    clk_info.exp_freq_hz = 125 * 1000 * 1000;
    real_freq = hal_utils_calc_clk_div_frac_fast(&clk_info, &clk_div);
    TEST_ASSERT_FALSE(real_freq);
    real_freq = hal_utils_calc_clk_div_frac_accurate(&clk_info, &clk_div);
    TEST_ASSERT_FALSE(real_freq);

    // Equal to the min integer
    clk_info.exp_freq_hz = 80 * 1000 * 1000;
    real_freq = hal_utils_calc_clk_div_frac_fast(&clk_info, &clk_div);
    TEST_ASSERT_EQUAL_UINT32(clk_info.exp_freq_hz, real_freq);
    real_freq = hal_utils_calc_clk_div_frac_accurate(&clk_info, &clk_div);
    TEST_ASSERT_EQUAL_UINT32(clk_info.exp_freq_hz, real_freq);

    // divide 0 case
    clk_info.exp_freq_hz = 200 * 1000 * 1000;
    clk_info.min_integ = 0;
    real_freq = hal_utils_calc_clk_div_frac_fast(&clk_info, &clk_div);
    TEST_ASSERT_FALSE(real_freq);
    real_freq = hal_utils_calc_clk_div_frac_accurate(&clk_info, &clk_div);
    TEST_ASSERT_FALSE(real_freq);
}
