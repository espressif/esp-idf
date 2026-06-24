/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "portTestMacro.h"

/* ------------------------------------------------------------------------------------------------------------------ */

/*
Test Priority Scheduling (Single Core)

Purpose:
    - Test that the single-core scheduler always schedules the highest priority ready task
Procedure:
    - unityTask (highest priority) creates a binary semaphore (initially empty)
    - unityTask creates two lower-priority tasks that both block on the semaphore:
        - task_A (configMAX_PRIORITIES - 2)
        - task_B (configMAX_PRIORITIES - 3)
    - unityTask delays to let both tasks start and block on the semaphore
    - unityTask gives the semaphore once — FreeRTOS wakes the highest-priority
      waiter (task_A), which writes its ID to a shared variable and signals done
    - unityTask verifies the shared variable holds task_A's ID
Expected:
    - task_A (higher priority) wins the semaphore race, not task_B
*/

#if ( configNUM_CORES == 1 )

#define PRIO_TASK_A_ID  1
#define PRIO_TASK_B_ID  2

static volatile int s_prio_winner;
static SemaphoreHandle_t s_race_sem;
static SemaphoreHandle_t s_done_sem;

static void task_A(void *arg)
{
    /* Block until the race semaphore is given */
    xSemaphoreTake(s_race_sem, portMAX_DELAY);
    /* Higher priority: should be woken first */
    s_prio_winner = PRIO_TASK_A_ID;
    xSemaphoreGive(s_done_sem);
    vTaskSuspend(NULL);
}

static void task_B(void *arg)
{
    /* Block until the race semaphore is given */
    xSemaphoreTake(s_race_sem, portMAX_DELAY);
    /* Lower priority: should NOT be woken first */
    s_prio_winner = PRIO_TASK_B_ID;
    xSemaphoreGive(s_done_sem);
    vTaskSuspend(NULL);
}

TEST_CASE("Tasks: Test priority scheduling", "[freertos]")
{
    TaskHandle_t task_A_handle;
    TaskHandle_t task_B_handle;
    s_prio_winner = 0;

    /* Binary semaphores start empty — both tasks will block on take */
    s_race_sem = xSemaphoreCreateBinary();
    s_done_sem = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(s_race_sem);
    TEST_ASSERT_NOT_NULL(s_done_sem);

    /* Raise the priority of the unityTask */
    vTaskPrioritySet(NULL, configMAX_PRIORITIES - 1);

    /* Create task_A (higher prio) and task_B (lower prio) */
    xTaskCreate(task_A, "task_A", configTEST_DEFAULT_STACK_SIZE, NULL,
                configMAX_PRIORITIES - 2, &task_A_handle);
    xTaskCreate(task_B, "task_B", configTEST_DEFAULT_STACK_SIZE, NULL,
                configMAX_PRIORITIES - 3, &task_B_handle);

    /* Let both tasks start and block on s_race_sem */
    vTaskDelay(pdMS_TO_TICKS(50));

    /* Give the semaphore once. FreeRTOS wakes the highest-priority waiter
     * (task_A). Since unityTask is still higher priority, task_A won't
     * actually run until we block below. */
    xSemaphoreGive(s_race_sem);

    /* Block waiting for the winner to signal. This lets task_A run. */
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_done_sem, pdMS_TO_TICKS(200)));

    /* The higher-priority task_A should have won the race */
    TEST_ASSERT_EQUAL(PRIO_TASK_A_ID, s_prio_winner);

    /* Cleanup */
    vTaskDelete(task_A_handle);
    vTaskDelete(task_B_handle);
    vSemaphoreDelete(s_race_sem);
    vSemaphoreDelete(s_done_sem);

    /* Restore the priority of the unityTask */
    vTaskPrioritySet(NULL, configTEST_UNITY_TASK_PRIORITY);
}

#endif /* configNUM_CORES == 1 */
