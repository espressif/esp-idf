/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_runner.h"
#include "unity_test_utils.h"
#include "esp_heap_caps.h"
#include "esp_newlib.h"

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
    printf(" __    __   __    __    ______  __ \n");
    printf("|  |  |  | |  |  |  |  /      ||  |\n");
    printf("|  |  |  | |  |__|  | |  ,----'|  |\n");
    printf("|  |  |  | |   __   | |  |     |  |\n");
    printf("|  `--'  | |  |  |  | |  `----.|  |\n");
    printf(" \\______/  |__|  |__|  \\______||__|\n");

    unity_run_menu();
}
