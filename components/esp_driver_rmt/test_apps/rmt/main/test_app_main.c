/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_runner.h"
#include "unity_test_utils.h"
#include "esp_heap_caps.h"

// Some resources are lazy allocated in RMT driver, so we reserved this threadhold when checking memory leak
// A better way to check a potential memory leak is running a same case by twice, for the second time, the memory usage delta should be zero
#define TEST_MEMORY_LEAK_THRESHOLD (400)

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
    //  ____  __  __ _____   _____         _
    // |  _ \|  \/  |_   _| |_   _|__  ___| |_
    // | |_) | |\/| | | |     | |/ _ \/ __| __|
    // |  _ <| |  | | | |     | |  __/\__ \ |_
    // |_| \_\_|  |_| |_|     |_|\___||___/\__|
    printf(" ____  __  __ _____   _____         _\r\n");
    printf("|  _ \\|  \\/  |_   _| |_   _|__  ___| |_\r\n");
    printf("| |_) | |\\/| | | |     | |/ _ \\/ __| __|\r\n");
    printf("|  _ <| |  | | | |     | |  __/\\__ \\ |_\r\n");
    printf("|_| \\_\\_|  |_| |_|     |_|\\___||___/\\__|\r\n");
    unity_run_menu();
}
