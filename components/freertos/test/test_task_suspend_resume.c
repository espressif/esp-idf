/* Tests for FreeRTOS task suspend & resume */
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"
#include "unity.h"
#include "soc/cpu.h"

#include "driver/timer.h"

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
TEST_CASE("Suspend/resume task on other core", "[freertos]")
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


volatile bool timer_isr_fired;

/* Timer ISR clears interrupt, sets flag, then resumes the task supplied in the
 * callback argument.
 */
void IRAM_ATTR timer_group0_isr(void *vp_arg)
{
    // Clear interrupt
    TIMERG0.int_clr_timers.val = TIMERG0.int_st_timers.val;

    timer_isr_fired = true;
    TaskHandle_t handle = vp_arg;
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    higherPriorityTaskWoken = xTaskResumeFromISR(handle);
    if (higherPriorityTaskWoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

/* Create a task which suspends itself, then resume it from a timer
 * interrupt. */
static void test_resume_task_from_isr(int target_core)
{
    volatile bool resumed = false;
    TaskHandle_t suspend_task;

    xTaskCreatePinnedToCore(task_suspend_self, "suspend_self", 2048,
                            (void *)&resumed, UNITY_FREERTOS_PRIORITY + 1,
                            &suspend_task, target_core);

    vTaskDelay(1);
    TEST_ASSERT_FALSE(resumed);

    /* Configure timer ISR */
    const timer_config_t config = {
        .alarm_en = 1,
        .auto_reload = 0,
        .counter_dir = TIMER_COUNT_UP,
        .divider = 2,       //Range is 2 to 65536
        .intr_type = TIMER_INTR_LEVEL,
        .counter_en = TIMER_PAUSE,
    };
    /*Configure timer*/
    timer_init(TIMER_GROUP_0, TIMER_0, &config);
    timer_pause(TIMER_GROUP_0, TIMER_0);
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 1000);
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    timer_isr_register(TIMER_GROUP_0, TIMER_0, timer_group0_isr, (void*)suspend_task, ESP_INTR_FLAG_IRAM, NULL);
    timer_isr_fired = false;
    timer_start(TIMER_GROUP_0, TIMER_0);

    vTaskDelay(1);

    TEST_ASSERT_TRUE(timer_isr_fired);
    TEST_ASSERT_TRUE(resumed);
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
#endif
