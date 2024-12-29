/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_runner.h"
#include "esp_heap_caps.h"

// There should be no memory allocated during the test.
#define TEST_MEMORY_INITIAL_LEAK_THRESHOLD (-700)


static size_t before_free_8bit;
static size_t before_free_32bit;
static size_t actual_leak_threshold = TEST_MEMORY_INITIAL_LEAK_THRESHOLD;


static void check_leak(size_t before_free, size_t after_free, const char *type)
{
    ssize_t delta = after_free - before_free;
    printf("MALLOC_CAP_%s: Before %u bytes free, After %u bytes free (delta %d)\n", type, before_free, after_free, delta);
    TEST_ASSERT_MESSAGE(delta >= actual_leak_threshold, "memory leak");
}

void setUp(void)
{
    // load the partition table before measuring the initial free heap size.
    before_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    before_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
}

void tearDown(void)
{
    size_t after_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    size_t after_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
    check_leak(before_free_8bit, after_free_8bit, "8BIT");
    check_leak(before_free_32bit, after_free_32bit, "32BIT");

    // Reset the threshold to the zero after initial check
    actual_leak_threshold = 0;
}

void app_main(void)
{
    printf("Running nvs bootloader support component tests\n");
    unity_run_menu();
}
