/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_utils.h"
#include "esp_heap_caps.h"

// iterator to load partition tables in `test spi bus lock, with flash` will lead memory not free
#define TEST_MEMORY_LEAK_THRESHOLD (350)

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
    //  ____  ____ ___   __  __           _
    // / ___||  _ \_ _| |  \/  | __ _ ___| |_ ___ _ __
    // \___ \| |_) | |  | |\/| |/ _` / __| __/ _ \ '__|
    //  ___) |  __/| |  | |  | | (_| \__ \ ||  __/ |
    // |____/|_|  |___| |_|  |_|\__,_|___/\__\___|_|

    printf("\n");
    printf("     ____  ____ ___   __  __           _            \n");
    printf("    / ___||  _ \\_ _| |  \\/  | __ _ ___| |_ ___ _ __ \n");
    printf("    \\___ \\| |_) | |  | |\\/| |/ _` / __| __/ _ \\ '__|\n");
    printf("     ___) |  __/| |  | |  | | (_| \\__ \\ ||  __/ |   \n");
    printf("    |____/|_|  |___| |_|  |_|\\__,_|___/\\__\\___|_|   \n");

    unity_run_menu();
}
