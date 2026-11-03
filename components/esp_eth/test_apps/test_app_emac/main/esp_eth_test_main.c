/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TEST_TASK_STACK_SIZE 4096
#define TEST_TASK_PRIORITY   5

void test_task(void *pvParameters)
{
    unity_run_menu();
}

void app_main(void)
{
    xTaskCreatePinnedToCore(test_task, "testTask", TEST_TASK_STACK_SIZE, NULL, TEST_TASK_PRIORITY, NULL, tskNO_AFFINITY);
}
