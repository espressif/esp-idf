/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_runner.h"
#include "esp_heap_caps.h"
#include "soc/soc_caps.h"
#include "esp_riscv_trace.h"

#define TEST_MEMORY_LEAK_THRESHOLD (-400)

static size_t before_free_8bit;
static size_t before_free_32bit;

/* Clear any filter left set by a previous (possibly failed) test case, so each case starts with
 * the encoder tracing everything. Only the configured cores have an initialized handle.
 * No-op (ESP_ERR_NOT_SUPPORTED) on targets without a filter unit. */
static void clear_all_filters(void)
{
    esp_riscv_trace_filter_config_t filter_off = { .enable = false };
    for (int core = 0; core < SOC_CPU_CORES_NUM; core++) {
        if (CONFIG_ESP_RISCV_TRACE_CORE_MASK & (1 << core)) {
            esp_riscv_trace_set_filter((esp_riscv_trace_core_t)core, &filter_off);
        }
    }
}

static void check_leak(size_t before_free, size_t after_free, const char *type)
{
    ssize_t delta = after_free - before_free;
    printf("MALLOC_CAP_%s: Before %u bytes free, After %u bytes free (delta %d)\n",
           type, before_free, after_free, delta);
    TEST_ASSERT_MESSAGE(delta >= TEST_MEMORY_LEAK_THRESHOLD, "memory leak");
}

void setUp(void)
{
    clear_all_filters();
    before_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    before_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
}

void tearDown(void)
{
    size_t after_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    size_t after_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
    check_leak(before_free_8bit, after_free_8bit, "8BIT");
    check_leak(before_free_32bit, after_free_32bit, "32BIT");
}

void app_main(void)
{
    unity_run_menu();
}
