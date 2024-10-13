/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_types.h>
#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "esp_intr_alloc.h"
#include "esp_cpu.h"
#include "unity.h"
#include "test_utils.h"

#define NUMBER_OF_ITERATIONS 1023

static int compare_uint32(const void *a, const void *b)
{
    return (*(uint32_t *)a - * (uint32_t *)b);
}

static uint32_t calculate_median(uint32_t *values, int size)
{
    /* Sort the array of values containing the scheduling time of each iteration */
    qsort(values, size, sizeof(uint32_t), compare_uint32);

    /* Return the median value. (size is an odd number) */
    return values[size / 2];
}

typedef struct {
    SemaphoreHandle_t end_sema;
    uint32_t before_sched;
    uint32_t cycles_to_sched[NUMBER_OF_ITERATIONS];
    TaskHandle_t t1_handle;
} test_context_t;

static void test_task_1(void *arg)
{
    test_context_t *context = (test_context_t *)arg;

    for (;;) {
        context->before_sched = esp_cpu_get_cycle_count();
        taskYIELD();
    }

    vTaskDelete(NULL);
}

static void test_task_2(void *arg)
{
    test_context_t *context = (test_context_t *)arg;

    vTaskPrioritySet(NULL, CONFIG_UNITY_FREERTOS_PRIORITY + 1);
    vTaskPrioritySet(context->t1_handle, CONFIG_UNITY_FREERTOS_PRIORITY + 1);
    taskYIELD();

    for (int i = 0; i < NUMBER_OF_ITERATIONS; i++) {
        context->cycles_to_sched[i] = esp_cpu_get_cycle_count() - context->before_sched;
        taskYIELD();
    }

    xSemaphoreGive(context->end_sema);
    vTaskDelete(context->t1_handle);
    vTaskDelete(NULL);
}

TEST_CASE("scheduling time test", "[freertos]")
{
    test_context_t context;

    context.end_sema = xSemaphoreCreateBinary();
    TEST_ASSERT(context.end_sema != NULL);

#if !CONFIG_FREERTOS_UNICORE
    xTaskCreatePinnedToCore(test_task_1, "test1", 4096, &context, CONFIG_UNITY_FREERTOS_PRIORITY - 1, &context.t1_handle, 1);
    xTaskCreatePinnedToCore(test_task_2, "test2", 4096, &context, CONFIG_UNITY_FREERTOS_PRIORITY - 1, NULL, 1);
#else
    xTaskCreatePinnedToCore(test_task_1, "test1", 4096, &context, CONFIG_UNITY_FREERTOS_PRIORITY - 1, &context.t1_handle, 0);
    xTaskCreatePinnedToCore(test_task_2, "test2", 4096, &context, CONFIG_UNITY_FREERTOS_PRIORITY - 1, NULL, 0);
#endif

    BaseType_t result = xSemaphoreTake(context.end_sema, portMAX_DELAY);
    TEST_ASSERT_EQUAL_HEX32(pdTRUE, result);

    /* We calculate the median instead of the average to avoid outliers which may occur due to cache speed variabilities */
    uint32_t median_cycles = calculate_median(context.cycles_to_sched, NUMBER_OF_ITERATIONS);
    TEST_PERFORMANCE_LESS_THAN(SCHEDULING_TIME, "%"PRIu32" cycles", median_cycles);

    /* Cleanup */
    vSemaphoreDelete(context.end_sema);
}
