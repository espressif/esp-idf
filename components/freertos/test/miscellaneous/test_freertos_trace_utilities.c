/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Test FreeRTOS trace facility functions. These following functions are enabled
 * when configUSE_TRACE_FACILITY is defined 1 in FreeRTOS.
 * Tasks:           uxTaskGetTaskNumber(), uxTaskSetTaskNumber()
 * Queues:          ucQueueGetQueueType(), vQueueSetQueueNumber(), uxQueueGetQueueNumber()
 * Event Groups:    xEventGroupSetBitsFromISR(), xEventGroupClearBitsFromISR(), uxEventGroupGetNumber()
 *
 * Note: uxTaskGetSystemState() is tested in a separate unit test
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "unity.h"
#include "test_utils.h"

#ifdef CONFIG_FREERTOS_USE_TRACE_FACILITY
#define TSK_PRIORITY    (UNITY_FREERTOS_PRIORITY + 1)

#define NO_OF_CORES portNUM_PROCESSORS
#define BIN_SEM_QUEUE_TYPE queueQUEUE_TYPE_BINARY_SEMAPHORE     //Expected Queue Type

static QueueHandle_t test_queues[NO_OF_CORES];
static TaskHandle_t task_handles[NO_OF_CORES];

void task_test_trace_utilities(void *arg)
{
    int core = xPortGetCoreID();
    TaskHandle_t handle = xTaskGetCurrentTaskHandle();
    uint32_t id = (uint32_t)arg;

    vTaskSetTaskNumber(handle, (UBaseType_t)id);    //cast and store id as task number
    vQueueSetQueueNumber(test_queues[core], id);    //store id as queue number

    //Wait to start
    xSemaphoreTake(test_queues[core], portMAX_DELAY);

    //Tests on this core
    TEST_ASSERT(uxTaskGetTaskNumber(task_handles[core]) == (0x0F << (core)));
    TEST_ASSERT(uxQueueGetQueueNumber(test_queues[core]) == (0x0F << (core)));
    TEST_ASSERT(ucQueueGetQueueType(test_queues[core]) == BIN_SEM_QUEUE_TYPE)

    //Test on other core
#ifndef CONFIG_FREERTOS_UNICORE
    TEST_ASSERT(uxTaskGetTaskNumber(task_handles[!core]) == (0x0F << (!core)));
    TEST_ASSERT(uxQueueGetQueueNumber(test_queues[!core]) == (0x0F << (!core)));
    TEST_ASSERT(ucQueueGetQueueType(test_queues[!core]) == BIN_SEM_QUEUE_TYPE)
#endif

    xSemaphoreGive(test_queues[core]);      //Signal done
    vTaskDelete(NULL);
}

TEST_CASE("Test freertos trace facility functions", "[freertos]")
{
    for(int i = 0; i < NO_OF_CORES; i++){
        test_queues[i] = xSemaphoreCreateBinary();   //Create a queue as binary semaphore for each core
        xTaskCreatePinnedToCore(task_test_trace_utilities, "Test Task", 4096, (void *)(0x0F << i), TSK_PRIORITY, &task_handles[i], i);
    }

    vTaskDelay(10);

    //Start the tasks
    for(int i = NO_OF_CORES - 1; i >= 0; i--){
        xSemaphoreGive(test_queues[i]);
    }

    vTaskDelay(10); //Small delay to ensure semaphores are taken

    //Wait for done
    for(int i = 0; i < NO_OF_CORES; i++){
        xSemaphoreTake(test_queues[i], portMAX_DELAY);
        vSemaphoreDelete(test_queues[i]);
    }

    vTaskDelay(10);     //Give time for idle task to clean up
}


#define MAX_TASKS           15
#define TASKS_TO_CREATE     5

static TaskHandle_t created_handles[TASKS_TO_CREATE];
static TaskStatus_t *tsk_status_array;

void created_task(void* arg)
{
    while(1){
        vTaskDelay(100);
    }
}

TEST_CASE("Test freertos uxTaskGetSystemState", "[freertos]")
{
    tsk_status_array = calloc(MAX_TASKS, sizeof(TaskStatus_t));
    for(int i = 0; i < TASKS_TO_CREATE; i++){
        xTaskCreatePinnedToCore(created_task, "Created Task", 1024, NULL, TSK_PRIORITY, &created_handles[i], 0);
    }

    //Get System states
    int no_of_tasks = uxTaskGetSystemState(tsk_status_array, MAX_TASKS, NULL);
    TEST_ASSERT((no_of_tasks > 0) && (no_of_tasks <= MAX_TASKS));

    //Check if get system state has got all created tasks
    bool not_found = false;
    for(int i = 0; i < TASKS_TO_CREATE; i++){
        bool found = false;
        for(int j = 0; j < MAX_TASKS; j++){
            if(tsk_status_array[j].xHandle == created_handles[i]){
                found = true;
                break;
            }
        }
        if(!found){
            not_found = true;
            break;
        }
    }
    TEST_ASSERT(not_found == false);

    //Cleanup
    for(int i = 0; i < TASKS_TO_CREATE; i++){
        vTaskDelete(created_handles[i]);
    }
    free(tsk_status_array);
    vTaskDelay(10);
}

#endif //CONFIG_FREERTOS_USE_TRACE_FACILITY
