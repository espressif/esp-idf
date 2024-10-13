/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <stdio.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_private/freertos_debug.h"
#include "esp_cpu.h"
#include "esp_rom_sys.h"
#include "unity.h"
#include "sdkconfig.h"

#define TEST_MAX_TASKS_NUM      32
#define NUM_TASKS_PER_LIST      2
#define TASK_PRIORITY           (configMAX_PRIORITIES - 2)

static volatile bool start_ready_flag = false;
static volatile bool start_resp_flag = false;
static volatile bool stop_flag = false;

static void ready_task(void *arg)
{
    while (1) {
#if !CONFIG_FREERTOS_UNICORE
        taskDISABLE_INTERRUPTS();
        /*
        The main task (on core 0) has a higher priority than this task. Thus, by the time this flag is set, this task
        is guaranteed to running on core 1.
        */
        if (start_ready_flag) {   // Is the test starting?
            start_resp_flag = true;   // Indicate we are ready to start
            while (!stop_flag) {
                ;   // Wait until test completes
            }

            // Reset the flags
            start_ready_flag = false;
            start_resp_flag = false;
            stop_flag = false;
        }
        taskENABLE_INTERRUPTS();
        // Short non-blocking delay to allow INT WDT to feed
        esp_rom_delay_us(1000);
#endif // !CONFIG_FREERTOS_UNICORE
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

    /*
    Task snapshot functions are only meant to be called when FreeRTOS is no longer running (e.g., during a panic),
    thus assumes that nothing else (interrupt or the other core) will access the task lists simultaneously. However, for
    unit tests, FreeRTOS is still running. So we must guarantee thread safe access another way.

    - For single core, simply disabling interrupts will guarantee that the current task is the only thread to access the
      task lists
    - For SMP, we also need to stop the other core from accessing the task lists. Thus, we use a 2-way handshake to put
      the other core into a while loop with interrupts disabled.
    */
    // Disable interrupts so that the current core isn't preempted.
    taskDISABLE_INTERRUPTS();
#if !CONFIG_FREERTOS_UNICORE
    // For SMP, we need to do a 2-way handshake to ensure the other core spins with interrupts disabled.
    start_ready_flag = true;
    while (!start_resp_flag) {
        ;
    }
#endif // !CONFIG_FREERTOS_UNICORE
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
#if !CONFIG_FREERTOS_UNICORE
    // Signal to the other core that it can exit the loop
    stop_flag = true;
#endif // !CONFIG_FREERTOS_UNICORE
    // Reenable interrupts on the current core.
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

    // Get task snapshots using xTaskGetNext() and vTaskGetSnapshot()
    TaskSnapshot_t task_snapshots[TEST_MAX_TASKS_NUM];
    UBaseType_t num_snapshots = 0;
    TaskIterator_t task_iterator = {0};
    while (xTaskGetNext(&task_iterator) != -1) {
        if (task_iterator.pxTaskHandle != NULL) {
            // Get the task's snapshot
            BaseType_t Result = vTaskGetSnapshot(task_iterator.pxTaskHandle, &task_snapshots[num_snapshots]);
            TEST_ASSERT_EQUAL(pdTRUE, Result);
            num_snapshots++;
        }
    }
    TEST_ASSERT_LESS_OR_EQUAL(TEST_MAX_TASKS_NUM, num_snapshots);

    check_snapshots(task_list, num_tasks, task_snapshots, num_snapshots);
    teardown(task_list, num_tasks, old_priority);
}
