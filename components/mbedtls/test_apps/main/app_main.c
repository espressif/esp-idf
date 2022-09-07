/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "test_utils.h"

static void test_task(void *pvParameters)
{
    vTaskDelay(2); /* Delay a bit to let the main task be deleted */
    unity_run_menu();
}

void app_main(void)
{
    xTaskCreatePinnedToCore(test_task, "testTask", UNITY_FREERTOS_STACK_SIZE, NULL, UNITY_FREERTOS_PRIORITY, NULL, UNITY_FREERTOS_CPU);
}
