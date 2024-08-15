/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "unity_test_runner.h"
#include "unity_test_utils_memory.h"
#include "esp_heap_caps.h"

// Some resources are lazy allocated in the sleep code, the threshold is left for that case
#define TEST_MEMORY_LEAK_THRESHOLD (-500)

void setUp(void)
{
    unity_utils_set_leak_level(TEST_MEMORY_LEAK_THRESHOLD);
    unity_utils_record_free_mem();
}

void tearDown(void)
{
    // Add a short delay of 100ms to allow the idle task to free any remaining memory
    vTaskDelay(pdMS_TO_TICKS(100));
    unity_utils_evaluate_leaks();
}

void app_main(void)
{
    unity_run_menu();
}
