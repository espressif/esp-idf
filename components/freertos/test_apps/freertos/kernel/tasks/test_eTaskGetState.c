/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "test_utils.h"

/*
Test eTaskGetState()

Purpose:
    - Test that eTaskGetState() returns the correct state for a particular task

Procedure:
    - Create tasks in every state (and repeat for each core)
    - Note: eDeleted is not tested due to needing to control when the idle tasks run
    - Call eTaskGetState() on each created task

Expected:
    - eTaskGetState() should return the correct state for each created task
*/

static void blocked_task(void *arg)
{
    vTaskDelay(portMAX_DELAY-1);
    // Shouldn't need to self delete, but added for extra safety
    vTaskDelete(NULL);
}

static void suspended_task(void *arg)
{
    vTaskSuspend(NULL);
    // Shouldn't need to self delete, but added for extra safety
    vTaskDelete(NULL);
}

static void loop_task(void *arg)
{
    // Short delay to allow other created tasks to run
    vTaskDelay(2);
    while (1) {
        ;
    }
}

TEST_CASE("Test eTaskGetState", "[freertos]")
{
    TaskHandle_t blocked_tasks[portNUM_PROCESSORS];
    TaskHandle_t suspended_tasks[portNUM_PROCESSORS];
    TaskHandle_t ready_tasks[portNUM_PROCESSORS];
    TaskHandle_t running_tasks[portNUM_PROCESSORS];

    // Create tasks of each state on each core
    for (int i = 0; i < portNUM_PROCESSORS; i++) {
        TEST_ASSERT_EQUAL(pdPASS, xTaskCreatePinnedToCore(blocked_task, "blkd", configMINIMAL_STACK_SIZE * 2, NULL, UNITY_FREERTOS_PRIORITY - 1, &blocked_tasks[i], i));
        TEST_ASSERT_EQUAL(pdPASS, xTaskCreatePinnedToCore(suspended_task, "susp", configMINIMAL_STACK_SIZE * 2, NULL, UNITY_FREERTOS_PRIORITY - 1, &suspended_tasks[i], i));
        TEST_ASSERT_EQUAL(pdPASS, xTaskCreatePinnedToCore(loop_task, "rdy", configMINIMAL_STACK_SIZE * 2, NULL, UNITY_FREERTOS_PRIORITY - 1, &ready_tasks[i], i));
        if (i == UNITY_FREERTOS_CPU) {
            running_tasks[i] = xTaskGetCurrentTaskHandle();
        } else {
            xTaskCreatePinnedToCore(loop_task, "run", configMINIMAL_STACK_SIZE * 2, NULL, UNITY_FREERTOS_PRIORITY, &running_tasks[i], i);
        }
    }

    // Short delay to allow created tasks to run
    vTaskDelay(10);

    // Check the state of the created tasks
    for (int i = 0; i < portNUM_PROCESSORS; i++) {
        TEST_ASSERT_EQUAL(eBlocked, eTaskGetState(blocked_tasks[i]));
        TEST_ASSERT_EQUAL(eSuspended, eTaskGetState(suspended_tasks[i]));
        TEST_ASSERT_EQUAL(eReady, eTaskGetState(ready_tasks[i]));
        TEST_ASSERT_EQUAL(eRunning, eTaskGetState(running_tasks[i]));
    }

    // Clean up created tasks
    for (int i = 0; i < portNUM_PROCESSORS; i++) {
        vTaskDelete(blocked_tasks[i]);
        vTaskDelete(suspended_tasks[i]);
        vTaskDelete(ready_tasks[i]);
        if (i != UNITY_FREERTOS_CPU) {
            vTaskDelete(running_tasks[i]);
        }
    }

    // Short delay to allow task memory to be cleaned
    vTaskDelay(10);
}
