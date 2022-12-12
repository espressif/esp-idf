/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_utils.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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
    size_t after_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    size_t after_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
    printf("\n");
    unity_utils_check_leak(before_free_8bit, after_free_8bit, "8BIT", TEST_MEMORY_LEAK_THRESHOLD);
    unity_utils_check_leak(before_free_32bit, after_free_32bit, "32BIT", TEST_MEMORY_LEAK_THRESHOLD);
}

void app_main(void)
{
    //  _____         _     ____  ____  _  _    ___ ____
    // |_   _|__  ___| |_  |  _ \/ ___|| || |  ( o ) ___|
    //   | |/ _ \/ __| __| | |_) \___ \| || |_ / _ \__ `.
    //   | |  __/\__ \ |_  |  _ < ___) |__   _| (_) |__) |
    //   |_|\___||___/\__| |_| \_\____/   |_|  \___/____/
    printf("\n");
    printf("     _____         _     ____  ____  _  _    ___ ____\n");
    printf("    |_   _|__  ___| |_  |  _ \\/ ___|| || |  ( o ) ___|\n");
    printf("      | |/ _ \\/ __| __| | |_) \\___ \\| || |_ / _ \\__ `.\n");
    printf("      | |  __/\\__ \\ |_  |  _ < ___) |__   _| (_) |__) |\n");
    printf("      |_|\\___||___/\\__| |_| \\_\\____/   |_|  \\___/____/\n");

    unity_run_menu();
}
