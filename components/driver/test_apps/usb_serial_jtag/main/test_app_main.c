/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "unity_test_utils.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TEST_MEMORY_LEAK_THRESHOLD (200)

static size_t before_free_8bit;
static size_t before_free_32bit;

void setUp(void)
{
    before_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    before_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
}

void tearDown(void)
{
    size_t after_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    size_t after_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
    printf("\n");
    unity_utils_check_leak(before_free_8bit, after_free_8bit, "8BIT", TEST_MEMORY_LEAK_THRESHOLD);
    unity_utils_check_leak(before_free_32bit, after_free_32bit, "32BIT", TEST_MEMORY_LEAK_THRESHOLD);
}

void app_main(void)
{
    //  _    _  _____ ____    _____ ______ _____  _____          _
    // | |  | |/ ____|  _ \  / ____|  ____|  __ \|_   _|   /\   | |
    // | |  | | (___ | |_) || (___ | |__  | |__) | | |    /  \  | |
    // | |  | |\___ \|  _ <  \___ \|  __| |  _  /  | |   / /\ \ | |
    // | |__| |____) | |_) | ____) | |____| | \ \ _| |_ / ____ \| |____
    //  \____/|_____/|____/ |_____/|______|_|  \_\_____/_/    \_\______|
    printf("\n");
    printf(" _    _  _____ ____    _____ ______ _____  _____          _      \n");
    printf("| |  | |/ ____|  _ \\  / ____|  ____|  __ \\|_   _|   /\\   | |     \n");
    printf("| |  | | (___ | |_) || (___ | |__  | |__) | | |    /  \\  | |     \n");
    printf("| |  | |\\___ \\|  _ <  \\___ \\|  __| |  _  /  | |   / /\\ \\ | |     \n");
    printf("| |__| |____) | |_) | ____) | |____| | \\ \\ _| |_ / ____ \\| |____ \n");
    printf(" \\____/|_____/|____/ |_____/|______|_|  \\_\\_____/_/    \\_\\______|\n");

    unity_run_menu();
}
