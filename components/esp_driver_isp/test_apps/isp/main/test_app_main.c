/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "unity.h"
#include "unity_test_utils.h"
#include "esp_heap_caps.h"
#include "sdkconfig.h"

#define TEST_MEMORY_LEAK_THRESHOLD (400)

void setUp(void)
{
    unity_utils_record_free_mem();
}

void tearDown(void)
{
    unity_utils_evaluate_leaks_direct(TEST_MEMORY_LEAK_THRESHOLD);
}

void app_main(void)
{
    /*
     ____________________  ___________
    /_  __/ __/ __/_  __/ /  _/ __/ _ \
     / / / _/_\ \  / /   _/ /_\ \/ ___/
    /_/ /___/___/ /_/   /___/___/_/

    */

    printf(" ____________________  ___________\n");
    printf("/_  __/ __/ __/_  __/ /  _/ __/ _ \\\n");
    printf(" / / / _/_\\ \\  / /   _/ /_\\ \\/ ___/\n");
    printf("/_/ /___/___/ /_/   /___/___/_/\n");

    unity_run_menu();
}
