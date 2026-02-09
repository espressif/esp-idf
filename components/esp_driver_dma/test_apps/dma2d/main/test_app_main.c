/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_runner.h"
#include "esp_heap_caps.h"
#include "esp_newlib.h"
#include "unity_test_utils.h"

// Some resources are lazy allocated in the driver, the threshold is left for that case
#define TEST_MEMORY_LEAK_THRESHOLD (200)

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
    printf(" ______     ___   ___     ______      _____   ______   \n");
    printf("(______)   (___)_(___)   (______)   _(_____) (______)  \n");
    printf("(_)   (_) (_)  (_)  (_) (_)____(_) (_)  _(_) (_)   (_) \n");
    printf("(_)    (_)(_)  (_)  (_) (________)    _(_)   (_)    (_)\n");
    printf("(_)___(_) (_)       (_) (_)    (_)   (_)___  (_)___(_) \n");
    printf("(______)  (_)       (_) (_)    (_) (_______) (______)  \n");

    unity_run_menu();
}
