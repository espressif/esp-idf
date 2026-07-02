/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/stream_buffer.h"
#include "freertos/message_buffer.h"
#include "freertos/event_groups.h"
#include "freertos/idf_additions.h"
#include "freertos/freertos_debug.h"
#include "esp_memory_utils.h"
#include "unity.h"
#include "test_utils.h"
#include "esp_freertos_hooks.h"
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

/*
Test ESP-IDF FreeRTOS API additions from idf_additions.h:
    - ...Create...WithCaps() / ...DeleteWithCaps() memory capability helpers
    - Task utility helpers (xTaskGetCoreID, xTaskGetStackStart, per-core handle queries, etc.)

Run only these cases from the Unity menu with the [idf_additions] tag filter.
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

TEST_CASE("IDF additions: Task creation with memory caps and deletion from another task", "[freertos][idf_additions]")
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

TEST_CASE("IDF additions: Task creation with memory caps and self deletion", "[freertos][idf_additions]")
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

TEST_CASE("IDF additions: Task creation with SPIRAM memory caps and self deletion stress test", "[freertos][idf_additions]")
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
            // Get the task's memory
            TEST_ASSERT_EQUAL(pdTRUE, xTaskGetStaticBuffers(task_handle[i], &puxStackBuffer, &pxTaskBuffer));
        }

        for (int i = 0; i < TEST_NUM_TASKS; i++) {
            // Notify the task to delete itself
            xTaskNotifyGive(task_handle[i]);
        }
        // Let the scheduler run the self-delete tasks.
        vTaskDelay(1);
    }
    // Allow the last batch to be freed by the cleanup task in vTaskDeleteWithCaps().
    vTaskDelay(50);
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

TEST_CASE("IDF additions: Task creation with memory caps and deletion from another core", "[freertos][idf_additions]")
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

TEST_CASE("IDF additions: Queue creation with memory caps", "[freertos][idf_additions]")
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

TEST_CASE("IDF additions: Semaphore creation with memory caps", "[freertos][idf_additions]")
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

TEST_CASE("IDF additions: Stream & message buffer creation with memory caps", "[freertos][idf_additions]")
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
    vStreamBufferDeleteWithCaps(stream_buffer_handle);

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
    vMessageBufferDeleteWithCaps(msg_buffer_handle);
}

TEST_CASE("IDF additions: Event group creation with memory caps", "[freertos][idf_additions]")
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
    vEventGroupDeleteWithCaps(evt_group_handle);
}

/* ---------------------------------------------------------------------------------------------------------------------
 * Task utility API tests (xTaskGetCoreID, xTaskGetStackStart, per-core handle helpers, etc.)
 * --------------------------------------------------------------------------------------------------------------------- */

static void stack_test_task(void *arg)
{
    (void)arg;
    vTaskSuspend(NULL);
}

typedef struct {
    TaskHandle_t parent;
    BaseType_t expected_core;
    BaseType_t observed_core;
    TaskHandle_t self_handle;
} core_id_query_ctx_t;

static void pinned_core_id_task(void *arg)
{
    core_id_query_ctx_t *ctx = (core_id_query_ctx_t *)arg;

    ctx->self_handle = xTaskGetCurrentTaskHandle();
    ctx->observed_core = xPortGetCoreID();
    TEST_ASSERT_EQUAL(ctx->expected_core, ctx->observed_core);
    TEST_ASSERT_EQUAL(ctx->expected_core, xTaskGetCoreID(NULL));
    TEST_ASSERT_EQUAL(ctx->expected_core, xTaskGetCoreID(ctx->self_handle));

    xTaskNotifyGive(ctx->parent);
    vTaskSuspend(NULL);
}

TEST_CASE("IDF additions: xTaskGetCoreID for pinned tasks", "[freertos][idf_additions]")
{
    const TaskHandle_t parent = xTaskGetCurrentTaskHandle();
    core_id_query_ctx_t ctx[CONFIG_FREERTOS_NUMBER_OF_CORES];

    for (int core = 0; core < CONFIG_FREERTOS_NUMBER_OF_CORES; core++) {
        ctx[core].parent = parent;
        ctx[core].expected_core = core;
        ctx[core].observed_core = -1;
        ctx[core].self_handle = NULL;

        TEST_ASSERT_EQUAL(pdPASS, xTaskCreatePinnedToCore(pinned_core_id_task,
                                                          "core_id",
                                                          2048,
                                                          &ctx[core],
                                                          UNITY_FREERTOS_PRIORITY + 1,
                                                          NULL,
                                                          core));
    }

    for (int core = 0; core < CONFIG_FREERTOS_NUMBER_OF_CORES; core++) {
        TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
        TEST_ASSERT_EQUAL(core, ctx[core].observed_core);
        TEST_ASSERT_EQUAL(core, xTaskGetCoreID(ctx[core].self_handle));
    }

    for (int core = 0; core < CONFIG_FREERTOS_NUMBER_OF_CORES; core++) {
        vTaskDelete(ctx[core].self_handle);
    }
    vTaskDelay(1);
}

#if (CONFIG_FREERTOS_NUMBER_OF_CORES > 1) && !CONFIG_FREERTOS_SMP

static SemaphoreHandle_t s_unpinned_done_sem;
static TaskHandle_t s_unpinned_task_handle;

static void unpinned_core_id_task(void *arg)
{
    (void)arg;
    s_unpinned_task_handle = xTaskGetCurrentTaskHandle();
    xSemaphoreGive(s_unpinned_done_sem);
    vTaskSuspend(NULL);
}

TEST_CASE("IDF additions: xTaskGetCoreID returns tskNO_AFFINITY for unpinned task", "[freertos][idf_additions]")
{
    s_unpinned_done_sem = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(s_unpinned_done_sem);
    s_unpinned_task_handle = NULL;

    TEST_ASSERT_EQUAL(pdPASS, xTaskCreatePinnedToCore(unpinned_core_id_task,
                                                      "unpinned",
                                                      2048,
                                                      NULL,
                                                      UNITY_FREERTOS_PRIORITY - 1,
                                                      NULL,
                                                      tskNO_AFFINITY));
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s_unpinned_done_sem, pdMS_TO_TICKS(1000)));
    TEST_ASSERT_NOT_NULL(s_unpinned_task_handle);
    TEST_ASSERT_EQUAL(tskNO_AFFINITY, xTaskGetCoreID(s_unpinned_task_handle));

    vTaskDelete(s_unpinned_task_handle);
    vSemaphoreDelete(s_unpinned_done_sem);
    vTaskDelay(1);
}

#endif /* CONFIG_FREERTOS_NUMBER_OF_CORES > 1 && !CONFIG_FREERTOS_SMP */

#if CONFIG_FREERTOS_UNICORE

TEST_CASE("IDF additions: xTaskGetCoreID returns 0 on unicore", "[freertos][idf_additions]")
{
    TEST_ASSERT_EQUAL(0, xTaskGetCoreID(NULL));
    TEST_ASSERT_EQUAL(0, xTaskGetCoreID(xTaskGetCurrentTaskHandle()));
}

#endif /* CONFIG_FREERTOS_UNICORE */

TEST_CASE("IDF additions: xTaskGetStackStart", "[freertos][idf_additions]")
{
    StackType_t *current_stack_via_null = xTaskGetStackStart(NULL);
    StackType_t *current_stack_via_handle = xTaskGetStackStart(xTaskGetCurrentTaskHandle());

    TEST_ASSERT_NOT_NULL(current_stack_via_null);
    TEST_ASSERT_EQUAL_PTR(current_stack_via_null, current_stack_via_handle);

    TaskHandle_t caps_task = NULL;
    StackType_t *caps_stack_buffer = NULL;
    StaticTask_t *caps_task_buffer = NULL;

    TEST_ASSERT_EQUAL(pdPASS, xTaskCreatePinnedToCoreWithCaps(stack_test_task,
                                                              "stack_caps",
                                                              4096,
                                                              NULL,
                                                              UNITY_FREERTOS_PRIORITY + 1,
                                                              &caps_task,
                                                              UNITY_FREERTOS_CPU,
                                                              OBJECT_MEMORY_CAPS));
    TEST_ASSERT_NOT_NULL(caps_task);
    TEST_ASSERT_EQUAL(pdTRUE, xTaskGetStaticBuffers(caps_task, &caps_stack_buffer, &caps_task_buffer));
    TEST_ASSERT_EQUAL_PTR(caps_stack_buffer, xTaskGetStackStart(caps_task));

    vTaskDeleteWithCaps(caps_task);
    vTaskDelay(10);

#define STATIC_STACK_BYTES 4096
    static StackType_t static_stack[STATIC_STACK_BYTES / sizeof(StackType_t)];
    static StaticTask_t static_task_buffer;

    memset(&static_task_buffer, 0, sizeof(static_task_buffer));

    TaskHandle_t static_task = xTaskCreateStaticPinnedToCore(stack_test_task,
                                                             "stack_static",
                                                             STATIC_STACK_BYTES,
                                                             NULL,
                                                             UNITY_FREERTOS_PRIORITY + 1,
                                                             static_stack,
                                                             &static_task_buffer,
                                                             UNITY_FREERTOS_CPU);
    TEST_ASSERT_NOT_NULL(static_task);
    TEST_ASSERT_EQUAL_PTR(static_stack, xTaskGetStackStart(static_task));

    vTaskDelete(static_task);
    vTaskDelay(1);
}

#if !CONFIG_FREERTOS_SMP

TEST_CASE("IDF additions: xTaskGetIdleTaskHandleForCore", "[freertos][idf_additions]")
{
    for (int core = 0; core < CONFIG_FREERTOS_NUMBER_OF_CORES; core++) {
        TaskHandle_t idle_for_core = xTaskGetIdleTaskHandleForCore(core);
        char expected_name[configMAX_TASK_NAME_LEN];

        TEST_ASSERT_NOT_NULL(idle_for_core);
#if CONFIG_FREERTOS_NUMBER_OF_CORES > 1
        snprintf(expected_name, sizeof(expected_name), "IDLE%d", core);
#else
        snprintf(expected_name, sizeof(expected_name), "IDLE");
#endif
        TEST_ASSERT_EQUAL_STRING(expected_name, pcTaskGetName(idle_for_core));
    }

#if CONFIG_FREERTOS_UNICORE
    TEST_ASSERT_EQUAL(xTaskGetIdleTaskHandle(), xTaskGetIdleTaskHandleForCore(0));
#endif
}

typedef struct {
    TaskHandle_t parent;
    BaseType_t core;
    TaskHandle_t self_handle;
    TaskHandle_t queried_handle;
    void * queried_tcb;
} per_core_handle_ctx_t;

static void running_on_core_task(void *arg)
{
    per_core_handle_ctx_t *ctx = (per_core_handle_ctx_t *)arg;

    ctx->self_handle = xTaskGetCurrentTaskHandle();
    ctx->queried_handle = xTaskGetCurrentTaskHandleForCore(ctx->core);
    ctx->queried_tcb = pvTaskGetCurrentTCBForCore(ctx->core);

    xTaskNotifyGive(ctx->parent);
    vTaskSuspend(NULL);
}

TEST_CASE("IDF additions: xTaskGetCurrentTaskHandleForCore and pvTaskGetCurrentTCBForCore", "[freertos][idf_additions]")
{
    const TaskHandle_t parent = xTaskGetCurrentTaskHandle();

    for (int core = 0; core < CONFIG_FREERTOS_NUMBER_OF_CORES; core++) {
        per_core_handle_ctx_t ctx = {
            .parent = parent,
            .core = core,
            .self_handle = NULL,
            .queried_handle = NULL,
            .queried_tcb = NULL,
        };

        while (ulTaskNotifyTake(pdFALSE, 0) != 0) {
            ;
        }

        TEST_ASSERT_EQUAL(pdPASS, xTaskCreatePinnedToCore(running_on_core_task,
                                                          "on_core",
                                                          2048,
                                                          &ctx,
                                                          UNITY_FREERTOS_PRIORITY + 1,
                                                          NULL,
                                                          core));

        TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));

        TEST_ASSERT_EQUAL_PTR(ctx.self_handle, ctx.queried_handle);
        TEST_ASSERT_EQUAL_PTR((void *)ctx.self_handle, ctx.queried_tcb);

        vTaskDelete(ctx.self_handle);
        vTaskDelay(1);
    }
}

#endif /* !CONFIG_FREERTOS_SMP */

#if CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS && !CONFIG_FREERTOS_SMP

TEST_CASE("IDF additions: ulTaskGetIdleRunTimeCounterForCore", "[freertos][idf_additions]")
{
    for (int core = 0; core < CONFIG_FREERTOS_NUMBER_OF_CORES; core++) {
        configRUN_TIME_COUNTER_TYPE idle_counter = ulTaskGetIdleRunTimeCounterForCore(core);
        configRUN_TIME_COUNTER_TYPE idle_percent = ulTaskGetIdleRunTimePercentForCore(core);

        TEST_ASSERT_GREATER_OR_EQUAL(0, idle_counter);
        TEST_ASSERT_LESS_OR_EQUAL(100, idle_percent);
    }
}

#endif /* CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS && !CONFIG_FREERTOS_SMP */

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
static volatile uint32_t tick_hook_count[portNUM_PROCESSORS];

static void IRAM_ATTR tick_hook(void)
{
    tick_hook_count[portGET_CORE_ID()] += portTICK_PERIOD_MS;
}

static void suspend_task(void *arg)
{
    TaskHandle_t main_task_hdl = (TaskHandle_t)arg;

    /* Fetch the current core ID */
    BaseType_t xCoreID = portGET_CORE_ID();

    /* Warm up the cache by running the scheduler suspension/resumption once.
     * This reduces the execution time variance caused by cache misses during
     * the actual test on targets like the esp32p4.
     */
    vTaskSuspendAll();
    xTaskResumeAll();
    vTaskDelay(pdMS_TO_TICKS(10));

    /* Register tick hook */
    tick_hook_count[xCoreID] = 0;
    esp_register_freertos_tick_hook_for_cpu(tick_hook, xCoreID);

    /* Read the tick hook count before suspending */
    uint32_t initial_count = tick_hook_count[xCoreID];

    /* Suspend scheduler */
    vTaskSuspendAll();

    /* Suspend for TEST_DELAY_MS milliseconds */
    esp_rom_delay_us(TEST_DELAY_MS * 1000);

    /* Resume scheduler */
    xTaskResumeAll();

    /* Delay for a further TEST_DELAY_MS milliseconds after scheduler resumption */
    vTaskDelay(pdMS_TO_TICKS(TEST_DELAY_MS));

    /* Read the final tick hook count */
    uint32_t final_count = tick_hook_count[xCoreID];

    /* De-register tick hook */
    esp_deregister_freertos_tick_hook_for_cpu(tick_hook, xCoreID);

    /* Verify that the tick hook callback count equals the scheduler suspension time + the delay time.
     * We add a variation of 2 ticks to account for delays encountered during test setup and teardown.
     */
    printf("Core%d initial_count = %"PRIu32"\n", xCoreID, initial_count);
    printf("Core%d final_count = %"PRIu32"\n", xCoreID, final_count);
    TEST_ASSERT_INT_WITHIN(portTICK_PERIOD_MS * 2, TEST_DELAY_MS * 2, final_count - initial_count);

    /* Signal main task of test completion */
    xTaskNotifyGive(main_task_hdl);

    vTaskSuspend(NULL);
}

TEST_CASE("IDF additions: IDF tick hooks during scheduler suspension", "[freertos]")
{
    /* Run test for each core */
    TaskHandle_t suspend_task_handle[portNUM_PROCESSORS];
    for (int x = 0; x < portNUM_PROCESSORS; x++) {
        xTaskCreatePinnedToCore(&suspend_task, "suspend_task", 8192, (void *)xTaskGetCurrentTaskHandle(), UNITY_FREERTOS_PRIORITY, &suspend_task_handle[x], x);

        /* Wait for test completion */
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        /* Cleanup */
        vTaskSuspend(suspend_task_handle[x]);
        vTaskDelay(10);
        vTaskDelete(suspend_task_handle[x]);
    }
}

#endif // !CONFIG_FREERTOS_SMP
