/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "unity.h"
#include "unity_test_utils_memory.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "test_sublayer_common.h"

#ifndef CONFIG_ETH_SUBLAYER_SUPPORT
#error "This test app requires CONFIG_ETH_SUBLAYER_SUPPORT"
#endif

#define TEST_TASK_STACK_SIZE 8192
#define TEST_TASK_PRIORITY   5
/* Some resources are lazy allocated in lwIP which we don't have under control */
#define TEST_MEMORY_LEAK_THRESHOLD 400

void setUp(void)
{
    unity_utils_record_free_mem();
}

void tearDown(void)
{
    sublayer_test_force_teardown();
    unity_utils_evaluate_leaks_direct(TEST_MEMORY_LEAK_THRESHOLD);
}

static void test_task(void *pvParameters)
{
    unity_run_menu();
}

void app_main(void)
{
    xTaskCreatePinnedToCore(test_task, "testTask", TEST_TASK_STACK_SIZE,
                            NULL, TEST_TASK_PRIORITY, NULL, tskNO_AFFINITY);
}
