/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "unity.h"
#include "unity_test_utils.h"
#include "esp_heap_caps.h"

// Some resources are lazy allocated, the threadhold is left for that case
#define TEST_MEMORY_LEAK_THRESHOLD (500)

void setUp(void)
{
    unity_utils_record_free_mem();
}

void tearDown(void)
{
    esp_reent_cleanup();    //clean up some of the newlib's lazy allocations
    unity_utils_evaluate_leaks_direct(TEST_MEMORY_LEAK_THRESHOLD);
}

void app_main(void)
{
    unity_run_menu();
}
