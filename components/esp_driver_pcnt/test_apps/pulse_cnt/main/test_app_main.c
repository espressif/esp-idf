/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_utils.h"
#include "esp_heap_caps.h"
#include "esp_newlib.h"

// Some resources are lazy allocated in pulse_cnt driver, the threshold is left for that case
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
    //  ____   ____ _   _ _____   _____         _
    // |  _ \ / ___| \ | |_   _| |_   _|__  ___| |_
    // | |_) | |   |  \| | | |     | |/ _ \/ __| __|
    // |  __/| |___| |\  | | |     | |  __/\__ \ |_
    // |_|    \____|_| \_| |_|     |_|\___||___/\__|
    printf(" ____   ____ _   _ _____   _____         _\r\n");
    printf("|  _ \\ / ___| \\ | |_   _| |_   _|__  ___| |_\r\n");
    printf("| |_) | |   |  \\| | | |     | |/ _ \\/ __| __|\r\n");
    printf("|  __/| |___| |\\  | | |     | |  __/\\__ \\ |_\r\n");
    printf("|_|    \\____|_| \\_| |_|     |_|\\___||___/\\__|\r\n");
    unity_run_menu();
}
