/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Unit tests for FreeRTOS task yielding
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "unity.h"
#include "test_utils.h"

static QueueHandle_t yield_task_sequence;
static volatile bool task_sequence_ready;
static volatile uint32_t count;
static SemaphoreHandle_t mutex;

/*
 * Test yielding for same priority tasks on the same core.
 *
 * The test performs the following actions:
 * - Creates 2 tasks with the same priority on the same core.
 * - Each task pushes its task_id on to a queue and then yields.
 * - Unity task checks the sequence of the tasks run once the yield_tasks are done.
 */
static void yield_task1(void *arg)
{
    /* Set the task sequence flag once yield_task1 runs */
    task_sequence_ready = true;

    uint32_t task_id = (uint32_t)arg;

    /* Store task_id on queue */
    TEST_ASSERT_EQUAL(pdTRUE, xQueueSend(yield_task_sequence, &task_id, 0));

    /* Yield */
    taskYIELD();

    /* Increment task count to notify unity task */
    count++;

    /* Delete itself */
    vTaskDelete(NULL);
}

static void yield_task2(void *arg)
{
    /* Wait for the other task to run for the test to begin */
    while (!task_sequence_ready) {
        taskYIELD();
    };

    uint32_t task_id = (uint32_t)arg;

    /* Store task_id on queue */
    TEST_ASSERT_EQUAL(pdTRUE, xQueueSend(yield_task_sequence, &task_id, 0));

    /* Yield */
    taskYIELD();

    /* Increment task count to notify unity task */
    count++;

    /* Delete itself */
    vTaskDelete(NULL);
}

TEST_CASE("Task yield must run the next ready task of the same priority", "[freertos][ignore]")
{
    /* Reset task count */
    count = 0;

    /* Reset task sequence flag */
    task_sequence_ready = false;

    /* Create task yielding sequence queue */
    yield_task_sequence = xQueueCreate(3, sizeof(uint32_t));
    TEST_ASSERT_NOT_EQUAL(NULL, yield_task_sequence);

    /* Create test tasks */
    xTaskCreatePinnedToCore(yield_task1, "yield_task1", 2048, (void *)1, UNITY_FREERTOS_PRIORITY - 1, NULL, UNITY_FREERTOS_CPU);
    xTaskCreatePinnedToCore(yield_task2, "yield_task2", 2048, (void *)2, UNITY_FREERTOS_PRIORITY - 1, NULL, UNITY_FREERTOS_CPU);

    /* Wait for the tasks to finish up */
    while (count != 2) {
        vTaskDelay(10);
    }

    /* Verify that the yield is successful and the next ready task is run */
    uint32_t task_id;
    TEST_ASSERT_EQUAL(pdTRUE, xQueueReceive(yield_task_sequence, &task_id, 0));
    TEST_ASSERT_EQUAL(1, task_id);
    TEST_ASSERT_EQUAL(pdTRUE, xQueueReceive(yield_task_sequence, &task_id, 0));
    TEST_ASSERT_EQUAL(2, task_id);

    /* Cleanup yield sequence queue */
    vQueueDelete(yield_task_sequence);
}

/*
 * Test yielding behavior when a task is blocked
 *
 * The test performs the following actions:
 * - Creates 2 tasks with the same priority on the same core.
 * - One task blocks on a mutex.
 * - Second task does not contest for a mutex and yields.
 * - Unity task verifies that the blocked task is not scheduled unless it is ready to run.
 */
static void test_task1(void *arg)
{
    /* Set the task sequence flag once test_task1 runs */
    task_sequence_ready = true;

    uint32_t task_id = (uint32_t)arg;

    /* Block on mutex taken by the unity task */
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(mutex, portMAX_DELAY));

    /* Store task_id on queue */
    TEST_ASSERT_EQUAL(pdTRUE, xQueueSend(yield_task_sequence, &task_id, 0));

    /* Increment task count to notify unity task */
    count++;

    /* Release mutex */
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreGive(mutex));

    /* Delete itself */
    vTaskDelete(NULL);
}

static void test_task2(void *arg)
{
    /* Wait for the other task to run for the test to begin */
    while (!task_sequence_ready) {
        taskYIELD();
    };

    uint32_t task_id = (uint32_t)arg;

    /* Store task_id on queue */
    TEST_ASSERT_EQUAL(pdTRUE, xQueueSend(yield_task_sequence, &task_id, 0));

    /* Yield */
    taskYIELD();

    /* Store task_id on queue */
    TEST_ASSERT_EQUAL(pdTRUE, xQueueSend(yield_task_sequence, &task_id, 0));

    /* Increment task count to notify unity task */
    count++;

    /* Delete itself */
    vTaskDelete(NULL);
}

TEST_CASE("Task yield must not run a blocked task", "[freertos][ignore]")
{
    /* Reset task count */
    count = 0;

    /* Reset task sequence flag */
    task_sequence_ready = false;

    /* Create mutex and acquire it */
    mutex = xSemaphoreCreateMutex();
    TEST_ASSERT_NOT_EQUAL(NULL, mutex);
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(mutex, portMAX_DELAY));

    /* Create task yielding sequence queue */
    yield_task_sequence = xQueueCreate(3, sizeof(uint32_t));
    TEST_ASSERT_NOT_EQUAL(NULL, yield_task_sequence);

    /* Create test tasks */
    xTaskCreatePinnedToCore(test_task1, "test_task1", 2048, (void *)1, UNITY_FREERTOS_PRIORITY - 1, NULL, UNITY_FREERTOS_CPU);
    xTaskCreatePinnedToCore(test_task2, "test_task2", 2048, (void *)2, UNITY_FREERTOS_PRIORITY - 1, NULL, UNITY_FREERTOS_CPU);

    /* Wait for at least one of the tasks to finish up */
    while (count == 0) {
        vTaskDelay(10);
    }

    /* Verify that the yield results in the same task running again and not the blocked task */
    uint32_t task_id;
    TEST_ASSERT_EQUAL(pdTRUE, xQueueReceive(yield_task_sequence, &task_id, 0));
    TEST_ASSERT_EQUAL(2, task_id);

    /* Verify that the task yield did not result in a context switch */
    TEST_ASSERT_EQUAL(pdTRUE, xQueueReceive(yield_task_sequence, &task_id, 0));
    TEST_ASSERT_EQUAL(2, task_id);

    /* Release mutex */
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreGive(mutex));

    /* Wait for the second task to finish up */
    while (count != 2) {
        vTaskDelay(10);
    }

    /* Verify that the second task is scheduled once it is unblocked */
    TEST_ASSERT_EQUAL(pdTRUE, xQueueReceive(yield_task_sequence, &task_id, 0));
    TEST_ASSERT_EQUAL(1, task_id);

    /* Cleanup mutex */
    vSemaphoreDelete(mutex);

    /* Cleanup yield sequence queue */
    vQueueDelete(yield_task_sequence);
}

/*
 * Test yielding behavior when the scheduler is suspended
 *
 * The test performs the following actions:
 * - Creates 2 tasks with the same priority on the same core.
 * - One task suspends the scheduler and then yields.
 * - Unity task verifies that the yield does not happen until the scheduler is resumed.
 */
static void test_critical_task1(void *arg)
{
    /* Set the task sequence flag once test_critical_task1 runs */
    task_sequence_ready = true;

    uint32_t task_id = (uint32_t)arg;

    /* Store task_id on queue */
    TEST_ASSERT_EQUAL(pdTRUE, xQueueSend(yield_task_sequence, &task_id, 0));

    /* Suspend scheduler */
    vTaskSuspendAll();

    /* Yield */
    taskYIELD();

    /* Store task_id on queue again */
    TEST_ASSERT_EQUAL(pdTRUE, xQueueSend(yield_task_sequence, &task_id, 0));

    /* Increment task count to notify unity task */
    count++;

    /* Resume scheduler */
    xTaskResumeAll();

    /* Delete itself */
    vTaskDelete(NULL);
}

static void test_critical_task2(void *arg)
{
    /* Wait for the other task to run for the test to begin */
    while (!task_sequence_ready) {
        taskYIELD();
    };

    uint32_t task_id = (uint32_t)arg;

    /* Store task_id on queue */
    TEST_ASSERT_EQUAL(pdTRUE, xQueueSend(yield_task_sequence, &task_id, 0));

    /* Increment task count to notify unity task */
    count++;

    /* Delete itself */
    vTaskDelete(NULL);
}

TEST_CASE("Task yield must not happen when scheduler is suspended", "[freertos][ignore]")
{
    /* Reset task count */
    count = 0;

    /* Reset task sequence flag */
    task_sequence_ready = false;

    /* Create task yielding sequence queue */
    yield_task_sequence = xQueueCreate(3, sizeof(uint32_t));
    TEST_ASSERT_NOT_EQUAL(NULL, yield_task_sequence);

    /* Create test tasks */
    xTaskCreatePinnedToCore(test_critical_task1, "test_critical_task1", 2048, (void *)1, UNITY_FREERTOS_PRIORITY - 1, NULL, UNITY_FREERTOS_CPU);
    xTaskCreatePinnedToCore(test_critical_task2, "test_critical_task2", 2048, (void *)2, UNITY_FREERTOS_PRIORITY - 1, NULL, UNITY_FREERTOS_CPU);

    /* Wait for at least one of the tasks to finish up */
    while (count == 0) {
        vTaskDelay(10);
    }

    /* Verify that the first task runs */
    uint32_t task_id;
    TEST_ASSERT_EQUAL(pdTRUE, xQueueReceive(yield_task_sequence, &task_id, 0));
    TEST_ASSERT_EQUAL(1, task_id);

    /* Verify that the task yield when the scheduler is suspended did not result in a context switch */
    TEST_ASSERT_EQUAL(pdTRUE, xQueueReceive(yield_task_sequence, &task_id, 0));
    TEST_ASSERT_EQUAL(1, task_id);

    /* Wait for the second task to finish up */
    while (count != 2) {
        vTaskDelay(10);
    }

    /* Verify that the second task is scheduled once the scheduler is resumed */
    TEST_ASSERT_EQUAL(pdTRUE, xQueueReceive(yield_task_sequence, &task_id, 0));
    TEST_ASSERT_EQUAL(2, task_id);

    /* Cleanup yield sequence queue */
    vQueueDelete(yield_task_sequence);
}

/*
 * Test yielding behavior when a lower priority task creates a higher priority task
 *
 * The test performs the following actions:
 * - Creates a task with a priority higher than the unity task.
 * - Unity task verifies that it yields immediately to the newly created task.
 */
static void high_prio_task(void *arg)
{
    uint32_t task_id = (uint32_t)arg;

    /* Store task_id on queue */
    TEST_ASSERT_EQUAL(pdTRUE, xQueueSend(yield_task_sequence, &task_id, 0));

    /* Increment task count to notify unity task */
    count++;

    /* Delete itself */
    vTaskDelete(NULL);
}

TEST_CASE("Lower priority task must yield immediately on creation of higher priority task", "[freertos][ignore]")
{
    /* Reset task count */
    count = 0;

    /* Create task yielding sequence queue */
    yield_task_sequence = xQueueCreate(3, sizeof(uint32_t));
    TEST_ASSERT_NOT_EQUAL(NULL, yield_task_sequence);

    /* Create test tasks */
    xTaskCreatePinnedToCore(high_prio_task, "high_prio_task", 2048, (void *)1, UNITY_FREERTOS_PRIORITY + 1, NULL, UNITY_FREERTOS_CPU);

    uint32_t unity_task_id = 2;
    /* Store task_id on queue */
    TEST_ASSERT_EQUAL(pdTRUE, xQueueSend(yield_task_sequence, &unity_task_id, 0));

    /* Wait for the newly created task to finish up */
    while (count == 0) {
        vTaskDelay(10);
    }

    /* Verify that the unity task yields as soon as a higher prio task is created */
    uint32_t task_id;
    TEST_ASSERT_EQUAL(pdTRUE, xQueueReceive(yield_task_sequence, &task_id, 0));
    TEST_ASSERT_EQUAL(1, task_id);

    /* Verify that the unity task_id is stored after the higher priority task runs */
    TEST_ASSERT_EQUAL(pdTRUE, xQueueReceive(yield_task_sequence, &task_id, 0));
    TEST_ASSERT_EQUAL(2, task_id);

    /* Cleanup yield sequence queue */
    vQueueDelete(yield_task_sequence);
}

/*
 * Test yielding behavior when a lower priority task raises the priority of another task
 *
 * The test performs the following actions:
 * - Creates a task with a priority lower than the unity task.
 * - Unity task raises the priority of the newly created task.
 * - Unity task verifies that it yields once the priority is raised.
 */
static void low_prio_task(void *arg)
{
    uint32_t task_id = (uint32_t)arg;

    /* Store task_id on queue */
   TEST_ASSERT_EQUAL(pdTRUE, xQueueSend(yield_task_sequence, &task_id, 0));

    /* Increment task count to notify unity task */
    count++;

    /* Delete itself */
    vTaskDelete(NULL);
}

TEST_CASE("Lower priority task must yield immediately when the priority of another task is raised", "[freertos][ignore]")
{
    /* Reset task count */
    count = 0;

    /* Create task yielding sequence queue */
    yield_task_sequence = xQueueCreate(3, sizeof(uint32_t));
    TEST_ASSERT_NOT_EQUAL(NULL, yield_task_sequence);

    /* Create test tasks */
    TaskHandle_t task_handle;
    xTaskCreatePinnedToCore(low_prio_task, "low_prio_task", 2048, (void *)1, UNITY_FREERTOS_PRIORITY - 1, &task_handle, UNITY_FREERTOS_CPU);

    uint32_t unity_task_id = 2;
    /* Store unity task_id on queue */
    TEST_ASSERT_EQUAL(pdTRUE, xQueueSend(yield_task_sequence, &unity_task_id, 0));

    /* Raise the priority of the lower priority task */
    vTaskPrioritySet(task_handle, UNITY_FREERTOS_PRIORITY + 1);

    /* Store unity task_id on queue again */
    TEST_ASSERT_EQUAL(pdTRUE, xQueueSend(yield_task_sequence, &unity_task_id, 0));

    /* Wait for at least the task to finish up */
    while (count == 0) {
        vTaskDelay(10);
    }

    /* Verify that the unity task does not yield to a lower priority task when it is created */
    uint32_t task_id;
    TEST_ASSERT_EQUAL(pdTRUE, xQueueReceive(yield_task_sequence, &task_id, 0));
    TEST_ASSERT_EQUAL(2, task_id);

    /* Verify that the unity task_id yielded once the priority of the lower priority task is raised */
    TEST_ASSERT_EQUAL(pdTRUE, xQueueReceive(yield_task_sequence, &task_id, 0));
    TEST_ASSERT_EQUAL(1, task_id);

    /* Verify that the unity task_id is stored again once the other task finishes up */
    TEST_ASSERT_EQUAL(pdTRUE, xQueueReceive(yield_task_sequence, &task_id, 0));
    TEST_ASSERT_EQUAL(2, task_id);

    /* Cleanup yield sequence queue */
    vQueueDelete(yield_task_sequence);
}

#if (portNUM_PROCESSORS > 1) && !(CONFIG_FREERTOS_UNICORE)
/*
 * Test yielding behavior when a task on one core forces an yield on the other core
 *
 * The test performs the following actions:
 * - Creates 2 tasks with the same priority on the core on which unity task is not running.
 * - One task spins and does not let the other task run.
 * - Force a cross-core yield from the unity task.
 * - Verify that the cross-core yield happens and the second task is scheduled to run.
 */
static void other_core_task1(void *arg)
{
    /* Set the task sequence flag once other_core_task1 runs */
    task_sequence_ready = true;

    uint32_t task_id = (uint32_t)arg;

    /* Store task_id on queue */
    TEST_ASSERT_EQUAL(pdTRUE, xQueueSend(yield_task_sequence, &task_id, 0));

    while (1) { }
}

static void other_core_task2(void *arg)
{
    /* Wait for the other task to run for the test to begin */
    while (!task_sequence_ready) {
        taskYIELD();
    };

    uint32_t task_id = (uint32_t)arg;

    /* Store task_id on queue */
    TEST_ASSERT_EQUAL(pdTRUE, xQueueSend(yield_task_sequence, &task_id, 0));

    while (1) { }
}

TEST_CASE("Task yield on other core can go through", "[freertos][ignore]")
{
    TaskHandle_t other_core_taskhandle1;
    TaskHandle_t other_core_taskhandle2;

    /* Reset task sequence flag */
    task_sequence_ready = false;

    /* Create task yielding sequence queue */
    yield_task_sequence = xQueueCreate(3, sizeof(uint32_t));
    TEST_ASSERT_NOT_EQUAL(NULL, yield_task_sequence);

    /* Create test tasks */
    xTaskCreatePinnedToCore(other_core_task1, "test_task1", 2048, (void *)1, UNITY_FREERTOS_PRIORITY - 1, &other_core_taskhandle1, !UNITY_FREERTOS_CPU);
    xTaskCreatePinnedToCore(other_core_task2, "test_task2", 2048, (void *)2, UNITY_FREERTOS_PRIORITY - 1, &other_core_taskhandle2, !UNITY_FREERTOS_CPU);

    /* Wait for everything to be setup */
    vTaskDelay(10);

    /* Verify that other_core_task1 runs */
    uint32_t task_id;
    TEST_ASSERT_EQUAL(pdTRUE, xQueueReceive(yield_task_sequence, &task_id, 0));
    TEST_ASSERT_EQUAL(1, task_id);

    /* Force an yield on the other core */
#if CONFIG_FREERTOS_SMP
    portYIELD_CORE(!UNITY_FREERTOS_CPU);
#else
    vPortYieldOtherCore(!UNITY_FREERTOS_CPU);
#endif

    /* Verify that other_core_task1 yields and other_core_task2 runs */
    TEST_ASSERT_EQUAL(pdTRUE, xQueueReceive(yield_task_sequence, &task_id, 0));
    TEST_ASSERT_EQUAL(2, task_id);

    /* Cleanup yield sequence queue */
    vQueueDelete(yield_task_sequence);

    /* Cleanup test tasks */
    vTaskDelete(other_core_taskhandle1);
    vTaskDelete(other_core_taskhandle2);
}

#if !CONFIG_FREERTOS_SMP
static volatile bool yield_triggered = false;

/*
 * Test cross-core yielding behavior when the scheduler is suspended
 *
 * The test performs the following actions:
 * - Creates 2 tasks with the same priority on the other core.
 * - One task suspends the scheduler.
 * - Unity task forces a cross-core yield.
 * - Unity task verifies that the yield does not happen until the scheduler is resumed.
 *
 * Note: This test case is not valid when FreeRTOS SMP is used as the scheduler suspension
 * is not per core but across cores and hence the test cannot be executed.
 */
static void other_core_critical_task1(void *arg)
{
    /* Set the task sequence flag once other_core_critical_task1 runs */
    task_sequence_ready = true;

    uint32_t task_id = (uint32_t)arg;

    /* Store task_id on queue */
    TEST_ASSERT_EQUAL(pdTRUE, xQueueSend(yield_task_sequence, &task_id, 0));

    /* Suspend scheduler*/
    vTaskSuspendAll();

    /* Store task_id on queue again */
    TEST_ASSERT_EQUAL(pdTRUE, xQueueSend(yield_task_sequence, &task_id, 0));

    /* Increment task count to notify unity task */
    count++;

    while (!yield_triggered) {  }

    /* Resume scheduler */
    xTaskResumeAll();

    /* Delete itself */
    vTaskDelete(NULL);
}

static void other_core_critical_task2(void *arg)
{
    /* Wait for the other task to run for the test to begin */
    while (!task_sequence_ready) {
        taskYIELD();
    };

    uint32_t task_id = (uint32_t)arg;

    /* Store task_id on queue */
    TEST_ASSERT_EQUAL(pdTRUE, xQueueSend(yield_task_sequence, &task_id, 0));

    /* Increment task count to notify unity task */
    count++;

    /* Delete itself */
    vTaskDelete(NULL);
}

TEST_CASE("Task yield on other core must not happen when scheduler is suspended", "[freertos][ignore]")
{
    /* Reset task count */
    count = 0;

    /* Reset task sequence flag */
    task_sequence_ready = false;

    /* Create task yielding sequence queue */
    yield_task_sequence = xQueueCreate(3, sizeof(uint32_t));
    TEST_ASSERT_NOT_EQUAL(NULL, yield_task_sequence);

    /* Create test tasks */
    xTaskCreatePinnedToCore(other_core_critical_task1, "other_core_critical_task1", 2048, (void *)1, UNITY_FREERTOS_PRIORITY - 1, NULL, !UNITY_FREERTOS_CPU);
    xTaskCreatePinnedToCore(other_core_critical_task2, "other_core_critical_task2", 2048, (void *)2, UNITY_FREERTOS_PRIORITY - 1, NULL, !UNITY_FREERTOS_CPU);

    /* Wait for at least one of the tasks to finish up */
    while (count == 0) {
        vTaskDelay(10);
    }

    /* Force an yield on the other core */
    vPortYieldOtherCore(!UNITY_FREERTOS_CPU);

    /* Set yield triggered flag */
    yield_triggered = true;

    /* Verify that the first task runs */
    uint32_t task_id;
    TEST_ASSERT_EQUAL(pdTRUE, xQueueReceive(yield_task_sequence, &task_id, 0));
    TEST_ASSERT_EQUAL(1, task_id);

    /* Verify that the task yield when the scheduler is suspended did not result in a context switch */
    TEST_ASSERT_EQUAL(pdTRUE, xQueueReceive(yield_task_sequence, &task_id, 0));
    TEST_ASSERT_EQUAL(1, task_id);

    /* Wait for the second task to finish up */
    while (count != 2) {
        vTaskDelay(10);
    }

    /* Verify that the second task is scheduled once the critical section is over */
    TEST_ASSERT_EQUAL(pdTRUE, xQueueReceive(yield_task_sequence, &task_id, 0));
    TEST_ASSERT_EQUAL(2, task_id);

    /* Cleanup yield sequence queue */
    vQueueDelete(yield_task_sequence);
}
#endif // !CONFIG_FREERTOS_SMP
#endif // (portNUM_PROCESSORS > 1) && !(CONFIG_FREERTOS_UNICORE)
