/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_utils.h"
#include "esp_heap_caps.h"

// iterator to load partition tables in `test spi bus lock, with flash` will lead memory not free
#define TEST_MEMORY_LEAK_THRESHOLD (250)

static size_t before_free_8bit;
static size_t before_free_32bit;

void setUp(void)
{
    before_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    before_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
}

void tearDown(void)
{
    esp_reent_cleanup();    //clean up some of the newlib's lazy allocations
    size_t after_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    size_t after_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
    printf("\n");
    unity_utils_check_leak(before_free_8bit, after_free_8bit, "8BIT", TEST_MEMORY_LEAK_THRESHOLD);
    unity_utils_check_leak(before_free_32bit, after_free_32bit, "32BIT", TEST_MEMORY_LEAK_THRESHOLD);
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
