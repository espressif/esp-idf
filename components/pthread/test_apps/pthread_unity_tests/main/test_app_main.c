/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <errno.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "unity_test_runner.h"
#include "unity_test_utils_memory.h"

// Some resources are lazy allocated (e.g. newlib locks), the threshold is left for that case
#define TEST_MEMORY_LEAK_THRESHOLD (-200)

void setUp(void)
{
    unity_utils_set_leak_level(TEST_MEMORY_LEAK_THRESHOLD);
    unity_utils_record_free_mem();
    errno = 0;
}

void tearDown(void)
{
#ifndef CONFIG_IDF_TARGET_LINUX // on Linux, we don't check for memory leaks with memory utils
    // Add a short delay of 200ms to allow the idle task to free remaining memory
    vTaskDelay(pdMS_TO_TICKS(200));
#endif // CONFIG_IDF_TARGET_LINUX

    unity_utils_evaluate_leaks();
}

void app_main(void)
{
    printf("Running pthread component unity tests\n");
    unity_run_menu();
}
