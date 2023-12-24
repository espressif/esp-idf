/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_runner.h"
#include "unity_test_utils.h"
#include "esp_heap_caps.h"

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
