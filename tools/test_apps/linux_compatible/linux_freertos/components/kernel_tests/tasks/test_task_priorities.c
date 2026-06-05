/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
  Unit tests for FreeRTOS task priority get/set
*/

#include <esp_types.h>
#include <stdio.h>
#include <strings.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "unity.h"

/*
 * A race task blocks on a shared binary semaphore. When the semaphore is given,
 * FreeRTOS wakes the highest-priority waiter, which writes its ID to s_winner
 * and signals s_done_sem. The task then suspends itself (it is single-shot).
 */

#define TASK_ID_A  1
#define TASK_ID_B  2

static volatile int s_winner;
static SemaphoreHandle_t s_race_sem;
static SemaphoreHandle_t s_done_sem;

static void race_task(void *arg)
{
    int my_id = (int)(uintptr_t)arg;
    xSemaphoreTake(s_race_sem, portMAX_DELAY);
    s_winner = my_id;
    xSemaphoreGive(s_done_sem);
    vTaskSuspend(NULL);
}

TEST_CASE("Get/Set Priorities", "[freertos]")
{
    TaskHandle_t task_a, task_b;

    s_race_sem = xSemaphoreCreateBinary();
    s_done_sem = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(s_race_sem);
    TEST_ASSERT_NOT_NULL(s_done_sem);

    /* Verify unity task's own priority */
    TEST_ASSERT_EQUAL(CONFIG_UNITY_FREERTOS_PRIORITY, uxTaskPriorityGet(NULL));

    /* --- Round 1: task_a has higher priority, should win the race --- */
    xTaskCreatePinnedToCore(race_task, "a", 2048, (void *)(uintptr_t)TASK_ID_A,
                            CONFIG_UNITY_FREERTOS_PRIORITY - 1, &task_a, 0);
    xTaskCreatePinnedToCore(race_task, "b", 2048, (void *)(uintptr_t)TASK_ID_B,
                            CONFIG_UNITY_FREERTOS_PRIORITY - 2, &task_b, 0);

    /* Verify created priorities */
    TEST_ASSERT_EQUAL(CONFIG_UNITY_FREERTOS_PRIORITY - 1, uxTaskPriorityGet(task_a));
    TEST_ASSERT_EQUAL(CONFIG_UNITY_FREERTOS_PRIORITY - 2, uxTaskPriorityGet(task_b));

    /* Let both tasks block on s_race_sem */
    vTaskDelay(pdMS_TO_TICKS(50));

    s_winner = 0;
    xSemaphoreGive(s_race_sem);
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_done_sem, pdMS_TO_TICKS(200)));
    TEST_ASSERT_EQUAL(TASK_ID_A, s_winner);

    vTaskDelete(task_a);
    vTaskDelete(task_b);

    /* --- Test vTaskPrioritySet API --- */
    xTaskCreatePinnedToCore(race_task, "p", 2048, (void *)(uintptr_t)TASK_ID_A,
                            CONFIG_UNITY_FREERTOS_PRIORITY - 1, &task_a, 0);
    TEST_ASSERT_EQUAL(CONFIG_UNITY_FREERTOS_PRIORITY - 1, uxTaskPriorityGet(task_a));
    vTaskPrioritySet(task_a, CONFIG_UNITY_FREERTOS_PRIORITY - 2);
    TEST_ASSERT_EQUAL(CONFIG_UNITY_FREERTOS_PRIORITY - 2, uxTaskPriorityGet(task_a));
    vTaskDelete(task_a);

    /* --- Round 2: swap priorities — task_b now higher, should win --- */
    xTaskCreatePinnedToCore(race_task, "a2", 2048, (void *)(uintptr_t)TASK_ID_A,
                            CONFIG_UNITY_FREERTOS_PRIORITY - 2, &task_a, 0);
    xTaskCreatePinnedToCore(race_task, "b2", 2048, (void *)(uintptr_t)TASK_ID_B,
                            CONFIG_UNITY_FREERTOS_PRIORITY - 1, &task_b, 0);

    /* Let both tasks block on s_race_sem */
    vTaskDelay(pdMS_TO_TICKS(50));

    s_winner = 0;
    xSemaphoreGive(s_race_sem);
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_done_sem, pdMS_TO_TICKS(200)));
    TEST_ASSERT_EQUAL(TASK_ID_B, s_winner);

    /* Cleanup */
    vTaskDelete(task_a);
    vTaskDelete(task_b);
    vSemaphoreDelete(s_race_sem);
    vSemaphoreDelete(s_done_sem);
}
