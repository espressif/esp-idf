/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "unity.h"
#include "soc/soc_caps.h"
#include "esp_cpu.h"

#if SOC_CPU_WATCHPOINTS_NUM > 0
static volatile uint32_t s_watchpoint_target;
#define WATCHPOINT_TARGET_ADDR ((const void *)&s_watchpoint_target)

TEST_CASE("CPU watchpoint: invalid index is rejected", "[cpu][cpu_watchpoint]")
{
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG,
                      esp_cpu_set_watchpoint(-1, WATCHPOINT_TARGET_ADDR, sizeof(s_watchpoint_target), ESP_CPU_WATCHPOINT_ACCESS));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG,
                      esp_cpu_set_watchpoint(SOC_CPU_WATCHPOINTS_NUM, WATCHPOINT_TARGET_ADDR, sizeof(s_watchpoint_target), ESP_CPU_WATCHPOINT_ACCESS));
}

TEST_CASE("CPU watchpoint: invalid non-power-of-two size is rejected", "[cpu][cpu_watchpoint]")
{
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG,
                      esp_cpu_set_watchpoint(0, WATCHPOINT_TARGET_ADDR, 0, ESP_CPU_WATCHPOINT_ACCESS));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG,
                      esp_cpu_set_watchpoint(0, WATCHPOINT_TARGET_ADDR, 3, ESP_CPU_WATCHPOINT_ACCESS));
}

TEST_CASE("CPU watchpoint: misaligned address is rejected", "[cpu][cpu_watchpoint]")
{
    const void *misaligned_addr = (const void *)((uintptr_t)&s_watchpoint_target + 1);

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG,
                      esp_cpu_set_watchpoint(0, misaligned_addr, sizeof(s_watchpoint_target), ESP_CPU_WATCHPOINT_ACCESS));
}

TEST_CASE("CPU watchpoint: all watchpoints can be set and cleared", "[cpu][cpu_watchpoint]")
{
    for (int wp_num = 0; wp_num < SOC_CPU_WATCHPOINTS_NUM; ++wp_num) {
        TEST_ASSERT_EQUAL(ESP_OK,
                          esp_cpu_set_watchpoint(wp_num, WATCHPOINT_TARGET_ADDR, sizeof(s_watchpoint_target), ESP_CPU_WATCHPOINT_ACCESS));
    }

    for (int wp_num = 0; wp_num < SOC_CPU_WATCHPOINTS_NUM; ++wp_num) {
        TEST_ASSERT_EQUAL(ESP_OK, esp_cpu_clear_watchpoint(wp_num));
    }
}

TEST_CASE("CPU watchpoint: invalid clear index is rejected", "[cpu][cpu_watchpoint]")
{
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_cpu_clear_watchpoint(-1));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_cpu_clear_watchpoint(SOC_CPU_WATCHPOINTS_NUM));
}

#endif
