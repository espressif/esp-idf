/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_runner.h"
#include "unity_test_utils_memory.h"
#include "esp_heap_caps.h"

// Some resources are lazy allocated in JPEG driver, so we reserved this threshold when checking memory leak
// A better way to check a potential memory leak is running a same case by twice, for the second time, the memory usage delta should be zero
#define LEAKS (500)

void setUp(void)
{
    unity_utils_record_free_mem();
}

void tearDown(void)
{
    unity_utils_evaluate_leaks_direct(LEAKS);
}

void app_main(void)
{
    //      _ ____  _____ ____   _____ _____ ____ _____
    //     | |  _ \| ____/ ___| |_   _| ____/ ___|_   _|
    //  _  | | |_) |  _|| |  _    | | |  _| \___ \ | |
    // | |_| |  __/| |__| |_| |   | | | |___ ___) || |
    //  \___/|_|   |_____\____|   |_| |_____|____/ |_|

    printf("     _ ____  _____ ____   _____ _____ ____ _____ \n");
    printf("    | |  _ \\| ____/ ___| |_   _| ____/ ___|_   _|\n");
    printf(" _  | | |_) |  _|| |  _    | | |  _| \\___ \\ | |  \n");
    printf("| |_| |  __/| |__| |_| |   | | | |___ ___) || |  \n");
    printf(" \\___/|_|   |_____\\____|   |_| |_____|____/ |_|  \n");

    unity_run_menu();
}
