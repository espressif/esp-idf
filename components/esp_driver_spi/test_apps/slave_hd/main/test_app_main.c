/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
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
    //  ____  ____ ___   ____  _                   _   _ ____
    // / ___||  _ \_ _| / ___|| | __ ___   _____  | | | |  _ `.
    // \___ \| |_) | |  \___ \| |/ _` \ \ / / _ \ | |_| | | | |
    //  ___) |  __/| |   ___) | | (_| |\ V /  __/ |  _  | |_| |
    // |____/|_|  |___| |____/|_|\__,_| \_/ \___| |_| |_|____/

    printf("\n");
    printf("     ____  ____ ___   ____  _                   _   _ ____  \n");
    printf("    / ___||  _ \\_ _| / ___|| | __ ___   _____  | | | |  _ \\ \n");
    printf("    \\___ \\| |_) | |  \\___ \\| |/ _` \\ \\ / / _ \\ | |_| | | | |\n");
    printf("     ___) |  __/| |   ___) | | (_| |\\ V /  __/ |  _  | |_| |\n");
    printf("    |____/|_|  |___| |____/|_|\\__,_| \\_/ \\___| |_| |_|____/ \n");

    unity_run_menu();
}
