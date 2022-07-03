/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"

#if CONFIG_FREERTOS_ENABLE_TASK_SNAPSHOT
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/task_snapshot.h"
#include "esp_cpu.h"
#include "unity.h"
#include "sdkconfig.h"

#define TEST_MAX_TASKS_NUM      32
#define NUM_TASKS_PER_LIST      2
#define TASK_PRIORITY           (configMAX_PRIORITIES - 2)

static void ready_task(void *arg)
{
    while (1) {
        ;
    }
}

static void blocked_task(void *arg)
{
    // Delay for portMAX_DELAY - 1 as not to go on the suspended list
    vTaskDelay(portMAX_DELAY - 1);
}

static void suspended_task(void *arg)
{
    vTaskSuspend(NULL);
}

static void setup(TaskHandle_t *task_list, int *num_tasks_ret, UBaseType_t *old_priority_ret)
{
    // Raise our priority so that we aren't preempted
    *old_priority_ret = uxTaskPriorityGet(NULL);
    vTaskPrioritySet(NULL, configMAX_PRIORITIES - 1);

    // Create tasks
    int num_tasks = 0;
    for (int i = 0; i < NUM_TASKS_PER_LIST; i++) {
        //Ready task
        xTaskCreate(ready_task, "ready", 1024, NULL, TASK_PRIORITY, &(task_list[num_tasks]));
        num_tasks++;
        //Blocked task
        xTaskCreate(blocked_task, "blkd", 1024, NULL, TASK_PRIORITY, &(task_list[num_tasks]));
        num_tasks++;
        //Suspended task
        xTaskCreate(suspended_task, "susp", 1024, NULL, TASK_PRIORITY, &(task_list[num_tasks]));
        num_tasks++;
    }
    *num_tasks_ret = num_tasks;
    // Short delay to allow tasks to spin up
    vTaskDelay(10);

    // Stop preemption on this core, and stall the other core
    taskDISABLE_INTERRUPTS();
#if !CONFIG_FREERTOS_UNICORE
    esp_cpu_stall(!xPortGetCoreID());
#endif


}

static void check_snapshots(TaskHandle_t *task_list, int num_tasks, TaskSnapshot_t *task_snapshots, UBaseType_t num_snapshots)
{
    // Check task snapshots. Every created task should be found in the task snapshot
    for (int i = 0; i < num_tasks; i++) {
        bool found = false;
        for (int j = 0; j < num_snapshots; j++) {
            if (task_list[i] == (TaskHandle_t)task_snapshots[j].pxTCB) {
                found = true;
                break;
            }
        }
        TEST_ASSERT(found);
    }
}

static void teardown(TaskHandle_t *task_list, int num_tasks, UBaseType_t old_priority)
{
    // Resume other cores and allow preemption
#if !CONFIG_FREERTOS_UNICORE
    esp_cpu_unstall(!xPortGetCoreID());
#endif
    taskENABLE_INTERRUPTS();

    for (int i = 0; i < num_tasks; i++) {
        vTaskDelete(task_list[i]);
    }
    // Restore priority
    vTaskPrioritySet(NULL, old_priority);
    // Short delay to allow tasks to clean up
    vTaskDelay(10);
}

TEST_CASE("Task snapshot: Get all", "[freertos]")
{
    // Short delay to allow both cores to spin up
    vTaskDelay(10);

    TaskHandle_t task_list[TEST_MAX_TASKS_NUM];
    int num_tasks;
    UBaseType_t old_priority;
    setup(task_list, &num_tasks, &old_priority);

    // Get task snapshots using uxTaskGetSnapshotAll()
    TaskSnapshot_t task_snapshots[TEST_MAX_TASKS_NUM];
    UBaseType_t tcb_size;
    UBaseType_t num_snapshots;
    num_snapshots = uxTaskGetSnapshotAll(task_snapshots, TEST_MAX_TASKS_NUM, &tcb_size);
    TEST_ASSERT_LESS_OR_EQUAL(TEST_MAX_TASKS_NUM, num_snapshots);

    check_snapshots(task_list, num_tasks, task_snapshots, num_snapshots);
    teardown(task_list, num_tasks, old_priority);
}

TEST_CASE("Task snapshot: Iterate", "[freertos]")
{
    // Short delay to allow both cores to spin up
    vTaskDelay(10);

    TaskHandle_t task_list[TEST_MAX_TASKS_NUM];
    int num_tasks;
    UBaseType_t old_priority;
    setup(task_list, &num_tasks, &old_priority);

    // Get task snapshots using pxTaskGetNext() and vTaskGetSnapshot()
    TaskSnapshot_t task_snapshots[TEST_MAX_TASKS_NUM];
    UBaseType_t num_snapshots = 0;
    TaskHandle_t cur_task_handle = pxTaskGetNext(NULL);
    while (cur_task_handle != NULL) {
        // Get the task's snapshot
        BaseType_t Result = vTaskGetSnapshot(cur_task_handle, &task_snapshots[num_snapshots]);
        TEST_ASSERT_EQUAL(pdTRUE, Result);
        num_snapshots++;
        cur_task_handle = pxTaskGetNext(cur_task_handle);
    }
    TEST_ASSERT_LESS_OR_EQUAL(TEST_MAX_TASKS_NUM, num_snapshots);

    check_snapshots(task_list, num_tasks, task_snapshots, num_snapshots);
    teardown(task_list, num_tasks, old_priority);
}

#endif // CONFIG_FREERTOS_ENABLE_TASK_SNAPSHOT
