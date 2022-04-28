/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Test FreeRTOS debug functions and utilities.
 * - Queue registry functions vQueueAddToRegistry(), vQueueUnregisterQueue(),
 *   and pcQueueGetName(backported)
 *
 *
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "unity.h"
#include "test_utils.h"

#if (CONFIG_FREERTOS_QUEUE_REGISTRY_SIZE > 0)
#define NO_OF_QUEUES_PER_CORE  ((int)((CONFIG_FREERTOS_QUEUE_REGISTRY_SIZE - 3)/portNUM_PROCESSORS))     //Save space for some preallocated tasks
#define NO_OF_QUEUES_TOTAL    (NO_OF_QUEUES_PER_CORE * portNUM_PROCESSORS)
#define QUEUE_NAME_MAX_LENGTH    30

static SemaphoreHandle_t start_sem[portNUM_PROCESSORS];
static SemaphoreHandle_t done_sem = NULL;
static char *names[NO_OF_QUEUES_TOTAL];
static QueueHandle_t handles[NO_OF_QUEUES_TOTAL];

void test_queue_registry_task(void *arg)
{
    int core = xPortGetCoreID();
    int offset = core * NO_OF_QUEUES_PER_CORE;
    //Create queues and accompanying queue names
    for(int i = 0; i < NO_OF_QUEUES_PER_CORE; i++){
        handles[i + offset] = xQueueCreate(1,1);   //Create queues
        names[i + offset] = calloc(QUEUE_NAME_MAX_LENGTH, sizeof(char));
        sprintf(names[i + offset], "Queue%d%d", core, i);
    }

    xSemaphoreTake(start_sem[core], portMAX_DELAY);   //Wait for start vQueueAddToRegistry()
    for(int i = 0; i < NO_OF_QUEUES_PER_CORE; i++){
        vQueueAddToRegistry(handles[i + offset] , names[i + offset]);   //Register queues to queue registry
    }
    xSemaphoreGive(done_sem);   //Signal that vQueueAddToRegistry() has completed

    vTaskDelay(1);

    xSemaphoreTake(start_sem[core], portMAX_DELAY);   //Wait to start vQueueUnregisterQueue()
    for(int i = 0; i < NO_OF_QUEUES_PER_CORE; i++){
        vQueueDelete(handles[i + offset]);  //Internally calls vQueueUnregisterQueue
    }
    xSemaphoreGive(done_sem);   //Signal done

    vTaskDelete(NULL);  //Delete self
}

TEST_CASE("Test FreeRTOS Queue Registry", "[freertos]")
{
    //Create synchronization semaphores and tasks to test queue registry
    done_sem = xSemaphoreCreateCounting(portNUM_PROCESSORS, 0);
    for(int i = 0; i < portNUM_PROCESSORS; i++){
        start_sem[i] = xSemaphoreCreateBinary();
        xTaskCreatePinnedToCore(test_queue_registry_task, "testing task", 4096, NULL, UNITY_FREERTOS_PRIORITY+1, NULL, i);
    }

    portDISABLE_INTERRUPTS();
    for(int i = 0; i < portNUM_PROCESSORS; i++){
        xSemaphoreGive(start_sem[i]);  //Trigger start
    }
    portENABLE_INTERRUPTS();
    for(int i = 0; i < portNUM_PROCESSORS; i++){
        xSemaphoreTake(done_sem, portMAX_DELAY);    //Wait for tasks to complete vQueueAddToRegistry
    }
    for(int i = 0; i < NO_OF_QUEUES_TOTAL; i++){
        const char *addr = pcQueueGetName(handles[i]);
        TEST_ASSERT(addr == names[i])   //Check vQueueAddToRegistry was successful
    }

    portDISABLE_INTERRUPTS();
    for(int i = 0; i < portNUM_PROCESSORS; i++){
        xSemaphoreGive(start_sem[i]);  //Trigger start
    }
    portENABLE_INTERRUPTS();
    for(int i = 0; i < portNUM_PROCESSORS; i++){
        xSemaphoreTake(done_sem, portMAX_DELAY);    //Wait for tasks to complete vQueueUnregisterQueue
    }
    for(int i = 0; i <  NO_OF_QUEUES_TOTAL; i++){
        const char *addr = pcQueueGetName(handles[i]);
        TEST_ASSERT(addr == NULL)   //Check vQueueUnregisterQueue was successful
        handles[i] = NULL;
    }

    //Cleanup
    for(int i = 0; i < NO_OF_QUEUES_TOTAL; i++){
        free(names[i]);
        names[i] = NULL;
    }
    for(int i = 0; i < portNUM_PROCESSORS; i++){
        vSemaphoreDelete(start_sem[i]);
        start_sem[i] = NULL;
    }
    vSemaphoreDelete(done_sem);
    done_sem = NULL;
}
#endif      //(CONFIG_FREERTOS_QUEUE_REGISTRY_SIZE > 0)
