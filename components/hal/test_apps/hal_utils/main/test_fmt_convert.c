/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <math.h>
#include "unity.h"
#include "hal/hal_utils.h"

#ifndef BIT
#define BIT(n)          (1UL << (n))
#endif

TEST_CASE("test_float_to_fixed_point", "[fmt_convert]")
{
    float f_nan = NAN;
    float f_inf = INFINITY;
    float f0 = 100.9f;
    float f_zero = 0;
    float f_precise = 0.5f;
    float f_int = 2.0f;
    float f_frac = 1.0f / 3.0f;
    float f_dec = 1.453f;
    float f_neg = -1.25f;
    uint32_t out = 0;
    hal_utils_fixed_point_t fp_cfg = {
        .int_bit = 24,
        .frac_bit = 8,
        .saturation = false,
    };

    // Invalid arguments case
    TEST_ASSERT_EQUAL_INT(-3, hal_utils_float_to_fixed_point_32b(f_dec, &fp_cfg, &out));
    printf("Invalid arguments case passed!\n");
    fp_cfg.int_bit = 2;

    // Overflow case
    TEST_ASSERT_EQUAL_INT(-1, hal_utils_float_to_fixed_point_32b(f0, &fp_cfg, &out));
    TEST_ASSERT_EQUAL_UINT32(0x64E6, out);   // integ: 0x64 = 100, frac: 0xE6 / 0x100 = 0.8984375
    printf("Overflow case passed!\n");

    // Not-a-Number case
    TEST_ASSERT_EQUAL_INT(-2, hal_utils_float_to_fixed_point_32b(f_nan, &fp_cfg, &out));
    printf("Not-a-Number case passed!\n");

    // Infinity case
    TEST_ASSERT_EQUAL_INT(-1, hal_utils_float_to_fixed_point_32b(f_inf, &fp_cfg, &out));
    TEST_ASSERT_EQUAL_UINT32(BIT(31) - 1, out);
    printf("Infinity case passed!\n");

    fp_cfg.saturation = true;
    // Limit overflow case
    TEST_ASSERT_EQUAL_INT(-1, hal_utils_float_to_fixed_point_32b(f0, &fp_cfg, &out));
    TEST_ASSERT_EQUAL_UINT32(BIT(10) - 1, out);  // Limit to the maximum value, integ: 0x03 = 3 | frac: 0xff / 0x100 = 0.99609375
    printf("Limit overflow case passed!\n");

    // Zero case
    TEST_ASSERT_EQUAL_INT(0, hal_utils_float_to_fixed_point_32b(f_zero, &fp_cfg, &out));
    TEST_ASSERT_EQUAL_UINT32(0, out);  // Special case, 0 = 0
    printf("Zero case passed!\n");

    // Precision case
    TEST_ASSERT_EQUAL_INT(0, hal_utils_float_to_fixed_point_32b(f_precise, &fp_cfg, &out));
    TEST_ASSERT_EQUAL_UINT32(BIT(7), out);  // frac: 0x80 / 0x100 = 0.5
    printf("Precision case passed!\n");

    // Integer case
    TEST_ASSERT_EQUAL_INT(0, hal_utils_float_to_fixed_point_32b(f_int, &fp_cfg, &out));
    TEST_ASSERT_EQUAL_UINT32(BIT(9), out);  // integ: 2 | frac: 0x00 / 0x100 = 0
    printf("Integer case passed!\n");

    // Fraction case
    TEST_ASSERT_EQUAL_INT(0, hal_utils_float_to_fixed_point_32b(f_frac, &fp_cfg, &out));
    TEST_ASSERT_EQUAL_UINT32(0x055, out);  // 0x55 / 0x100 = 0.33203125
    printf("Fraction case passed!\n");

    // Decimal case
    TEST_ASSERT_EQUAL_INT(0, hal_utils_float_to_fixed_point_32b(f_dec, &fp_cfg, &out));
    TEST_ASSERT_EQUAL_UINT32(0x173, out);   // integ: 0x01 = 1, frac: 0x73 / 0x100 = 0.44921875
    printf("Decimal case passed!\n");

    // Negative case
    TEST_ASSERT_EQUAL_INT(0, hal_utils_float_to_fixed_point_32b(f_neg, &fp_cfg, &out));
    TEST_ASSERT_EQUAL_UINT32(BIT(10) | BIT(8) | (BIT(6)), out); // sign: 1 | integ: 1 | frac: 0x40 / 0x100 = 0.25
    printf("Negative case passed!\n");

    fp_cfg.int_bit = 8;
    // Integer bits case
    TEST_ASSERT_EQUAL_INT(0, hal_utils_float_to_fixed_point_32b(f0, &fp_cfg, &out));
    TEST_ASSERT_EQUAL_UINT32(0x64E6, out);   // integ: 0x64 = 100, frac: 0xE6 / 0x100 = 0.8984375
    printf("Integer bits case passed!\n");
}
