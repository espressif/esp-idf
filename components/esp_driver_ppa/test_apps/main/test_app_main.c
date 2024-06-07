/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_runner.h"
#include "esp_heap_caps.h"
#include "unity_test_utils.h"

// Some resources are lazy allocated in the driver, the threshold is left for that case
#define TEST_MEMORY_LEAK_THRESHOLD (300)

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
    printf(" ________  \n");
    printf("|___ ___ \\ \n");
    printf("   | \\_/ | \n");
    printf("   '.___.' \n");
    printf(" ________  \n");
    printf("|___ ___ \\ \n");
    printf("   | \\_/ | \n");
    printf("   '.___.' \n");
    printf(" _______   \n");
    printf("|__. _  '. \n");
    printf(" __||_/  / \n");
    printf("|______.'  \n");

    unity_run_menu();
}
