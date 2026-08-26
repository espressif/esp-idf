/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/idf_additions.h"
#include "unity.h"

#define QUEUE_LEN       4

static void allocate_resources(int num_queues, int queue_len, QueueHandle_t *queue_list_ret, QueueSetHandle_t *queue_set_ret)
{
    // Create queue set
    *queue_set_ret = xQueueCreateSet(num_queues * queue_len);
    TEST_ASSERT_NOT_EQUAL(NULL, *queue_set_ret);
    // Create queues and add them to the queue set
    for (int i = 0; i < num_queues; i++) {
        queue_list_ret[i] = xQueueCreate(queue_len, sizeof(BaseType_t));
        TEST_ASSERT_NOT_EQUAL(NULL, queue_list_ret[i]);
        TEST_ASSERT_EQUAL(pdPASS, xQueueAddToSet(queue_list_ret[i], *queue_set_ret));
    }
}

static void free_resources(int num_queues, QueueHandle_t *queue_list, QueueSetHandle_t queue_set)
{
    // Remove queues form queue set and delete the queues
    for (int i = 0; i < num_queues; i++) {
        TEST_ASSERT_EQUAL(pdPASS, xQueueRemoveFromSet(queue_list[i], queue_set));
        vQueueDelete(queue_list[i]);
    }
    vQueueDelete(queue_set);
}

/*
Test queue sets basic

Purpose:
    - Test that queue set works as expected

Procedure:
    - Create NUM_QUEUES queues and add them to the same queue set
    - Fill each queue sequentially with QUEUE_LEN items

Expected:
    - Each call to xQueueSend() should generate a member in the queue set
    - The order of the members should match the order in which xQueueSend() was called
    - The item sent by the xQueueSend() is correct when read
*/

#define NUM_QUEUES      5

TEST_CASE("Test Queue sets", "[freertos]")
{
    // Create queues and queue set
    QueueHandle_t queues[NUM_QUEUES];
    QueueSetHandle_t queue_set;
    allocate_resources(NUM_QUEUES, QUEUE_LEN, queues, &queue_set);

    // Fill each queue sequentially with QUEUE_LEN items
    for (int i = 0; i < NUM_QUEUES; i++) {
        for (int j = 0; j < QUEUE_LEN; j++) {
            BaseType_t item = j;
            TEST_ASSERT_EQUAL(pdTRUE, xQueueSend(queues[i], &item, 0));
        }
    }

    for (int i = 0; i < NUM_QUEUES; i++) {
        for (int j = 0; j < QUEUE_LEN; j++) {
            // Check the queue set member
            QueueSetMemberHandle_t member = xQueueSelectFromSet(queue_set, 0);
            TEST_ASSERT_EQUAL(queues[i], member);
            // Check the queue's items
            BaseType_t item;
            TEST_ASSERT_EQUAL(pdTRUE, xQueueReceive(member, &item, 0));
            TEST_ASSERT_EQUAL(j, item);
        }
    }
    // Check that there are no more members
    TEST_ASSERT_EQUAL(NULL, xQueueSelectFromSet(queue_set, 0));

    // Cleanup queues and queue set
    free_resources(NUM_QUEUES, queues, queue_set);
}

#ifndef CONFIG_FREERTOS_UNICORE
/*
Test queue set SMP thread safety

Purpose:
    - Test that queue set works when being used from different cores simultaneously

Procedure:
    - Create a queue for each core and add them to the same queue set
    - Create a task on each core to send QUEUE_LEN items to their assigned queue
    - Synchronize the tasks so that the start sending at the same time

Expected:
    - Each call to xQueueSend() should generate a member in the queue set
    - The item sent by the xQueueSend() is correct when read
*/

static volatile bool start_other_cores;
static SemaphoreHandle_t done_sem = NULL;

static void send_func(void *arg)
{
    QueueHandle_t queue = (QueueHandle_t)arg;
    BaseType_t core_id = xPortGetCoreID();
    if (core_id == 0) {
        // We are core 0. Trigger the other cores to start
        start_other_cores = true;
    } else {
        // Wait to be started by main core
        while (!start_other_cores) {
            ;
        }
    }

    // Fill the queue assigned to the current core
    for (int i = 0; i < QUEUE_LEN; i++) {
        TEST_ASSERT_EQUAL(pdTRUE, xQueueSend(queue, &core_id, 0));
    }

    if (core_id != 0) {
        // Indicate completion to core 0 and self delete
        xSemaphoreGive(done_sem);
        vTaskDelete(NULL);
    }
}

TEST_CASE("Test queue sets multi-core", "[freertos]")
{
    // Create done semaphore
    done_sem = xSemaphoreCreateCounting(configNUM_CORES - 1, 0);
    TEST_ASSERT_NOT_EQUAL(NULL, done_sem);

    // Create queues and queue set
    QueueHandle_t queues[configNUM_CORES];
    QueueSetHandle_t queue_set;
    allocate_resources(configNUM_CORES, QUEUE_LEN, queues, &queue_set);

    // Create tasks of the same priority for all cores except for core 0
    for (int i = 1; i < configNUM_CORES; i++) {
        TEST_ASSERT_EQUAL(pdTRUE, xTaskCreatePinnedToCore(send_func, "send", 2048, (void *)queues[i], CONFIG_UNITY_FREERTOS_PRIORITY, NULL, i));
    }

    // Core 0 calls send_func as well triggering the simultaneous sends from all cores
    send_func((void *)queues[0]);

    // Wait for all other cores to be done
    for (int i = 1; i < configNUM_CORES; i++) {
        xSemaphoreTake(done_sem, portMAX_DELAY);
    }

    // Read queues from the queue set, then read an item from the queue
    uint32_t queues_check_count[configNUM_CORES] = {0};
    QueueSetMemberHandle_t member = xQueueSelectFromSet(queue_set, 0);
    while (member != NULL) {
        // Read the core ID from the queue, check that core ID is sane
        BaseType_t core_id;
        TEST_ASSERT_EQUAL(pdTRUE, xQueueReceive(member, &core_id, 0));
        TEST_ASSERT_LESS_THAN(configNUM_CORES, core_id);
        queues_check_count[core_id]++;

        // Get next member
        member = xQueueSelectFromSet(queue_set, 0);
    }
    // Check that all items from all queues have been read
    for (int i = 0; i < configNUM_CORES; i++) {
        TEST_ASSERT_EQUAL(QUEUE_LEN, queues_check_count[i]);
    }

    // Cleanup queues and queue set
    free_resources(configNUM_CORES, queues, queue_set);
    // Cleanup done sem
    vSemaphoreDelete(done_sem);
    done_sem = NULL;
}
#endif // CONFIG_FREERTOS_UNICORE
