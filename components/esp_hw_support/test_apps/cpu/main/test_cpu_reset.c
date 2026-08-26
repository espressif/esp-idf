/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_runner.h"
#include "esp_cpu.h"
#include "esp_system.h"

#if SOC_CPU_CORES_NUM == 1
static void do_cpu_reset(void)
{
    esp_cpu_reset(esp_cpu_get_core_id());
    while (true) {
    }
}

static void check_cpu_reset_reason(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_SW, esp_reset_reason());
}

TEST_CASE_MULTIPLE_STAGES("CPU reset: current core reset reboots the chip", "[cpu][cpu_reset][reset=SW_CPU_RESET]",
                          do_cpu_reset, check_cpu_reset_reason)
#endif
