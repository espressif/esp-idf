/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

void test_task(void *pvParameters)
{
    unity_run_menu();
}

void app_main(void)
{
    xTaskCreatePinnedToCore(test_task, "testTask", CONFIG_ETH_TEST_UNITY_TEST_TASK_STACK, NULL, CONFIG_ETH_TEST_UNITY_TEST_TASK_PRIO, NULL, tskNO_AFFINITY);
}
