/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "esp_heap_caps.h"
#include "unity.h"
#include "memory_checks.h"
#ifdef CONFIG_HEAP_TRACING
#include "esp_heap_trace.h"
#endif

static size_t before_free_8bit;
static size_t before_free_32bit;

static size_t test_unity_leak_level[ESP_LEAK_TYPE_MAX][ESP_COMP_LEAK_ALL] = { 0 };

esp_err_t test_utils_set_leak_level(size_t leak_level, esp_type_leak_t type_of_leak, esp_comp_leak_t component)
{
    if (type_of_leak >= ESP_LEAK_TYPE_MAX || component >= ESP_COMP_LEAK_ALL) {
        return ESP_ERR_INVALID_ARG;
    }
    test_unity_leak_level[type_of_leak][component] = leak_level;
    return ESP_OK;
}

size_t test_utils_get_leak_level(esp_type_leak_t type_of_leak, esp_comp_leak_t component)
{
    size_t leak_level = 0;
    if (type_of_leak >= ESP_LEAK_TYPE_MAX || component > ESP_COMP_LEAK_ALL) {
        leak_level = 0;
    } else {
        if (component == ESP_COMP_LEAK_ALL) {
            for (int comp = 0; comp < ESP_COMP_LEAK_ALL; ++comp) {
                leak_level += test_unity_leak_level[type_of_leak][comp];
            }
        } else {
            leak_level = test_unity_leak_level[type_of_leak][component];
        }
    }
    return leak_level;
}

void test_utils_record_free_mem(void)
{
    before_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    before_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
}

void setup_heap_record(void)
{
#ifdef CONFIG_HEAP_TRACING
    const size_t num_heap_records = 80;
    static heap_trace_record_t *record_buffer;
    if (!record_buffer) {
        record_buffer = malloc(sizeof(heap_trace_record_t) * num_heap_records);
        assert(record_buffer);
        heap_trace_init_standalone(record_buffer, num_heap_records);
    }
#endif
}

static void check_leak(size_t before_free,
        size_t after_free,
        const char *type,
        size_t warn_threshold,
        size_t critical_threshold)
{
    int free_delta = (int)after_free - (int)before_free;
    printf("MALLOC_CAP_%s usage: Free memory delta: %d Leak threshold: -%u \n",
           type,
           free_delta,
           critical_threshold);

    if (free_delta > 0) {
        return; // free memory went up somehow
    }

    size_t leaked = (size_t)(free_delta * -1);
    if (leaked <= warn_threshold) {
        return;
    }
    else {
        printf("The test leaked more memory than warn threshold: Leaked: %d Warn threshold: -%u \n",
        leaked,
        warn_threshold);
    }

    printf("MALLOC_CAP_%s %s leak: Before %u bytes free, After %u bytes free (delta %u)\n",
           type,
           leaked <= critical_threshold ? "potential" : "critical",
           before_free, after_free, leaked);
    fflush(stdout);
    TEST_ASSERT_MESSAGE(leaked <= critical_threshold, "The test leaked too much memory");
}

void test_utils_finish_and_evaluate_leaks(size_t warn_threshold, size_t critical_threshold)
{
    size_t after_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    size_t after_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
    check_leak(before_free_8bit, after_free_8bit, "8BIT", warn_threshold, critical_threshold);
    check_leak(before_free_32bit, after_free_32bit, "32BIT", warn_threshold, critical_threshold);
}
