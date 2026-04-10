/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "unity.h"
#include "unity_test_runner.h"
#include "soc/soc_caps.h"
#include "esp_cpu.h"
#include "esp_system.h"

#if __XTENSA__
#define CPU_DEBUG_EXCEPTION_REASON "Unhandled debug exception"
#else
#define CPU_DEBUG_EXCEPTION_REASON "Breakpoint"
#endif

#if SOC_CPU_BREAKPOINTS_NUM > 0
static void __attribute__((noinline)) breakpoint_target_function(void)
{
    __asm__ __volatile__("" ::: "memory");
}

static void trigger_breakpoint_panic(void)
{
    TEST_ASSERT_EQUAL(ESP_OK, esp_cpu_set_breakpoint(0, (const void *)breakpoint_target_function));
    breakpoint_target_function();
    while (true) {
    }
}

static void check_breakpoint_reset_reason(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_PANIC, esp_reset_reason());
}

TEST_CASE_MULTIPLE_STAGES("CPU breakpoint: trigger causes panic",
                          "[cpu][cpu_breakpoint][cpu_non_recoverable][reset=" CPU_DEBUG_EXCEPTION_REASON ",SW_CPU_RESET]",
                          trigger_breakpoint_panic, check_breakpoint_reset_reason)
#endif

#if SOC_CPU_WATCHPOINTS_NUM > 0
static volatile uint32_t s_watchpoint_target;
static volatile uint32_t s_watchpoint_sink;
#define WATCHPOINT_TARGET_ADDR ((const void *)&s_watchpoint_target)

static void __attribute__((noinline)) trigger_watchpoint_store(void)
{
    s_watchpoint_target++;
}

static void __attribute__((noinline)) trigger_watchpoint_load(void)
{
    s_watchpoint_sink = s_watchpoint_target;
}

static void trigger_watchpoint_store_panic(void)
{
    s_watchpoint_target = 0;
    TEST_ASSERT_EQUAL(ESP_OK,
                      esp_cpu_set_watchpoint(0, WATCHPOINT_TARGET_ADDR, sizeof(s_watchpoint_target), ESP_CPU_WATCHPOINT_STORE));
    trigger_watchpoint_store();
    while (true) {
    }
}

static void check_watchpoint_reset_reason(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_PANIC, esp_reset_reason());
}

TEST_CASE_MULTIPLE_STAGES("CPU watchpoint: store trigger causes panic",
                          "[cpu][cpu_watchpoint][cpu_non_recoverable][reset=" CPU_DEBUG_EXCEPTION_REASON ",SW_CPU_RESET]",
                          trigger_watchpoint_store_panic, check_watchpoint_reset_reason)

static void trigger_watchpoint_load_panic(void)
{
    s_watchpoint_target = 0x12345678;
    s_watchpoint_sink = 0;
    TEST_ASSERT_EQUAL(ESP_OK,
                      esp_cpu_set_watchpoint(0, WATCHPOINT_TARGET_ADDR, sizeof(s_watchpoint_target), ESP_CPU_WATCHPOINT_LOAD));
    trigger_watchpoint_load();
    while (true) {
    }
}

TEST_CASE_MULTIPLE_STAGES("CPU watchpoint: load trigger causes panic",
                          "[cpu][cpu_watchpoint][cpu_non_recoverable][reset=" CPU_DEBUG_EXCEPTION_REASON ",SW_CPU_RESET]",
                          trigger_watchpoint_load_panic, check_watchpoint_reset_reason)
#endif
