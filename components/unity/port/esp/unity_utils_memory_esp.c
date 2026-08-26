/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_heap_caps.h"
#include "unity_test_utils.h"
#ifdef CONFIG_HEAP_TRACING
#include "esp_heap_trace.h"
#endif

static size_t s_before_free_8bit;
static size_t s_before_free_32bit;

void unity_utils_record_free_mem(void)
{
    s_before_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    s_before_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
}

void unity_utils_setup_heap_record(size_t num_heap_records)
{
#ifdef CONFIG_HEAP_TRACING
    static heap_trace_record_t *record_buffer;
    if (!record_buffer) {
        record_buffer = malloc(sizeof(heap_trace_record_t) * num_heap_records);
        assert(record_buffer);
        heap_trace_init_standalone(record_buffer, num_heap_records);
    }
#endif
}

void unity_utils_evaluate_leaks_direct(size_t threshold)
{
    size_t after_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    size_t after_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
    unity_utils_check_leak(s_before_free_8bit, after_free_8bit, "8BIT", threshold);
    unity_utils_check_leak(s_before_free_32bit, after_free_32bit, "32BIT", threshold);
}
