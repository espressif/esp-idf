/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_utils.h"
#include "esp_heap_caps.h"

// Some resources are lazy allocated in the TWAI driver, the threshold is left for that case
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
    //  _______        ___    ___   _____         _
    // |_   _\ \      / / \  |_ _| |_   _|__  ___| |_
    //   | |  \ \ /\ / / _ \  | |    | |/ _ \/ __| __|
    //   | |   \ V  V / ___ \ | |    | |  __/\__ \ |_
    //   |_|    \_/\_/_/   \_\___|   |_|\___||___/\__|
    printf(" _______        ___    ___   _____         _\n");
    printf("|_   _\\ \\      / / \\  |_ _| |_   _|__  ___| |_\n");
    printf("  | |  \\ \\ /\\ / / _ \\  | |    | |/ _ \\/ __| __|\n");
    printf("  | |   \\ V  V / ___ \\ | |    | |  __/\\__ \\ |_\n");
    printf("  |_|    \\_/\\_/_/   \\_\\___|   |_|\\___||___/\\__|\n");
    unity_run_menu();
}
