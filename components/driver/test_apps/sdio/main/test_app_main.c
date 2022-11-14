/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_utils.h"
#include "esp_heap_caps.h"


#define TEST_MEMORY_LEAK_THRESHOLD (200)

static size_t before_free_8bit;
static size_t before_free_32bit;

void setUp(void)
{
    before_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    before_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
}

void tearDown(void)
{
    esp_reent_cleanup();    //clean up some of the newlib's lazy allocations
    size_t after_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    size_t after_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
    printf("\n");
    unity_utils_check_leak(before_free_8bit, after_free_8bit, "8BIT", TEST_MEMORY_LEAK_THRESHOLD);
    unity_utils_check_leak(before_free_32bit, after_free_32bit, "32BIT", TEST_MEMORY_LEAK_THRESHOLD);
}

void app_main(void)
{
    //  _____         _             _ _
    // |_   _|__  ___| |_   ___  __| (_) ___
    //   | |/ _ \/ __| __| / __|/ _` | |/ _ `.
    //   | |  __/\__ \ |_  \__ \ (_| | | (_) |
    //   |_|\___||___/\__| |___/\__,_|_|\___/

    printf("     _____         _             _ _       \n");
    printf("    |_   _|__  ___| |_   ___  __| (_) ___  \n");
    printf("      | |/ _ \\/ __| __| / __|/ _` | |/ _ \\ \n");
    printf("      | |  __/\\__ \\ |_  \\__ \\ (_| | | (_) |\n");
    printf("      |_|\\___||___/\\__| |___/\\__,_|_|\\___/ \n");

    unity_run_menu();
}
