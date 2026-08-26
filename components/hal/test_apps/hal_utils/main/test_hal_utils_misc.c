/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "unity.h"
#include "hal/hal_utils.h"

TEST_CASE("test_hal_utils_bitwise_reverse", "[bit_ops]")
{
    uint8_t input_data[] = {0x00, 0x0F, 0xAA, 0x55, 0xC3};
    uint8_t expected_data[] = {0x00, 0xF0, 0x55, 0xAA, 0xC3};
    int num_test_cases = sizeof(input_data) / sizeof(input_data[0]);

    for (int i = 0; i < num_test_cases; i++) {
        uint8_t result = hal_utils_bitwise_reverse8(input_data[i]);
        TEST_ASSERT_EQUAL_HEX8(expected_data[i], result);
    }
}
