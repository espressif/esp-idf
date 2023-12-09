/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_utils.h"
#include "esp_heap_caps.h"

// Some resources are lazy allocated in LEDC driver, the threshold is left for that case
#define TEST_MEMORY_LEAK_THRESHOLD (230)

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

    //  _____         _     _     _____ ____   ____
    // |_   _|__  ___| |_  | |   | ____|  _ \ / ___|
    //   | |/ _ \/ __| __| | |   |  _| | | | | |
    //   | |  __/\__ \ |_  | |___| |___| |_| | |___
    //   |_|\___||___/\__| |_____|_____|____/ \____|

    printf("\n");
    printf("  _____         _     _     _____ ____   ____ \n");
    printf(" |_   _|__  ___| |_  | |   | ____|  _ \\ / ___|\n");
    printf("   | |/ _ \\/ __| __| | |   |  _| | | | | |    \n");
    printf("   | |  __/\\__ \\ |_  | |___| |___| |_| | |___ \n");
    printf("   |_|\\___||___/\\__| |_____|_____|____/ \\____|\n");
    unity_run_menu();
}
