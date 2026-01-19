/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gptimer.h"
#include "esp_rom_sys.h"
#include "unity.h"
#include "test_utils.h"

/*
Scheduler suspension behavior differs significantly in SMP FreeRTOS, thus none of these tests apply to SMP FreeRTOS
*/
#if !CONFIG_FREERTOS_SMP

/*
GP timer is used to trigger an interrupt. Test cases will register an interrupt callback called from the timer's
interrupt callback. The functions below simply the interrupt registration/trigger/deregistration process.
*/

#if SOC_GPTIMER_SUPPORTED

static gptimer_handle_t gptimer = NULL;
static bool (*registered_intr_callback)(void *) = NULL;

static bool on_timer_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    bool yield;
    if (registered_intr_callback) {
        yield = registered_intr_callback(user_ctx);
    } else {
        yield = false;
    }
    return yield;
}

static void register_intr_cb(bool (*callback)(void *), void *callback_arg)
{
    gptimer_handle_t gptimer_temp;

    // Initialize a GP timer used to trigger an interrupt
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1MHz, 1 tick=1us
    };
    TEST_ESP_OK(gptimer_new_timer(&timer_config, &gptimer_temp));

    // Configure an alarm (of 1ms) and callback for the timer
    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = 1000, // alarm period 1ms
        .flags.auto_reload_on_alarm = true,
    };
    gptimer_event_callbacks_t cbs = {
        .on_alarm = on_timer_alarm_cb,
    };
    TEST_ESP_OK(gptimer_register_event_callbacks(gptimer_temp, &cbs, callback_arg));
    TEST_ESP_OK(gptimer_enable(gptimer_temp));
    TEST_ESP_OK(gptimer_set_alarm_action(gptimer_temp, &alarm_config));

    gptimer = gptimer_temp;
    registered_intr_callback = callback;
}

static void trigger_intr_cb(void)
{
    // Interrupt should be triggered in 1ms
    TEST_ESP_OK(gptimer_start(gptimer));
}

static void deregister_intr_cb(void)
{
    gptimer_handle_t gptimer_temp = gptimer;
    gptimer = NULL;
    registered_intr_callback = NULL;

    TEST_ESP_OK(gptimer_stop(gptimer_temp));
    TEST_ESP_OK(gptimer_disable(gptimer_temp));
    TEST_ESP_OK(gptimer_del_timer(gptimer_temp));
}

#endif //SOC_GPTIMER_SUPPORTED

/* ---------------------------------------------------------------------------------------------------------------------
Test vTaskSuspendAll() and xTaskResumeAll() basic

Purpose:
    - Test that vTaskSuspendAll() will suspends the scheduler for the calling core
    - Test that xTaskResumeAll() will resumes scheduling for the calling core

Procedure:
    - Call vTaskSuspendAll() to suspend the scheduler
    - Call xTaskResumeAll() to resume the scheduler

Expected:
    - xTaskGetSchedulerState() should return the correct state
--------------------------------------------------------------------------------------------------------------------- */

TEST_CASE("Test vTaskSuspendAll and xTaskResumeAll basic", "[freertos]")
{
    // Check scheduler is running on the current core
    TEST_ASSERT_EQUAL(taskSCHEDULER_RUNNING, xTaskGetSchedulerState());
    vTaskSuspendAll();
    TEST_ASSERT_EQUAL(taskSCHEDULER_SUSPENDED, xTaskGetSchedulerState());
    xTaskResumeAll();
    TEST_ASSERT_EQUAL(taskSCHEDULER_RUNNING, xTaskGetSchedulerState());
}

/* ---------------------------------------------------------------------------------------------------------------------
Test vTaskSuspendAll() and xTaskResumeAll() multicore

Only runs on !CONFIG_FREERTOS_UNICORE

Purpose:
    - Test that vTaskSuspendAll() will only suspends scheduling only for the calling core
    - Test that xTaskResumeAll() will only resume scheduling for the calling core

Procedure:
    Each core gets tested in the role of core A
    - Create a taskA pinned to one core (e.g., core A) that will disable the scheduler
    - Created a "taskB" to another core (e.g., core B) that will not disable the scheduler
    - taskA calls vTaskSuspendAll() to suspend the scheduler on core A
    - taskA calls xTaskResumeAll() to resume the scheduler on core A

Expected:
    - vTaskSuspendAll() should only disable the scheduler for the suspended core A
    - xTaskResumeAll() should resume the scheduler for the suspended core A
    - Scheduler on core B should remain enabled
--------------------------------------------------------------------------------------------------------------------- */

#if !CONFIG_FREERTOS_UNICORE
#define TEST_BASIC_BUSY_DELAY_US    10000

static volatile int taskA_sync;
static volatile int taskB_sync;

static void test_multicore_taskA(void *arg)
{
    // Wait to be started
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    // Check scheduler on core A is enabled
    TEST_ASSERT_EQUAL(taskSCHEDULER_RUNNING, xTaskGetSchedulerState());
    taskA_sync++;
    while (taskB_sync != 1) {
        ;   // Wait for task B to complete its check
    }

    // Suspend the scheduler on core A
    vTaskSuspendAll();
    // Check scheduler is suspended on core A
    TEST_ASSERT_EQUAL(taskSCHEDULER_SUSPENDED, xTaskGetSchedulerState());
    taskA_sync++;
    while (taskB_sync != 2) {
        ;   // Wait for task B to complete its check
    }

    // Busy spin for a while to simulate work done while scheduler is suspended
    esp_rom_delay_us(TEST_BASIC_BUSY_DELAY_US);

    // Check scheduler is still suspended on core A
    TEST_ASSERT_EQUAL(taskSCHEDULER_SUSPENDED, xTaskGetSchedulerState());
    taskA_sync++;
    while (taskB_sync != 3) {
        ;   // Wait for task B to complete its check
    }

    // Resume the scheduler on core A
    xTaskResumeAll();
    // Check that scheduler has resumed resumed on core A
    TEST_ASSERT_EQUAL(taskSCHEDULER_RUNNING, xTaskGetSchedulerState());
    taskA_sync++;
    while (taskB_sync != 4) {
        ;   // Wait for task B to complete its check
    }

    // Indicate done and wait to be deleted
    SemaphoreHandle_t done_sem = (SemaphoreHandle_t)arg;
    xSemaphoreGive(done_sem);
    vTaskSuspend(NULL);
}

static void test_multicore_taskB(void *arg)
{
    // Wait to be started
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    for (int i = 1; i <= 4; i++) {
        // Wait for suspended trigger from task A
        while (taskA_sync != i) {
            ;
        }
        // Check that scheduler is still running on core B
        TEST_ASSERT_EQUAL(taskSCHEDULER_RUNNING, xTaskGetSchedulerState());
        taskB_sync++;
    }

    // Indicate done and wait to be deleted
    SemaphoreHandle_t done_sem = (SemaphoreHandle_t)arg;
    xSemaphoreGive(done_sem);
    vTaskSuspend(NULL);
}

TEST_CASE("Test vTaskSuspendAll() and xTaskResumeAll() multicore", "[freertos]")
{
    SemaphoreHandle_t done_sem = xSemaphoreCreateCounting(CONFIG_FREERTOS_NUMBER_OF_CORES, 0);
    TEST_ASSERT_NOT_EQUAL(NULL, done_sem);

    for (int i = 0; i < CONFIG_FREERTOS_NUMBER_OF_CORES; i++) {
        // Create tasks on core A and core B
        TaskHandle_t taskA_hdl;
        TaskHandle_t taskB_hdl;
        TEST_ASSERT_EQUAL(pdTRUE, xTaskCreatePinnedToCore(test_multicore_taskA, "taskA", 2048, (void *)done_sem, UNITY_FREERTOS_PRIORITY - 1, &taskA_hdl, i));
        TEST_ASSERT_EQUAL(pdTRUE, xTaskCreatePinnedToCore(test_multicore_taskB, "taskB", 2048, (void *)done_sem, UNITY_FREERTOS_PRIORITY - 1, &taskB_hdl, !i));
        // Start the tasks and wait for their completion
        taskA_sync = 0;
        taskB_sync = 0;
        xTaskNotifyGive(taskA_hdl);
        xTaskNotifyGive(taskB_hdl);
        for (int j = 0; j < 2; j++) {
            xSemaphoreTake(done_sem, portMAX_DELAY);
        }

        // Suspend the test tasks in case they haven't suspended themselves yet
        vTaskSuspend(taskA_hdl);
        vTaskSuspend(taskB_hdl);
        vTaskDelay(10);

        // Cleanup the tasks
        vTaskDelete(taskA_hdl);
        vTaskDelete(taskB_hdl);
    }

    vSemaphoreDelete(done_sem);
    // Add a short delay to allow the idle task to free any remaining task memory
    vTaskDelay(10);
}
#endif  // !CONFIG_FREERTOS_UNICORE

/* ---------------------------------------------------------------------------------------------------------------------
Test vTaskSuspendAll allows scheduling on other cores

Only runs on !CONFIG_FREERTOS_UNICORE

Purpose:
    - Test that disabling a scheduler on one core (e.g., core B) does not disable scheduling on the other core (e.g., core A)
    - While the scheduler on core B is disabled, test that...
        - A task on Core A can be unblocked by another task also on core A
        - A task on Core A can be unblocked by an interrupt on core A

Procedure:
    Each core gets tested in the role of core A
        - Create task B1 pinned to core B that will suspend scheduling on core B
        - Create task A2 pinned to core A that will test unblocking on core A
        - Create task A1 pinned to core A that will unblock task A2
        - Register an interrupt on core A that will unblock task A2
        - Have A2 block
        - Have B1 disable scheduling on core B. A1 checks that scheduling is still enabled on core A
        - Have A1 unblock A2
        - Have the core A ISR unblock A2
        - Cleanup the tasks

Expected:
    When B1 disables scheduling on core B...
        - Scheduling on core A should still be enabled
        - A2 should be unblocked by A1 and run without issue
        - A2 should be unblocked by core A ISR and run without issue
--------------------------------------------------------------------------------------------------------------------- */

#if !CONFIG_FREERTOS_UNICORE

#if SOC_GPTIMER_SUPPORTED

static volatile int test_unblk_sync;
static SemaphoreHandle_t test_unblk_done_sem;

static bool test_unblk_coreA_isr(void *arg)
{
    TaskHandle_t a2_task_hdl = (TaskHandle_t)arg;
    BaseType_t task_woken = pdFALSE;
    // Unblock task b2
    vTaskNotifyGiveFromISR(a2_task_hdl, &task_woken);
    return (task_woken == pdTRUE);
}

static void test_unblk_a2_task(void *arg)
{
    volatile int *a2_task_run_count = (volatile int *)arg;
    // Wait to be unblocked by A1
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    (*a2_task_run_count)++;
    // Wait to be unblocked by Core A ISR
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    (*a2_task_run_count)++;
    // Wait to be deleted
    vTaskSuspend(NULL);
}

static void test_unblk_a1_task(void *arg)
{
    volatile int a2_task_run_count = 0;

    // Create task A2 to block on this core (i.e., core A)
    TaskHandle_t a2_task_hdl;
    TEST_ASSERT_EQUAL(pdTRUE, xTaskCreatePinnedToCore(test_unblk_a2_task, "A2", 8192, (void *)&a2_task_run_count, UNITY_FREERTOS_PRIORITY + 2, &a2_task_hdl, xPortGetCoreID()));
    // Install an interrupt on core A
    register_intr_cb(test_unblk_coreA_isr, (void *)a2_task_hdl);

    // Wait to be started by the main task
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    // Start B1
    xTaskNotifyGive((TaskHandle_t)arg);
    while (test_unblk_sync != 1) {
        ; // Wait for confirmation from B1 that scheduler has been suspended on Core B
    }
    // Verify that the scheduler is still enabled on core A
    TEST_ASSERT_EQUAL(taskSCHEDULER_RUNNING, xTaskGetSchedulerState());

    // Unblock A2, it should preempt immediately due to its higher priority
    xTaskNotifyGive(a2_task_hdl);
    // Verify that task A2 has run
    TEST_ASSERT_EQUAL(1, a2_task_run_count);

    // Trigger an ISR on this core A to unblock task A2. A2 should preempt immediately due to its higher priority
    trigger_intr_cb();
    esp_rom_delay_us(2000); // Short busy delay to ensure interrupt has triggered
    // Verify that task A2 has run
    TEST_ASSERT_EQUAL(2, a2_task_run_count);

    // Trigger B1 to resume scheduling on core B
    test_unblk_sync = 2;
    while (test_unblk_sync != 3) {
        ; // Wait for confirmation from B1 that scheduler has been resumed
    }
    // Verify that the scheduler is still enabled on core A
    TEST_ASSERT_EQUAL(taskSCHEDULER_RUNNING, xTaskGetSchedulerState());

    // Cleanup A2 and interrupt
    deregister_intr_cb();
    vTaskSuspend(a2_task_hdl);
    vTaskDelay(10);
    vTaskDelete(a2_task_hdl);

    // Indicate done and wait to be deleted
    xSemaphoreGive(test_unblk_done_sem);
    vTaskSuspend(NULL);
}

static void test_unblk_b1_task(void *arg)
{
    // Wait to be started by A1
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    // Check scheduler is running on core B
    TEST_ASSERT_EQUAL(taskSCHEDULER_RUNNING, xTaskGetSchedulerState());
    // Suspend the scheduler on core B
    vTaskSuspendAll();
    TEST_ASSERT_EQUAL(taskSCHEDULER_SUSPENDED, xTaskGetSchedulerState());

    // Indicate to A1 that core B scheduler has been suspended
    test_unblk_sync = 1;

    while (test_unblk_sync != 2) {
        ;   // Wait for trigger from A1
    }
    // Resume the scheduler on core B
    xTaskResumeAll();
    TEST_ASSERT_EQUAL(taskSCHEDULER_RUNNING, xTaskGetSchedulerState());
    // Indicate to A1 that core B scheduler has been resumed
    test_unblk_sync = 3;

    // Indicate done and wait to be deleted
    xSemaphoreGive(test_unblk_done_sem);
    vTaskSuspend(NULL);
}

TEST_CASE("Test vTaskSuspendAll allows scheduling on other cores", "[freertos]")
{
    test_unblk_done_sem = xSemaphoreCreateCounting(CONFIG_FREERTOS_NUMBER_OF_CORES, 0);
    TEST_ASSERT_NOT_EQUAL(NULL, test_unblk_done_sem);

    for (int i = 0; i < CONFIG_FREERTOS_NUMBER_OF_CORES; i++) {
        test_unblk_sync = 0;
        // Create a tasks
        TaskHandle_t a1_task_hdl;
        TaskHandle_t b1_task_hdl;
        TEST_ASSERT_EQUAL(pdTRUE, xTaskCreatePinnedToCore(test_unblk_b1_task, "B1", 8192, NULL, UNITY_FREERTOS_PRIORITY + 1, &b1_task_hdl, !i));
        TEST_ASSERT_EQUAL(pdTRUE, xTaskCreatePinnedToCore(test_unblk_a1_task, "A1", 8192, (void *)b1_task_hdl, UNITY_FREERTOS_PRIORITY + 1, &a1_task_hdl, i));
        // Start A1 to and wait for both tasks to complete
        xTaskNotifyGive(a1_task_hdl);
        for (int j = 0; j < 2; j++) {
            xSemaphoreTake(test_unblk_done_sem, portMAX_DELAY);
        }
        // Suspend the test tasks in case they haven't suspended themselves yet
        vTaskSuspend(a1_task_hdl);
        vTaskSuspend(b1_task_hdl);
        vTaskDelay(10);

        // Cleanup tasks
        vTaskDelete(a1_task_hdl);
        vTaskDelete(b1_task_hdl);
        vTaskDelay(10);
    }

    vSemaphoreDelete(test_unblk_done_sem);
    // Add a short delay to allow the idle task to free any remaining task memory
    vTaskDelay(10);
}

#endif //SOC_GPTIMER_SUPPORTED

/* ---------------------------------------------------------------------------------------------------------------------
Test vTaskSuspendAll doesn't block unpinned tasks from being scheduled on other cores

Only runs on !CONFIG_FREERTOS_UNICORE

Purpose:
    - Test that disabling a scheduler on one core (e.g., core B) does not block unpinned tasks running on core B to be scheduled on core A

Procedure:
        - Suspend the scheduler on core A so that the unpinned task spawns on core B
        - Create a pinned task pinned to core B
        - Pinned task is blocked on start
        - Unpinned task verifies that it spawns on core B and then blocks
        - Resume the scheduler on core A
        - Unblock the pinned task
        - The pinned task suspends the scheduler on core B
        - The pinned task unblocks the unpinned task
        - Verify that the unpinned task can move to core A which has the scheduler running
        - Resume scheduler on core B
        - Cleanup the tasks

Expected:
    When the pinned task disables scheduling on core B...
        - The unpinned task, which was originally running on core B, moves to core A once it is unblocked
--------------------------------------------------------------------------------------------------------------------- */
static SemaphoreHandle_t test_unpinned_sem;
volatile uint8_t unpinned_task_running = 0;

static void test_unpinned_task(void* arg)
{
    BaseType_t *core_A = (BaseType_t *)arg;

    // Verify that the task is running on core B
    TEST_ASSERT_EQUAL(!(*core_A), xPortGetCoreID());

    // Wait to be unblocked by the pinned task
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    // Verify that the task is running on core A once it is unblocked
    TEST_ASSERT_EQUAL((*core_A), xPortGetCoreID());
    unpinned_task_running = 1;

    // Mark test completion
    xSemaphoreGive(test_unpinned_sem);

    // Cleanup
    vTaskSuspend(NULL);
}

static void test_pinned_task(void* arg)
{
    TaskHandle_t unpinned_task_hdl = (TaskHandle_t)arg;

    // Wait to be notified by the main task
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    // Check scheduler is running on core B
    TEST_ASSERT_EQUAL(taskSCHEDULER_RUNNING, xTaskGetSchedulerState());

    // Suspend the scheduler on core B
    vTaskSuspendAll();

    // Check scheduler is suspended on core B
    TEST_ASSERT_EQUAL(taskSCHEDULER_SUSPENDED, xTaskGetSchedulerState());

    // Unblock the unpinned task
    // It should run on core A which is not in suspended state
    // We use the FromISR() call to create an ISR scenario and to force the unblocked task to be placed in the ready list
    BaseType_t yield = 0;
    vTaskNotifyGiveFromISR(unpinned_task_hdl, &yield);

    // Busy wait to ensure that the unpinned task is scheduled
    while (unpinned_task_running == 0)
        ;

    // Resume scheduler on core B
    xTaskResumeAll();

    // Mark test completion
    xSemaphoreGive(test_unpinned_sem);

    // Cleanup
    vTaskSuspend(NULL);
}

TEST_CASE("Test vTaskSuspendAll doesn't block unpinned tasks from being scheduled on other cores", "[freertos]")
{
    TaskHandle_t pinned_task_hdl;
    TaskHandle_t unpinned_task_hdl;
    BaseType_t core_A = xPortGetCoreID();
    test_unpinned_sem = xSemaphoreCreateCounting(2, 0);

    // Suspend the scheduler on core A to avoid the unpinned task from being spawned on core A
    vTaskSuspendAll();

    // Create unpinned task. This should spin up on core B as we have suspended the scheduler on core A
    TEST_ASSERT_EQUAL(pdTRUE, xTaskCreatePinnedToCore(test_unpinned_task, "unpinned_task", 8192, &core_A, UNITY_FREERTOS_PRIORITY + 1, &unpinned_task_hdl, tskNO_AFFINITY));

    // Create task pinned to core B
    TEST_ASSERT_EQUAL(pdTRUE, xTaskCreatePinnedToCore(test_pinned_task, "pinned_task", 8192, (void *)unpinned_task_hdl, UNITY_FREERTOS_PRIORITY + 1, &pinned_task_hdl, !core_A));

    // Resume the scheduler on core A
    xTaskResumeAll();

    // Unblock the pinned task
    xTaskNotifyGive(pinned_task_hdl);

    // Wait for test completion
    for (int i = 0; i < 2; i++) {
        xSemaphoreTake(test_unpinned_sem, portMAX_DELAY);
    }
    // Suspend the test tasks in case they haven't suspended themselves yet
    vTaskSuspend(pinned_task_hdl);
    vTaskSuspend(unpinned_task_hdl);
    vTaskDelay(10);

    // Cleanup
    vTaskDelete(pinned_task_hdl);
    vTaskDelete(unpinned_task_hdl);
    vSemaphoreDelete(test_unpinned_sem);
}
#endif  // !CONFIG_FREERTOS_UNICORE

/* ---------------------------------------------------------------------------------------------------------------------
Test xTaskResumeAll() resumes pended tasks on the current core

Purpose:
    - When the scheduler is suspended on a particular core, test that tasks unblocked by an ISR on that core will place
      those tasks on the core's pending ready list if the tasks are pinned to the core with the suspended scheduler.
      Tasks which have affinity to the other core must be resumed.
    - When the scheduler is resumed on a particular core, test that the tasks on core's pending ready list will be
      scheduled.

Procedure:
    Test for each core
        - Create some blocking tasks on the same core
        - Register an interrupt on the same core to unblock those tasks
        - Suspend the scheduler on the core
        - Trigger the interrupt to unblock those tasks
        - Resume the scheduler
        - Cleanup

Expected:
    - When the ISR unblocks the blocked tasks, the task's state should be ready
    - When the scheduler is resumed, the tasks should be scheduled and run without issue.
--------------------------------------------------------------------------------------------------------------------- */

#define TEST_PENDED_NUM_BLOCKED_TASKS   4

#if SOC_GPTIMER_SUPPORTED

static bool test_pended_isr(void *arg)
{
    TaskHandle_t *blkd_tsks = (TaskHandle_t *)arg;
    BaseType_t task_woken = pdFALSE;
    // Unblock the blocked tasks
    for (int i = 0; i < TEST_PENDED_NUM_BLOCKED_TASKS; i++) {
        vTaskNotifyGiveFromISR(blkd_tsks[i], &task_woken);
    }

    return (task_woken == pdTRUE);
}

static void test_pended_blkd_task(void *arg)
{
    volatile bool *has_run = (bool *)arg;
    // Wait to be unblocked
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    // Indicate the task been unblocked and has run
    *has_run = true;
    // Wait to be deleted
    vTaskSuspend(NULL);
}

static void test_pended_running_task(void *arg)
{
    TaskHandle_t main_task_hdl = (TaskHandle_t)arg;
    TaskHandle_t blkd_tsks[TEST_PENDED_NUM_BLOCKED_TASKS];
    volatile bool has_run[TEST_PENDED_NUM_BLOCKED_TASKS];

    // Created blocked tasks pinned to each core
    for (int i = 0; i < TEST_PENDED_NUM_BLOCKED_TASKS; i++) {
        has_run[i] = false;
        TEST_ASSERT_EQUAL(pdTRUE, xTaskCreatePinnedToCore(test_pended_blkd_task, "blkd", 4096, (void *)&has_run[i], UNITY_FREERTOS_PRIORITY + 2, &blkd_tsks[i], i % CONFIG_FREERTOS_NUMBER_OF_CORES));
    }
    vTaskDelay(10);

    // Install an interrupt on the current core core
    register_intr_cb(test_pended_isr, (void *)blkd_tsks);

    // Checked that all tasks are blocked and have no run yet
    for (int i = 0; i < TEST_PENDED_NUM_BLOCKED_TASKS; i++) {
        TEST_ASSERT_EQUAL(eBlocked, eTaskGetState(blkd_tsks[i])); // Should be eSuspended due to portMAX_DELAY
        TEST_ASSERT_EQUAL(false, has_run[i]);
    }

    // Suspend the scheduler on the current core
    vTaskSuspendAll();

    // Trigger the interrupt to unblocked the blocked tasks
    trigger_intr_cb();
    esp_rom_delay_us(2000); // Short busy delay to ensure interrupt has triggered

    // Check that tasks which have affinity to the current core are blocked and have not run as the scheduler is suspended.
    // While tasks which do not have affinity to the current core are unblocked.
    for (int i = 0; i < TEST_PENDED_NUM_BLOCKED_TASKS; i++) {
        // Note: We use eBlocked instead of eReady due to a bug in eTaskGetState(). See (IDF-5543)
        if (xTaskGetCoreID(blkd_tsks[i]) == xPortGetCoreID()) {
            TEST_ASSERT_EQUAL(eBlocked, eTaskGetState(blkd_tsks[i]));
            TEST_ASSERT_EQUAL(false, has_run[i]);
        } else {
            TEST_ASSERT_NOT_EQUAL(eBlocked, eTaskGetState(blkd_tsks[i]));
        }
    }

    // Resume the scheduler on the current core to schedule the unblocked tasks
    xTaskResumeAll();
    esp_rom_delay_us(10000); // Busy delay to ensure each task has enough time to run

    // Check that all tasks have run
    for (int i = 0; i < TEST_PENDED_NUM_BLOCKED_TASKS; i++) {
        TEST_ASSERT_EQUAL(true, has_run[i]);
    }

    // Clean up the interrupt and tasks
    deregister_intr_cb();
    for (int i = 0; i < TEST_PENDED_NUM_BLOCKED_TASKS; i++) {
        vTaskDelete(blkd_tsks[i]);
    }

    // Notify completion and wait for deletion
    xTaskNotifyGive(main_task_hdl);
    vTaskSuspend(NULL);
}

TEST_CASE("Test xTaskResumeAll resumes pended tasks", "[freertos]")
{
    // Run the test on each core
    for (int i = 0; i < CONFIG_FREERTOS_NUMBER_OF_CORES; i++) {
        TaskHandle_t susp_tsk_hdl;
        TEST_ASSERT_EQUAL(pdTRUE, xTaskCreatePinnedToCore(test_pended_running_task, "susp", 2048, (void *)xTaskGetCurrentTaskHandle(), UNITY_FREERTOS_PRIORITY + 1, &susp_tsk_hdl, i));
        // Wait for to be notified to test completion
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        // Suspend the test task in case it hasn't suspended itself yet
        vTaskSuspend(susp_tsk_hdl);
        vTaskDelay(10);
        vTaskDelete(susp_tsk_hdl);
    }
    // Add a short delay to allow the idle task to free any remaining task memory
    vTaskDelay(10);
}

#endif //SOC_GPTIMER_SUPPORTED

/* ---------------------------------------------------------------------------------------------------------------------
Test xTaskSuspendAll on both cores pends all tasks and xTaskResumeAll on both cores resumes all tasks

Purpose:
    - When the scheduler is suspended on both cores, test that tasks unblocked by an ISR on a core would place the
      those tasks on the core's pending ready list.
    - When the scheduler is resumed on both cores, test that each core will schedule the
      tasks from their respective pending ready lists.

Procedure:
        - Create some blocking tasks pinned on both cores
        - Create a task which suspends the scheduler on the other core
        - Suspend the scheduler respectively on both cores
        - Unblock pinned tasks on both cores once the scheduler is suspended
        - Test that unblocked tasks are not scheduled
        - Resume the scheduler respectively on both cores
        - Test that unblocked tasks are now scheduled
        - Cleanup

Expected:
    - When the ISR unblocks the blocked tasks, the task's state should be blocked
    - When the scheduler is resumed, the tasks should be scheduled and run without issue
--------------------------------------------------------------------------------------------------------------------- */

#if !CONFIG_FREERTOS_UNICORE
TaskHandle_t blkd_tasks[TEST_PENDED_NUM_BLOCKED_TASKS];
SemaphoreHandle_t done_sem;

static void test_susp_task(void *arg)
{
    bool *has_run = (bool *)arg;

    // Suspend the scheduler on this core
    vTaskSuspendAll();

    for (int i = 0; i < TEST_PENDED_NUM_BLOCKED_TASKS; i++) {
        if ((i % CONFIG_FREERTOS_NUMBER_OF_CORES) == xPortGetCoreID()) {
            // Unblock the blocked tasks pinned to this core.
            // We use the FromISR() call to create an ISR scenario and to force the unblocked task to be placed
            // on the pending ready list
            BaseType_t yield = pdFALSE;
            vTaskNotifyGiveFromISR(blkd_tasks[i], &yield);

            // The unblocked task must still be blocked and must not have run
            TEST_ASSERT_EQUAL(eBlocked, eTaskGetState(blkd_tasks[i]));
            TEST_ASSERT_EQUAL(false, has_run[i]);
        }
    }

    // Resume the scheduler on this core
    xTaskResumeAll();

    // Signal test completion
    xSemaphoreGive(done_sem);

    // Wait for cleanup
    vTaskSuspend(NULL);
}

#if SOC_GPTIMER_SUPPORTED
TEST_CASE("Test xTaskSuspendAll on all cores pends all tasks and xTaskResumeAll on all cores resumes all tasks", "[freertos]")
{
    volatile bool has_run[TEST_PENDED_NUM_BLOCKED_TASKS];
    done_sem = xSemaphoreCreateBinary();

    // Creat blocked tasks pinned to each core
    for (int i = 0; i < TEST_PENDED_NUM_BLOCKED_TASKS; i++) {
        has_run[i] = false;
        TEST_ASSERT_EQUAL(pdTRUE, xTaskCreatePinnedToCore(test_pended_blkd_task, "blkd", 4096, (void *)&has_run[i], UNITY_FREERTOS_PRIORITY + 2, &blkd_tasks[i], i % CONFIG_FREERTOS_NUMBER_OF_CORES));
    }
    vTaskDelay(10);

    // Create pinned task on the other core which will suspend its scheduler
    TaskHandle_t susp_task;
    TEST_ASSERT_EQUAL(pdTRUE, xTaskCreatePinnedToCore(test_susp_task, "susp_task", 2048, (void *)has_run, UNITY_FREERTOS_PRIORITY, &susp_task, !xPortGetCoreID()));

    // Suspend the scheduler on this core
    vTaskSuspendAll();

    for (int i = 0; i < TEST_PENDED_NUM_BLOCKED_TASKS; i++) {
        if ((i % CONFIG_FREERTOS_NUMBER_OF_CORES) == xPortGetCoreID()) {
            // Unblock the blocked tasks pinned to this core.
            // We use the FromISR() call to create an ISR scenario and to force the unblocked task to be placed
            // on the pending ready list
            BaseType_t yield = pdFALSE;
            vTaskNotifyGiveFromISR(blkd_tasks[i], &yield);

            // The unblocked task must still be blocked and must not have run
            TEST_ASSERT_EQUAL(eBlocked, eTaskGetState(blkd_tasks[i]));
            TEST_ASSERT_EQUAL(false, has_run[i]);
        }
    }

    // Resume scheduler on this core
    xTaskResumeAll();

    // Wait for test completion
    xSemaphoreTake(done_sem, portMAX_DELAY);

    // Verify that all blocked tasks have resumed and run when the schdulers are resumed on both cores
    for (int i = 0; i < TEST_PENDED_NUM_BLOCKED_TASKS; i++) {
        TEST_ASSERT_NOT_EQUAL(eBlocked, eTaskGetState(blkd_tasks[i]));
        TEST_ASSERT_EQUAL(true, has_run[i]);
    }

    // Cleanup
    for (int i = 0; i < TEST_PENDED_NUM_BLOCKED_TASKS; i++) {
        vTaskSuspend(blkd_tasks[i]);
        vTaskDelay(10);
        vTaskDelete(blkd_tasks[i]);
    }
    vTaskSuspend(susp_task);
    vTaskDelay(10);
    vTaskDelete(susp_task);
    vSemaphoreDelete(done_sem);
}
#endif //SOC_GPTIMER_SUPPORTED
#endif  // !CONFIG_FREERTOS_UNICORE

/* ---------------------------------------------------------------------------------------------------------------------
Test vTaskSuspendAll pinned task scheduling

Purpose:
    - Test that when we disable the scheduler on core X, core X does not schedule any unblocked tasks pinned to it until
      scheduling is resumed.
    - While the scheduler on a core X is suspended, test that...
        - A task pinned to core X is not scheduled even if its unblock time has been met
        - The task is scheduled as soon as the scheduler on the core is resumed

Procedure:
    Each core gets tested in the role of core X
        - Create task A1 pinned to core X that will suspend scheduling on core X
        - Create task A2 pinned to core X that will test unblocking on core X
        - Put task A2 in blocked state with a finite delay
        - Suspend the scheduler on core X from task A1
        - Make sure that the delay time on task A2 expires
        - Resume scheduler on core X from task A1
        - Cleanup the tasks

Expected:
        - When A1 disables scheduling, A2 should not be scheduled even after expiry of its delay time
        - When A1 resumes scheduling, A2 should be scheduled
--------------------------------------------------------------------------------------------------------------------- */

#define TEST_BLOCKED_TASK_DELAY_MS   100

volatile static bool has_run = false;
SemaphoreHandle_t done_sem1;

void test_blocked_task(void *arg)
{
    // Wait to be started
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    // Mark has_run as false to begin the test
    has_run = false;

    // Got to blocked state
    vTaskDelay(TEST_BLOCKED_TASK_DELAY_MS / portTICK_PERIOD_MS);

    // Mark when this task runs
    has_run = true;

    // Cleanup
    vTaskDelete(NULL);
}

void test_suspend_task(void *arg)
{
    TaskHandle_t blkd_task = (TaskHandle_t)arg;

    // Wait to be started
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    // Start the task which would block
    xTaskNotifyGive(blkd_task);

    // Verify the state of the blocked task is eBlocked
    TEST_ASSERT_EQUAL(eBlocked, eTaskGetState(blkd_task));

    // Suspend the scheduler on this core
    vTaskSuspendAll();

    // Busy spin for a time which ensures that the blocked task's delay expires
    esp_rom_delay_us(TEST_BLOCKED_TASK_DELAY_MS * 1000 * 2);

    // Verify that the blocked task has not been scheduled
    TEST_ASSERT_EQUAL(false, has_run);

    // Resume the scheduler
    xTaskResumeAll();

    // Let the blocked task to be scheduled
    vTaskDelay(10);

    // Verify that state of the blocked task is not eBlocked
    TEST_ASSERT_NOT_EQUAL(eBlocked, eTaskGetState(blkd_task));

    // Verify that the blocked task has run after scheduler is resumed
    TEST_ASSERT_EQUAL(true, has_run);

    // Signal test completion
    xSemaphoreGive(done_sem1);

    // Cleanup
    vTaskDelete(NULL);
}

TEST_CASE("Test vTaskSuspendAll pinned task scheduling", "[freertos]")
{
    for (int x = 0; x < portNUM_PROCESSORS; x++) {
        TaskHandle_t susp_task;
        TaskHandle_t blkd_task;
        done_sem1 = xSemaphoreCreateBinary();
        TEST_ASSERT_NOT_EQUAL(NULL, done_sem1);

        // Create pinned task on core x which will block
        // Ensure that this has a higher priority than the suspension task so that it immediately runs when the scheduler resumes
        TEST_ASSERT_EQUAL(pdTRUE, xTaskCreatePinnedToCore(test_blocked_task, "blkd", 4096, NULL, UNITY_FREERTOS_PRIORITY + 1, &blkd_task, x));

        // Create pinned task on core x which will suspend its scheduler
        TEST_ASSERT_EQUAL(pdTRUE, xTaskCreatePinnedToCore(test_suspend_task, "susp", 4096, (void *)blkd_task, UNITY_FREERTOS_PRIORITY, &susp_task, x));

        // Start the scheduler suspension task
        xTaskNotifyGive(susp_task);

        // Wait for test completion
        xSemaphoreTake(done_sem1, portMAX_DELAY);

        // Cleanup
        vSemaphoreDelete(done_sem1);

        // Add a short delay to allow the idle task to free any remaining task memory
        vTaskDelay(10);
    }
}

#endif // !CONFIG_FREERTOS_SMP
