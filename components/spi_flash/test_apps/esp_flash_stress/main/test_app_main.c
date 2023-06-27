/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_utils.h"
#include "esp_heap_caps.h"

// load partition table in tests will use memory
#define TEST_MEMORY_LEAK_THRESHOLD (600)

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
    printf(" ______  _____ _____    ______ _           _        _____ _                     \n");
    printf("|  ____|/ ____|  __ \\  |  ____| |         | |      / ____| |                    \n");
    printf("| |__  | (___ | |__) | | |__  | | __ _ ___| |__   | (___ | |_ _ __ ___  ___ ___ \n");
    printf("|  __|  \\___ \\|  ___/  |  __| | |/ _` / __| '_ \\   \\___ \\| __| '__/ _ \\/ __/ __|\n");
    printf("| |____ ____) | |      | |    | | (_| \\__ \\ | | |  ____) | |_| | |  __/\\__ \\__ \\\n");
    printf("|______|_____/|_|      |_|    |_|\\__,_|___/_| |_| |_____/ \\__|_|  \\___||___/___/\n");

    unity_run_menu();
}
