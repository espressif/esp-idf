/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "unity.h"
#include "unity_test_utils.h"
#include "esp_heap_caps.h"

// Some resources are lazy allocated, the threshold is left for that case
#define TEST_MEMORY_LEAK_THRESHOLD (1000)

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
    unity_utils_check_leak(before_free_8bit, after_free_8bit, "8BIT", TEST_MEMORY_LEAK_THRESHOLD);
    unity_utils_check_leak(before_free_32bit, after_free_32bit, "32BIT", TEST_MEMORY_LEAK_THRESHOLD);
}

void app_main(void)
{

    /*
        ______ _                _____ _    _      _____ _    _  _____ _____  ______ _   _ _____
        |  ____| |        /\    / ____| |  | |    / ____| |  | |/ ____|  __ \|  ____| \ | |  __ \
        | |__  | |       /  \  | (___ | |__| |   | (___ | |  | | (___ | |__) | |__  |  \| | |  | |
        |  __| | |      / /\ \  \___ \|  __  |    \___ \| |  | |\___ \|  ___/|  __| | . ` | |  | |
        | |    | |____ / ____ \ ____) | |  | |    ____) | |__| |____) | |    | |____| |\  | |__| |
        |_|    |______/_/    \_\_____/|_|  |_|   |_____/ \____/|_____/|_|    |______|_| \_|_____/


    */

    printf(" ______ _                _____ _    _      _____ _    _  _____ _____  ______ _   _ _____  \n");
    printf("|  ____| |        /\\    / ____| |  | |    / ____| |  | |/ ____|  __ \\|  ____| \\ | |  __ \\ \n");
    printf("| |__  | |       /  \\  | (___ | |__| |   | (___ | |  | | (___ | |__) | |__  |  \\| | |  | |\n");
    printf("|  __| | |      / /\\ \\  \\___ \\|  __  |    \\___ \\| |  | |\\___ \\|  ___/|  __| | . ` | |  | |\n");
    printf("| |    | |____ / ____ \\ ____) | |  | |    ____) | |__| |____) | |    | |____| |\\  | |__| |\n");
    printf("|_|    |______/_/    \\_\\_____/|_|  |_|   |_____/ \\____/|_____/|_|    |______|_| \\_|_____/ \n");

    unity_run_menu();
}
