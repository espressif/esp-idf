/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "unity.h"
#include "unity_test_utils.h"
#include "esp_heap_caps.h"

// Some resources are lazy allocated, the threadhold is left for that case
#define TEST_MEMORY_LEAK_THRESHOLD (300)

static size_t before_free_8bit;
static size_t before_free_32bit;

void setUp(void)
{
    before_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    before_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
}

void tearDown(void)
{
    size_t after_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    size_t after_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
    unity_utils_check_leak(before_free_8bit, after_free_8bit, "8BIT", TEST_MEMORY_LEAK_THRESHOLD);
    unity_utils_check_leak(before_free_32bit, after_free_32bit, "32BIT", TEST_MEMORY_LEAK_THRESHOLD);
}

void app_main(void)
{
    /*
       _____ ____  ________     _____ __
      / ___// __ \/  _/ __ \   / ___// /___ __   _____
      \__ \/ / / // // / / /   \__ \/ / __ `/ | / / _ \
     ___/ / /_/ // // /_/ /   ___/ / / /_/ /| |/ /  __/
    /____/_____/___/\____/   /____/_/\__,_/ |___/\___/

    */

    printf("   _____ ____  ________     _____ __               \n");
    printf("  / ___// __ \\/  _/ __ \\   / ___// /___ __   _____ \n");
    printf("  \\__ \\/ / / // // / / /   \\__ \\/ / __ `/ | / / _ \\\n");
    printf(" ___/ / /_/ // // /_/ /   ___/ / / /_/ /| |/ /  __/\n");
    printf("/____/_____/___/\\____/   /____/_/\\__,_/ |___/\\___/\n");

    unity_run_menu();
}
