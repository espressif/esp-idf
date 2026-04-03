/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_runner.h"
#include "esp_heap_caps.h"
#include "esp_newlib.h"
#include "unity_test_utils.h"

// Some resources are lazy allocated in the driver, the threshold is left for that case
#define TEST_MEMORY_LEAK_THRESHOLD (0)

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
    printf("                   _ ____               \n");
    printf("    _ __ ___  __ _(_)___ \\ ___         \n");
    printf("   | '__/ _ \\/ _` | | __) / __|        \n");
    printf("   | | |  __/ (_| | |/ __/ (__          \n");
    printf("   |_|  \\___|\\__, |_|_____\\___|      \n");
    printf("             |___/                      \n");

    unity_run_menu();
}
