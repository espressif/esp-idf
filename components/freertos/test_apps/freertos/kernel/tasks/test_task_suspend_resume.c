/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Tests for FreeRTOS task suspend & resume */
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
#include "unity.h"
#include "test_utils.h"
#include "driver/gptimer.h"
#ifndef CONFIG_FREERTOS_UNICORE
#include "esp_ipc.h"
#endif
#include "esp_freertos_hooks.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"

/* Counter task counts a target variable forever */
static void task_count(void *vp_counter)
{
    volatile unsigned *counter = (volatile unsigned *)vp_counter;
    *counter = 0;

    while (1) {
        (*counter)++;
        vTaskDelay(1);
    }
}

static void test_suspend_resume(int target_core)
{
    volatile unsigned counter = 0;
    TaskHandle_t counter_task;

    xTaskCreatePinnedToCore(task_count, "Count", 2048,
                            (void *)&counter, UNITY_FREERTOS_PRIORITY + 1,
                            &counter_task, target_core);

    vTaskDelay(10);
    /* check some counting has happened */
    TEST_ASSERT_NOT_EQUAL(0, counter);

    // Do the next part a few times, just to be sure multiple suspends & resumes
    // work as expected...
    const int TEST_ITERATIONS = 5;
    for (int i = 0; i < TEST_ITERATIONS; i++) {
        vTaskSuspend(counter_task);
        unsigned suspend_count = counter;
        printf("Suspending @ %d\n", suspend_count);

        vTaskDelay(2);

        printf("Still suspended @ %d\n", counter);

        /* check the counter hasn't gone up while the task is suspended */
        TEST_ASSERT_EQUAL(suspend_count, counter);
        vTaskResume(counter_task);
        vTaskDelay(2);

        printf("Resumed @ %d\n", counter);
        /* check the counter is going up again now the task is resumed */
        TEST_ASSERT_NOT_EQUAL(suspend_count, counter);
    }

    vTaskDelete(counter_task);
}


TEST_CASE("Suspend/resume task on same core", "[freertos]")
{
    test_suspend_resume(UNITY_FREERTOS_CPU);
}

#ifndef CONFIG_FREERTOS_UNICORE
/*
Note: This test is ignore for now due to a known issue (xKernelLock contention + task state change will lead to a race
condition). More specifically:

- test_suspend_resume() suspends task_count() thus moving it to the suspended list
- But task_count() is already contesting the xKernelLock.
- Changes its own task list once it takes the xKernelLock
- xKernelLock never receives the crosscore interrupt as it was contesting for xKernelLock
Addressed in IDF-5844
*/
TEST_CASE("Suspend/resume task on other core", "[freertos][ignore]")
{
    test_suspend_resume(!UNITY_FREERTOS_CPU);
}
#endif

/* Task suspends itself, then sets a flag and deletes itself */
static void task_suspend_self(void *vp_resumed)
{
    volatile bool *resumed = (volatile bool *)vp_resumed;
    *resumed = false;
    vTaskSuspend(NULL);
    *resumed = true;
    vTaskDelete(NULL);
}

TEST_CASE("Suspend the current running task", "[freertos]")
{
    volatile bool resumed = false;
    TaskHandle_t suspend_task;

    xTaskCreatePinnedToCore(task_suspend_self, "suspend_self", 2048,
                            (void *)&resumed, UNITY_FREERTOS_PRIORITY + 1,
                            &suspend_task, UNITY_FREERTOS_CPU);

    vTaskDelay(1);
    TEST_ASSERT_FALSE(resumed);

    vTaskResume(suspend_task);
    // Shouldn't need any delay here, as task should resume on this CPU immediately
    TEST_ASSERT_TRUE(resumed);
}


static volatile bool timer_isr_fired;
static gptimer_handle_t gptimer = NULL;

/* Timer ISR clears interrupt, sets flag, then resumes the task supplied in the
 * callback argument.
 */
bool on_timer_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    TaskHandle_t handle = user_ctx;
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    gptimer_stop(timer);
    timer_isr_fired = true;
    higherPriorityTaskWoken = xTaskResumeFromISR(handle);
    return higherPriorityTaskWoken == pdTRUE;
}

/* Task suspends itself, then sets parameter value to the current timer group counter and deletes itself */
static IRAM_ATTR void task_suspend_self_with_timer(void *vp_resumed)
{
    volatile uint64_t *resumed_counter = vp_resumed;
    *resumed_counter = 0;
    vTaskSuspend(NULL);
    gptimer_get_raw_count(gptimer, (uint64_t *)resumed_counter);
    vTaskDelete(NULL);
}


/* Create a task which suspends itself, then resume it from a timer
 * interrupt. */
static void test_resume_task_from_isr(int target_core)
{
    volatile uint64_t resumed_counter = 99;
    TaskHandle_t suspend_task;

    xTaskCreatePinnedToCore(task_suspend_self_with_timer, "suspend_self", 2048,
                            (void *)&resumed_counter, UNITY_FREERTOS_PRIORITY + 1,
                            &suspend_task, target_core);
    // delay to make the task has resumed itself
    vTaskDelay(1);
    TEST_ASSERT_EQUAL(0, resumed_counter);

    /* Configure timer ISR */
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1MHz, 1 tick = 1us
    };
    TEST_ESP_OK(gptimer_new_timer(&timer_config, &gptimer));
    timer_isr_fired = false;
    vTaskDelay(1); // Make sure we're at the start of a new tick
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = 1000000 / configTICK_RATE_HZ / 2,
        .reload_count = 0,
    };
    gptimer_event_callbacks_t cbs = {
        .on_alarm = on_timer_alarm_cb,
    };
    TEST_ESP_OK(gptimer_register_event_callbacks(gptimer, &cbs, suspend_task));
    TEST_ESP_OK(gptimer_set_alarm_action(gptimer, &alarm_config));
    TEST_ESP_OK(gptimer_enable(gptimer));
    TEST_ESP_OK(gptimer_start(gptimer));
    // wait the timer interrupt fires up
    vTaskDelay(2);
    TEST_ASSERT_TRUE(timer_isr_fired);
    // check the task was resumed
    TEST_ASSERT_NOT_EQUAL(0, resumed_counter);
    // The task should have woken within 500us of the timer interrupt event (note: task may be a flash cache miss)
    printf("alarm value %llu task resumed at %u\n", alarm_config.alarm_count, (unsigned)resumed_counter);
    TEST_ASSERT_UINT32_WITHIN(100, alarm_config.alarm_count, (unsigned)resumed_counter);

    // clean up
    TEST_ESP_OK(gptimer_disable(gptimer));
    TEST_ESP_OK(gptimer_del_timer(gptimer));
}

TEST_CASE("Resume task from ISR (same core)", "[freertos]")
{
    test_resume_task_from_isr(UNITY_FREERTOS_CPU);
}

#ifndef CONFIG_FREERTOS_UNICORE
TEST_CASE("Resume task from ISR (other core)", "[freertos]")
{
    test_resume_task_from_isr(!UNITY_FREERTOS_CPU);
}

#if !CONFIG_FREERTOS_SMP
/*
Scheduler suspension behavior has changed in SMP FreeRTOS, thus these test are disabled for SMP FreeRTOS.
See IDF-5201
*/

static volatile bool block;
static bool suspend_both_cpus;

static void IRAM_ATTR suspend_scheduler_while_block_set(void *arg)
{
    vTaskSuspendAll();

    while (block) { };
    esp_rom_delay_us(1);
    xTaskResumeAll();
}

static void IRAM_ATTR suspend_scheduler_on_both_cpus(void)
{
    block = true;
    if (suspend_both_cpus) {
        TEST_ESP_OK(esp_ipc_call((xPortGetCoreID() == 0) ? 1 : 0, &suspend_scheduler_while_block_set, NULL));
    }

    vTaskSuspendAll();
}

static void IRAM_ATTR resume_scheduler_on_both_cpus(void)
{
    block = false;
    xTaskResumeAll();
}

static const int waiting_ms = 2000;
static const int delta_ms = 100;
static int duration_wait_task_ms;
static int duration_ctrl_task_ms;

static void waiting_task(void *pvParameters)
{
    int cpu_id = xPortGetCoreID();
    int64_t start_time = esp_timer_get_time();
    printf("Start waiting_task cpu=%d\n", cpu_id);

    vTaskDelay(waiting_ms / portTICK_PERIOD_MS);

    duration_wait_task_ms = (esp_timer_get_time() - start_time) / 1000;
    printf("Finish waiting_task cpu=%d, time=%d ms\n", cpu_id, duration_wait_task_ms);
    vTaskDelete(NULL);
}

static void control_task(void *pvParameters)
{
    int cpu_id = xPortGetCoreID();
    esp_rom_delay_us(2000); // let to start the waiting_task first
    printf("Start control_task cpu=%d\n", cpu_id);
    int64_t start_time = esp_timer_get_time();

    suspend_scheduler_on_both_cpus();
    esp_rom_delay_us(waiting_ms * 1000 + delta_ms * 1000);
    resume_scheduler_on_both_cpus();

    duration_ctrl_task_ms = (esp_timer_get_time() - start_time) / 1000;
    printf("Finish control_task cpu=%d, time=%d ms\n", cpu_id, duration_ctrl_task_ms);
    vTaskDelete(NULL);
}

static void test_scheduler_suspend1(int cpu)
{
    /* This test tests a case then both CPUs were in suspend state and then resume CPUs back.
     * A task for which a wait time has been set and this time has elapsed in the suspended state should in any case be ready to start.
     * (In an old implementation of xTaskIncrementTick function the counting for waiting_task() will be continued
     * (excluding time in suspended) after control_task() is finished.)
     */
    duration_wait_task_ms = 0;
    duration_ctrl_task_ms = 0;

    printf("Test for CPU%d\n", cpu);
    int other_cpu = (cpu == 0) ? 1 : 0;
    xTaskCreatePinnedToCore(&waiting_task, "waiting_task", 8192, NULL, 5, NULL, other_cpu);
    xTaskCreatePinnedToCore(&control_task, "control_task", 8192, NULL, 5, NULL, cpu);

    vTaskDelay(waiting_ms * 2 / portTICK_PERIOD_MS);
    TEST_ASSERT_INT_WITHIN(4, waiting_ms + delta_ms + 4, duration_ctrl_task_ms);
    if (suspend_both_cpus == false && cpu == 1) {
        // CPU0 continues to increase the TickCount and the wait_task does not depend on Suspended Scheduler on CPU1
        TEST_ASSERT_INT_WITHIN(2, waiting_ms, duration_wait_task_ms);
    } else {
        TEST_ASSERT_INT_WITHIN(4, waiting_ms + delta_ms + 4, duration_wait_task_ms);
    }

    printf("\n");
}

TEST_CASE("Test the waiting task not missed due to scheduler suspension on both CPUs", "[freertos]")
{
    printf("Suspend both CPUs:\n");
    suspend_both_cpus = true;
    test_scheduler_suspend1(0);
    test_scheduler_suspend1(1);
}

TEST_CASE("Test the waiting task not missed due to scheduler suspension on one CPU", "[freertos]")
{
    printf("Suspend only one CPU:\n");
    suspend_both_cpus = false;
    test_scheduler_suspend1(0);
    test_scheduler_suspend1(1);
}

static uint32_t tick_hook_ms[2];

static void IRAM_ATTR tick_hook(void)
{
    tick_hook_ms[xPortGetCoreID()] += portTICK_PERIOD_MS;
}

static void test_scheduler_suspend2(int cpu)
{
    esp_register_freertos_tick_hook_for_cpu(tick_hook, 0);
    esp_register_freertos_tick_hook_for_cpu(tick_hook, 1);

    memset(tick_hook_ms, 0, sizeof(tick_hook_ms));

    printf("Test for CPU%d\n", cpu);
    xTaskCreatePinnedToCore(&control_task, "control_task", 8192, NULL, 5, NULL, cpu);

    vTaskDelay(waiting_ms * 2 / portTICK_PERIOD_MS);
    esp_deregister_freertos_tick_hook(tick_hook);

    printf("tick_hook_ms[cpu0] = %d, tick_hook_ms[cpu1] = %d\n", tick_hook_ms[0], tick_hook_ms[1]);

    TEST_ASSERT_INT_WITHIN(portTICK_PERIOD_MS * 2, waiting_ms * 2, tick_hook_ms[0]);
    TEST_ASSERT_INT_WITHIN(portTICK_PERIOD_MS * 2, waiting_ms * 2, tick_hook_ms[1]);
    printf("\n");
}

TEST_CASE("Test suspend-resume CPU. The number of tick_hook should be the same for both CPUs", "[freertos]")
{
    printf("Suspend both CPUs:\n");
    suspend_both_cpus = true;
    test_scheduler_suspend2(0);
    test_scheduler_suspend2(1);

    printf("Suspend only one CPU:\n");
    suspend_both_cpus = false;
    test_scheduler_suspend2(0);
    test_scheduler_suspend2(1);
}

static int duration_timer_ms;

static void timer_callback(TimerHandle_t arg)
{
    duration_timer_ms += portTICK_PERIOD_MS;
}

static void test_scheduler_suspend3(int cpu)
{
    duration_timer_ms = 0;
    duration_ctrl_task_ms = 0;

    printf("Test for CPU%d\n", cpu);
    TimerHandle_t count_time = xTimerCreate("count_time", 1, pdTRUE, NULL, timer_callback);
    xTimerStart( count_time, portMAX_DELAY);
    xTaskCreatePinnedToCore(&control_task, "control_task", 8192, NULL, 5, NULL, cpu);

    vTaskDelay(waiting_ms * 2 / portTICK_PERIOD_MS);
    xTimerDelete(count_time, portMAX_DELAY);
    printf("Finish duration_timer_ms=%d ms\n", duration_timer_ms);

    TEST_ASSERT_INT_WITHIN(2, waiting_ms * 2, duration_timer_ms);
    TEST_ASSERT_INT_WITHIN(5, waiting_ms + delta_ms, duration_ctrl_task_ms);

    printf("\n");
}

TEST_CASE("Test suspend-resume CPU works with xTimer", "[freertos]")
{
    printf("Suspend both CPUs:\n");
    suspend_both_cpus = true;
    test_scheduler_suspend3(0);
    test_scheduler_suspend3(1);

    printf("Suspend only one CPU:\n");
    suspend_both_cpus = false;
    test_scheduler_suspend3(0);
    test_scheduler_suspend3(1);
}
#endif // CONFIG_FREERTOS_UNICORE
#endif // !CONFIG_FREERTOS_SMP
