/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "soc/soc.h"
#include "ulp_lp_core_print.h"
#include <ctype.h>

void assert_function_in_rom(void *func)
{
    if ((intptr_t)func < SOC_LP_ROM_LOW || (intptr_t)func > SOC_LP_ROM_HIGH) {
        abort();
    }
}

static void test_memset(void)
{
#define TEST_MEMSET_VAL 0xAB
    assert_function_in_rom(memset);

    lp_core_printf("Testing memset\n");
    uint8_t test_buf[100];

    memset(test_buf, TEST_MEMSET_VAL, sizeof(test_buf));

    for (int i = 0; i < sizeof(test_buf); i++) {
        if (test_buf[i] != TEST_MEMSET_VAL) {
            lp_core_printf("test_buf[%d]: 0x%X != 0x%X\n", i, test_buf[i], TEST_MEMSET_VAL);
            abort();
        }
    }
}

static void test_memcpy(void)
{
#define TEST_MEMCPY_VAL 0xAC
#define TEST_SIZE 100

    assert_function_in_rom(memcpy);
    lp_core_printf("Testing memcpy\n");
    uint8_t test_buf_a[TEST_SIZE];
    memset(test_buf_a, TEST_MEMCPY_VAL, TEST_SIZE);

    uint8_t test_buf_b[TEST_SIZE];

    memcpy(test_buf_b, test_buf_a, TEST_SIZE);

    for (int i = 0; i < TEST_SIZE; i++) {
        if (test_buf_b[i] != TEST_MEMCPY_VAL) {
            lp_core_printf("test_buf_b[%d]: 0x%X != 0x%X\n", i, test_buf_b[i], TEST_MEMCPY_VAL);
            abort();
        }
    }
}

static void test_abs(void)
{
    assert_function_in_rom(abs);
    lp_core_printf("Testing abs\n");
    if (abs(-123) != 123) {
        lp_core_printf("Failed abs() test\n");
        abort();
    }
}

volatile bool lp_rom_test_finished;

int main(void)
{
    // Test a misc of ROM functions to catch any regression with LD file updates
    test_memset();
    test_memcpy();
    test_abs();

    lp_core_printf("ULP: all tests passed\n");
    lp_rom_test_finished = true;

    return 0;
}
