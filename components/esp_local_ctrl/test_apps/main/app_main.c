/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "unity.h"
#include "unity_test_runner.h"
#include "esp_heap_caps.h"

#define TEST_MEMORY_LEAK_THRESHOLD (-512)

static size_t s_before_free_8bit;
static size_t s_before_free_32bit;

static void check_leak(size_t before_free, size_t after_free, const char *type)
{
    ssize_t delta = after_free - before_free;
    printf("MALLOC_CAP_%s: Before %u bytes free, After %u bytes free (delta %d)\n",
           type, before_free, after_free, delta);
    TEST_ASSERT_MESSAGE(delta >= TEST_MEMORY_LEAK_THRESHOLD, "memory leak");
}

void setUp(void)
{
    s_before_free_8bit  = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    s_before_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
}

void tearDown(void)
{
    /* Catch heap corruption that would be caused by a double-free */
    TEST_ASSERT_MESSAGE(heap_caps_check_integrity(MALLOC_CAP_INVALID, true),
                        "heap integrity check failed (possible double-free or corruption)");

    size_t after_free_8bit  = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    size_t after_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
    check_leak(s_before_free_8bit,  after_free_8bit,  "8BIT");
    check_leak(s_before_free_32bit, after_free_32bit, "32BIT");
}

void app_main(void)
{
    printf("Running esp_local_ctrl component tests\n");
    unity_run_menu();
}
