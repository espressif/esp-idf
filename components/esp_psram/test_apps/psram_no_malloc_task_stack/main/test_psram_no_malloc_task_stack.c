/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include "esp_log.h"
#include "unity.h"
#include "unity_test_runner.h"
#include "unity_test_utils_memory.h"
#include "unity_test_utils.h"
#include "esp_heap_caps.h"
#include "esp_memory_utils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "psram_no_malloc_task_stack_test";

#define STACK_SIZE 4096
#define WAIT_TICKS 2
#define TEST_TASK_PRIORITY 6 // relatively high priority to let task finish quickly

void setUp(void)
{
    unity_utils_set_leak_level(0);
    unity_utils_record_free_mem();
}

void tearDown(void)
{
    unity_utils_evaluate_leaks();
}

static uint8_t *stack_memory;
static StaticTask_t *tcb_memory;
static bool is_external;
static SemaphoreHandle_t task_waiter;

static void test_task(void *arg)
{
    int dummy = 47;
    is_external = esp_ptr_external_ram(&dummy);
    xSemaphoreGive(task_waiter);
    vTaskDelay(portMAX_DELAY);
}

TEST_CASE("FreeRTOS task with stack on SPIRAM works", "[psram]")
{
    stack_memory = heap_caps_malloc(STACK_SIZE, MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
    TEST_ASSERT_NOT_NULL(stack_memory);
    tcb_memory = heap_caps_malloc(sizeof(StaticTask_t), MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(tcb_memory);
    task_waiter = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(task_waiter);

    TaskHandle_t task = xTaskCreateStatic(test_task,
                                          "heap caps static",
                                          STACK_SIZE,
                                          NULL,
                                          TEST_TASK_PRIORITY,
                                          stack_memory,
                                          tcb_memory);
    TEST_ASSERT_NOT_NULL(task);

    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(task_waiter, WAIT_TICKS));

    TEST_ASSERT_EQUAL(true, is_external);

    // use unity_utils_task_delete() to avoid deleting stack of a still running task
    unity_utils_task_delete(task);

    vSemaphoreDelete(task_waiter);
    heap_caps_free(tcb_memory);
    heap_caps_free(stack_memory);
}

void app_main(void)
{
    ESP_LOGI(TAG, "Running PSRAM task stack test app with SPIRAM_USE_CAPS_ALLOC");

    unity_run_menu();
}
