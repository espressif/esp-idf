/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/stream_buffer.h"
#include "freertos/message_buffer.h"
#include "freertos/event_groups.h"
#include "freertos/idf_additions.h"
#include "esp_memory_utils.h"
#include "unity.h"
#include "test_utils.h"
#include "esp_freertos_hooks.h"
#include <string.h>
#include <inttypes.h>

/*
Test ...Create...WithCaps() functions

Purpose:
    - Test that the ___Create___WithCaps() functions create objects using with correct memory capabilities

Procedure:
    - Create different FreeRTOS objects (e.g. queues, event groups etc) using the various ___Create___WithCaps()
      functions with (MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT) capabilities.
    - Call the ___GetStaticBuffers() functions to get the address of the memory of the created objects
    - Check that the memory has (MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT) capabilities by calling esp_ptr_in_dram()

Expected:
    - ___Create___WithCaps() should create objects in DRAM
*/

#define OBJECT_MEMORY_CAPS      (MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT)

static void task_with_caps(void *arg)
{
    xTaskNotifyGive((TaskHandle_t)arg);
    vTaskSuspend(NULL);
}

static void task_with_caps_self_delete(void *arg)
{
    /* Wait for the unity task to indicate that this task should delete itself */
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    /* Although it is not recommended to self-delete a task with memory caps but this
     * is done intentionally to test for memory leaks */
    vTaskDeleteWithCaps(NULL);
}

TEST_CASE("IDF additions: Task creation with memory caps and deletion from another task", "[freertos]")
{
    TaskHandle_t task_handle = NULL;
    StackType_t *puxStackBuffer;
    StaticTask_t *pxTaskBuffer;

    // Create a task with caps
    TEST_ASSERT_EQUAL(pdPASS, xTaskCreatePinnedToCoreWithCaps(task_with_caps, "task", 4096, (void *)xTaskGetCurrentTaskHandle(), UNITY_FREERTOS_PRIORITY + 1, &task_handle, UNITY_FREERTOS_CPU, OBJECT_MEMORY_CAPS));
    TEST_ASSERT_NOT_EQUAL(NULL, task_handle);
    // Get the task's memory
    TEST_ASSERT_EQUAL(pdTRUE, xTaskGetStaticBuffers(task_handle, &puxStackBuffer, &pxTaskBuffer));
    TEST_ASSERT(esp_ptr_in_dram(puxStackBuffer));
    TEST_ASSERT(esp_ptr_in_dram(pxTaskBuffer));
    // Wait for the created task to block
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    // Delete the task
    vTaskDeleteWithCaps(task_handle);
}

TEST_CASE("IDF additions: Task creation with memory caps and self deletion", "[freertos]")
{
    TaskHandle_t task_handle = NULL;
    StackType_t *puxStackBuffer;
    StaticTask_t *pxTaskBuffer;

    // Create a task with caps
    TEST_ASSERT_EQUAL(pdPASS, xTaskCreatePinnedToCoreWithCaps(task_with_caps_self_delete, "task", 4096, (void *)xTaskGetCurrentTaskHandle(), UNITY_FREERTOS_PRIORITY + 1, &task_handle, UNITY_FREERTOS_CPU, OBJECT_MEMORY_CAPS));
    TEST_ASSERT_NOT_EQUAL(NULL, task_handle);
    // Get the task's memory
    TEST_ASSERT_EQUAL(pdTRUE, xTaskGetStaticBuffers(task_handle, &puxStackBuffer, &pxTaskBuffer));
    TEST_ASSERT(esp_ptr_in_dram(puxStackBuffer));
    TEST_ASSERT(esp_ptr_in_dram(pxTaskBuffer));
    // Notify the task to delete itself
    xTaskNotifyGive(task_handle);
}

#if CONFIG_FREERTOS_TASK_CREATE_ALLOW_EXT_MEM

TEST_CASE("IDF additions: Task creation with SPIRAM memory caps and self deletion stress test", "[freertos]")
{
#define TEST_NUM_TASKS      5
#define TEST_NUM_ITERATIONS 1000
    TaskHandle_t task_handle[TEST_NUM_TASKS];
    StackType_t *puxStackBuffer;
    StaticTask_t *pxTaskBuffer;

    for (int j = 0; j < TEST_NUM_ITERATIONS; j++) {
        for (int i = 0; i < TEST_NUM_TASKS; i++) {
            // Create a task with caps
            TEST_ASSERT_EQUAL(pdPASS, xTaskCreateWithCaps(task_with_caps_self_delete, "task", 4096, NULL, UNITY_FREERTOS_PRIORITY, &task_handle[i], MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
            TEST_ASSERT_NOT_EQUAL(NULL, task_handle);
            // Get the task's memory
            TEST_ASSERT_EQUAL(pdTRUE, xTaskGetStaticBuffers(task_handle[i], &puxStackBuffer, &pxTaskBuffer));
        }

        for (int i = 0; i < TEST_NUM_TASKS; i++) {
            // Notify the task to delete itself
            xTaskNotifyGive(task_handle[i]);
        }
    }
}

#endif /* CONFIG_FREERTOS_TASK_CREATE_ALLOW_EXT_MEM */

#if ( CONFIG_FREERTOS_NUMBER_OF_CORES > 1 )

static void task_with_caps_running_on_other_core(void *arg)
{
    /* Notify the unity task that this task is running on the other core */
    xTaskNotifyGive((TaskHandle_t)arg);

    /* We make sure that this task is running on the other core */
    while (1) {
        ;
    }
}

TEST_CASE("IDF additions: Task creation with memory caps and deletion from another core", "[freertos]")
{
    TaskHandle_t task_handle = NULL;
    StackType_t *puxStackBuffer;
    StaticTask_t *pxTaskBuffer;

    // Create a task with caps on the other core
    TEST_ASSERT_EQUAL(pdPASS, xTaskCreatePinnedToCoreWithCaps(task_with_caps_running_on_other_core, "task", 4096, (void *)xTaskGetCurrentTaskHandle(), UNITY_FREERTOS_PRIORITY + 1, &task_handle, !UNITY_FREERTOS_CPU, OBJECT_MEMORY_CAPS));
    TEST_ASSERT_NOT_EQUAL(NULL, task_handle);
    // Get the task's memory
    TEST_ASSERT_EQUAL(pdTRUE, xTaskGetStaticBuffers(task_handle, &puxStackBuffer, &pxTaskBuffer));
    TEST_ASSERT(esp_ptr_in_dram(puxStackBuffer));
    TEST_ASSERT(esp_ptr_in_dram(pxTaskBuffer));
    // Wait for the created task to start running
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    // Delete the task from another core
    vTaskDeleteWithCaps(task_handle);
}

#endif // CONFIG_FREERTOS_NUMBER_OF_CORES > 1

TEST_CASE("IDF additions: Queue creation with memory caps", "[freertos]")
{
    QueueHandle_t queue_handle;
    uint8_t *queue_storage;
    StaticQueue_t *queue_obj;

    // Create queue with caps
    queue_handle = xQueueCreateWithCaps(5, sizeof(uint32_t), OBJECT_MEMORY_CAPS);
    TEST_ASSERT_NOT_EQUAL(NULL, queue_handle);
    // Get the queue's memory
    TEST_ASSERT_EQUAL(pdTRUE, xQueueGetStaticBuffers(queue_handle, &queue_storage, &queue_obj));
    TEST_ASSERT(esp_ptr_in_dram(queue_storage));
    TEST_ASSERT(esp_ptr_in_dram(queue_obj));
    // Free the queue
    vQueueDeleteWithCaps(queue_handle);
}

TEST_CASE("IDF additions: Semaphore creation with memory caps", "[freertos]")
{
    SemaphoreHandle_t sem_handle;
    StaticSemaphore_t *sem_obj;

    // Create binary semaphore with caps
    sem_handle = xSemaphoreCreateBinaryWithCaps(OBJECT_MEMORY_CAPS);
    TEST_ASSERT_NOT_EQUAL(NULL, sem_handle);
    // Get the binary semaphore's memory
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreGetStaticBuffer(sem_handle, &sem_obj));
    TEST_ASSERT(esp_ptr_in_dram(sem_obj));
    // Free the binary semaphore
    vSemaphoreDeleteWithCaps(sem_handle);

    // Create counting semaphore with caps
    sem_handle = xSemaphoreCreateCountingWithCaps(4, 0, OBJECT_MEMORY_CAPS);
    TEST_ASSERT_NOT_EQUAL(NULL, sem_handle);
    // Get the counting semaphore's memory
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreGetStaticBuffer(sem_handle, &sem_obj));
    TEST_ASSERT(esp_ptr_in_dram(sem_obj));
    // Free the counting semaphore
    vSemaphoreDeleteWithCaps(sem_handle);

    // Create mutex semaphore with caps
    sem_handle = xSemaphoreCreateMutexWithCaps(OBJECT_MEMORY_CAPS);
    TEST_ASSERT_NOT_EQUAL(NULL, sem_handle);
    // Get the mutex semaphore's memory
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreGetStaticBuffer(sem_handle, &sem_obj));
    TEST_ASSERT(esp_ptr_in_dram(sem_obj));
    // Free the mutex semaphore
    vSemaphoreDeleteWithCaps(sem_handle);

    // Create recursive mutex with caps
    sem_handle = xSemaphoreCreateRecursiveMutexWithCaps(OBJECT_MEMORY_CAPS);
    TEST_ASSERT_NOT_EQUAL(NULL, sem_handle);
    // Get the recursive mutex's memory
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreGetStaticBuffer(sem_handle, &sem_obj));
    TEST_ASSERT(esp_ptr_in_dram(sem_obj));
    // Free the recursive mutex
    vSemaphoreDeleteWithCaps(sem_handle);
}

TEST_CASE("IDF additions: Stream & message buffer creation with memory caps", "[freertos]")
{
    StreamBufferHandle_t stream_buffer_handle;
    uint8_t *stream_buffer_storage;
    StaticStreamBuffer_t *stream_buffer_obj;

    // Create stream buffer with caps
    stream_buffer_handle = xStreamBufferCreateWithCaps(16, 0, OBJECT_MEMORY_CAPS);
    TEST_ASSERT_NOT_EQUAL(NULL, stream_buffer_handle);
    // Get the stream buffer's memory
    TEST_ASSERT_EQUAL(pdTRUE, xStreamBufferGetStaticBuffers(stream_buffer_handle, &stream_buffer_storage, &stream_buffer_obj));
    TEST_ASSERT(esp_ptr_in_dram(stream_buffer_storage));
    TEST_ASSERT(esp_ptr_in_dram(stream_buffer_obj));
    // Free the stream buffer
    vStreamBufferDelete(stream_buffer_handle);

    MessageBufferHandle_t msg_buffer_handle;
    uint8_t *msg_buffer_storage;
    StaticMessageBuffer_t *msg_buffer_obj;

    // Create message buffer with caps
    msg_buffer_handle = xMessageBufferCreateWithCaps(16, OBJECT_MEMORY_CAPS);
    TEST_ASSERT_NOT_EQUAL(NULL, msg_buffer_handle);
    // Get the message buffer's memory
    TEST_ASSERT_EQUAL(pdTRUE, xMessageBufferGetStaticBuffers(msg_buffer_handle, &msg_buffer_storage, &msg_buffer_obj));
    TEST_ASSERT(esp_ptr_in_dram(msg_buffer_storage));
    TEST_ASSERT(esp_ptr_in_dram(msg_buffer_obj));
    // Free the message buffer
    vMessageBufferDelete(msg_buffer_handle);
}

TEST_CASE("IDF additions: Event group creation with memory caps", "[freertos]")
{
    EventGroupHandle_t evt_group_handle;
    StaticEventGroup_t *evt_group_obj;

    // Create event group with caps
    evt_group_handle = xEventGroupCreateWithCaps(OBJECT_MEMORY_CAPS);
    TEST_ASSERT_NOT_EQUAL(NULL, evt_group_handle);
    // Get the event group's memory
    TEST_ASSERT_EQUAL(pdTRUE, xEventGroupGetStaticBuffer(evt_group_handle, &evt_group_obj));
    TEST_ASSERT(esp_ptr_in_dram(evt_group_obj));
    // Free the event group
    vEventGroupDelete(evt_group_handle);
}

#if !CONFIG_FREERTOS_SMP
/*
Scheduler suspension behavior has changed in SMP FreeRTOS, thus these test are disabled for SMP FreeRTOS.
See IDF-5201
*/

/* ---------------------------------------------------------------------------------------------------------------------
IDF additions: IDF tick hooks during scheduler suspension

Purpose:
    - Test that the IDF tick hooks are called even with scheduler suspension

Procedure:
    Each core gets tested in the role of core X
        - Create suspend_task pinned to core X which will register a tick hook on core X and suspend scheduler on core X
        - Register tick hook on core X
        - suspend_task suspends scheduling on core X for Y milliseconds and then resumes scheduling
        - Delay suspend_task for Y milliseconds more after scheduler resumption
        - De-register the tick hook
        - Verify the tick hook callback count

Expected:
        - The tick hook is called for Y * 2 times
--------------------------------------------------------------------------------------------------------------------- */

#define TEST_DELAY_MS 200
static uint32_t tick_hook_count[portNUM_PROCESSORS];

static void IRAM_ATTR tick_hook(void)
{
    tick_hook_count[portGET_CORE_ID()] += portTICK_PERIOD_MS;
}

static void suspend_task(void *arg)
{
    TaskHandle_t main_task_hdl = (TaskHandle_t)arg;

    /* Fetch the current core ID */
    BaseType_t xCoreID = portGET_CORE_ID();

    /* Register tick hook */
    memset(tick_hook_count, 0, sizeof(tick_hook_count));
    esp_register_freertos_tick_hook_for_cpu(tick_hook, xCoreID);

    /* Suspend scheduler */
    vTaskSuspendAll();

    /* Suspend for TEST_DELAY_MS milliseconds */
    esp_rom_delay_us(TEST_DELAY_MS * 1000);

    /* Resume scheduler */
    xTaskResumeAll();

    /* Delay for a further TEST_DELAY_MS milliseconds after scheduler resumption */
    vTaskDelay(pdMS_TO_TICKS(TEST_DELAY_MS));

    /* De-register tick hook */
    esp_deregister_freertos_tick_hook_for_cpu(tick_hook, xCoreID);

    /* Verify that the tick hook callback count equals the scheduler suspension time + the delay time.
     * We add a variation of 2 ticks to account for delays encountered during test setup and teardown.
     */
    printf("Core%d tick_hook_count = %"PRIu32"\n", xCoreID, tick_hook_count[xCoreID]);
    TEST_ASSERT_INT_WITHIN(portTICK_PERIOD_MS * 2, TEST_DELAY_MS * 2, tick_hook_count[xCoreID]);

    /* Signal main task of test completion */
    xTaskNotifyGive(main_task_hdl);

    /* Cleanup */
    vTaskDelete(NULL);
}

TEST_CASE("IDF additions: IDF tick hooks during scheduler suspension", "[freertos]")
{
    /* Run test for each core */
    for (int x = 0; x < portNUM_PROCESSORS; x++) {
        xTaskCreatePinnedToCore(&suspend_task, "suspend_task", 8192, (void *)xTaskGetCurrentTaskHandle(), UNITY_FREERTOS_PRIORITY, NULL, x);

        /* Wait for test completion */
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }
}

#endif // !CONFIG_FREERTOS_SMP
