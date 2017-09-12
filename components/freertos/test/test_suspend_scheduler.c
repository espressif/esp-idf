/* Tests for FreeRTOS scheduler suspend & resume all tasks */
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"
#include "unity.h"
#include "soc/cpu.h"

#include "driver/timer.h"

static SemaphoreHandle_t isr_semaphore;
static volatile unsigned isr_count, task_count;

/* Timer ISR increments an ISR counter, and signals a
   mutex semaphore to wake up another counter task */
static void timer_group0_isr(void *vp_arg)
{
    TIMERG0.int_clr_timers.t0 = 1;
    TIMERG0.hw_timer[TIMER_0].update = 1;
    TIMERG0.hw_timer[TIMER_0].config.alarm_en = 1;
    portBASE_TYPE higher_awoken = pdFALSE;
    isr_count++;
    xSemaphoreGiveFromISR(isr_semaphore, &higher_awoken);
    if (higher_awoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

static void counter_task_fn(void *ignore)
{
    printf("counter_task running...\n");
    while(1) {
        xSemaphoreTake(isr_semaphore, portMAX_DELAY);
        task_count++;
    }
}


/* This test verifies that an interrupt can wake up a task while the scheduler is disabled.

   In the FreeRTOS implementation, this exercises the xPendingReadyList for that core.
 */
TEST_CASE("Handle pending context switch while scheduler disabled", "[freertos]")
{
    task_count = 0;
    isr_count = 0;
    isr_semaphore = xSemaphoreCreateMutex();
    TaskHandle_t counter_task;
    intr_handle_t isr_handle = NULL;

    xTaskCreatePinnedToCore(counter_task_fn, "counter", 2048,
                            NULL, UNITY_FREERTOS_PRIORITY + 1,
                            &counter_task, UNITY_FREERTOS_CPU);

    /* Configure timer ISR */
    const timer_config_t config = {
        .alarm_en = 1,
        .auto_reload = 1,
        .counter_dir = TIMER_COUNT_UP,
        .divider = 1,
        .intr_type = TIMER_INTR_LEVEL,
        .counter_en = TIMER_PAUSE,
    };
    /* Configure timer */
    timer_init(TIMER_GROUP_0, TIMER_0, &config);
    timer_pause(TIMER_GROUP_0, TIMER_0);
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 1000);
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    timer_isr_register(TIMER_GROUP_0, TIMER_0, timer_group0_isr, NULL, 0, &isr_handle);
    timer_start(TIMER_GROUP_0, TIMER_0);

    vTaskDelay(5);

    // Check some counts have been triggered via the ISR
    TEST_ASSERT(task_count > 10);
    TEST_ASSERT(isr_count > 10);

    for (int i = 0; i < 20; i++) {
        vTaskSuspendAll();
        esp_intr_noniram_disable();

        unsigned no_sched_task = task_count;

        // scheduler off on this CPU...
        ets_delay_us(20 * 1000);

        //TEST_ASSERT_NOT_EQUAL(no_sched_isr, isr_count);
        TEST_ASSERT_EQUAL(task_count, no_sched_task);

        // disable timer interrupts
        timer_disable_intr(TIMER_GROUP_0, TIMER_0);

        // When we resume scheduler, we expect the counter task
        // will preempt and count at least one more item
        esp_intr_noniram_enable();
        xTaskResumeAll();

        TEST_ASSERT_NOT_EQUAL(task_count, no_sched_task);
    }

    esp_intr_free(isr_handle);
    timer_disable_intr(TIMER_GROUP_0, TIMER_0);

    vTaskDelete(counter_task);
    vSemaphoreDelete(isr_semaphore);
}
