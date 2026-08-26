/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "soc/soc_caps.h"
#include "esp_cpu.h"

#if SOC_CPU_BREAKPOINTS_NUM > 0
static void __attribute__((noinline)) breakpoint_target_function(void)
{
    __asm__ __volatile__("" ::: "memory");
}

TEST_CASE("CPU breakpoint: all breakpoints can be set and cleared", "[cpu][cpu_breakpoint]")
{
    for (int bp_num = 0; bp_num < SOC_CPU_BREAKPOINTS_NUM; ++bp_num) {
        TEST_ASSERT_EQUAL(ESP_OK, esp_cpu_set_breakpoint(bp_num, (const void *)breakpoint_target_function));
    }

    for (int bp_num = 0; bp_num < SOC_CPU_BREAKPOINTS_NUM; ++bp_num) {
        TEST_ASSERT_EQUAL(ESP_OK, esp_cpu_clear_breakpoint(bp_num));
    }
}

TEST_CASE("CPU breakpoint: invalid index is rejected", "[cpu][cpu_breakpoint]")
{
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_cpu_set_breakpoint(-1, (const void *)breakpoint_target_function));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_cpu_clear_breakpoint(-1));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_cpu_set_breakpoint(SOC_CPU_BREAKPOINTS_NUM, (const void *)breakpoint_target_function));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_cpu_clear_breakpoint(SOC_CPU_BREAKPOINTS_NUM));
}

#endif
