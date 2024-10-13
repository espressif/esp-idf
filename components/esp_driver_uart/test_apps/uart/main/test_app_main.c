/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_runner.h"
#include "unity_test_utils.h"
#include "esp_heap_caps.h"

#define TEST_MEMORY_LEAK_THRESHOLD (212)

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
    // _____         _     _   _   _    ____ _____
    //|_   _|__  ___| |_  | | | | / \  |  _ \_   _|
    //  | |/ _ \/ __| __| | | | |/ _ \ | |_) || |
    //  | |  __/\__ \ |_  | |_| / ___ \|  _ < | |
    //  |_|\___||___/\__|  \___/_/   \_\_| \_\|_|
    printf("\n");
    printf("     _____         _     _   _   _    ____ _____ \n");
    printf("    |_   _|__  ___| |_  | | | | / \\  |  _ \\_   _|\n");
    printf("      | |/ _ \\/ __| __| | | | |/ _ \\ | |_) || |  \n");
    printf("      | |  __/\\__ \\ |_  | |_| / ___ \\|  _ < | |  \n");
    printf("      |_|\\___||___/\\__|  \\___/_/   \\_\\_| \\_\\|_|  \n");

    unity_run_menu();
}
