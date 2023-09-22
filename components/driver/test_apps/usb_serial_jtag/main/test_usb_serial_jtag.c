/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include <sys/param.h>

#include "unity.h"

#include "driver/usb_serial_jtag.h" // for the usb serial/jtag driver access
#include "esp_log.h"
#include "esp_vfs_dev.h"
#include "esp_vfs_usb_serial_jtag.h" // for the redirection of stdio

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#define TASK_NUM (4)
#define REPEAT_NUM (100)

static const char TAG[] = "USB Serial/JTAG";

static volatile bool case_end = false;
static SemaphoreHandle_t s_sem[TASK_NUM];

static void test_task_shared(void *pvParameter)
{
    const int task_id = (const int)pvParameter;
    int i = REPEAT_NUM;
    while (i--) {
        ESP_LOGI(TAG, "%i print lots of message from %i tasks", task_id, TASK_NUM);
        vTaskDelay(random() % TASK_NUM);
    }

    vTaskDelay(100);

    TEST_ASSERT(xSemaphoreGive(s_sem[task_id]) == pdTRUE);
    vTaskDelete(NULL);
}

static void test_task_driver(void *pvParameters)
{
    char task_name_buf[configMAX_TASK_NAME_LEN] = {0};

    for (int i = 0; i < TASK_NUM; ++i) {
        snprintf(task_name_buf, sizeof(task_name_buf), "tsk%i", i);

        TEST_ASSERT((s_sem[i] = xSemaphoreCreateBinary()) != NULL);
        TEST_ASSERT(xTaskCreatePinnedToCore(&test_task_shared, task_name_buf, 3072,
            (void *)i, 6, NULL, i % configNUM_CORES) == pdPASS);
    }

    for (int i = 0; i < TASK_NUM; ++i) {
        TEST_ASSERT(xSemaphoreTake(s_sem[i], portMAX_DELAY) == pdTRUE);
        vSemaphoreDelete(s_sem[i]);
    }

    vTaskDelay(2);

    case_end = true;
    vTaskDelete(NULL);
}

TEST_CASE("test print to usb serial/jtag from multiple apps", "[usb_serial_jtag]")
{
    xTaskCreate(test_task_driver, "tskdrv", 4096, NULL, 5, NULL);
    while (!case_end);
    vTaskDelay(2); // wait for test_task4 to exit
    case_end = false;
}
