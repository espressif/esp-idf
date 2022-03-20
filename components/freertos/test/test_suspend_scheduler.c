/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* Tests for FreeRTOS scheduler suspend & resume all tasks */
#include "sdkconfig.h"
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "unity.h"
#include "test_utils.h"
#include "esp_intr_alloc.h"
#include "driver/gptimer.h"
#include "esp_private/gptimer.h"
#include "sdkconfig.h"

#include "esp_rom_sys.h"

static SemaphoreHandle_t isr_semaphore;
static volatile unsigned isr_count;

/* Timer ISR increments an ISR counter, and signals a
   mutex semaphore to wake up another counter task */
static bool on_timer_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    portBASE_TYPE higher_awoken = pdFALSE;
    isr_count++;
    xSemaphoreGiveFromISR(isr_semaphore, &higher_awoken);
    return higher_awoken == pdTRUE;
}

typedef struct {
    SemaphoreHandle_t trigger_sem;
    volatile unsigned counter;
} counter_config_t;

static void counter_task_fn(void *vp_config)
{
    counter_config_t *config = (counter_config_t *)vp_config;
    printf("counter_task running...\n");
    while (1) {
        xSemaphoreTake(config->trigger_sem, portMAX_DELAY);
        config->counter++;
    }
}


/* This test verifies that an interrupt can wake up a task while the scheduler is disabled.

   In the FreeRTOS implementation, this exercises the xPendingReadyList for that core.
 */
TEST_CASE("Scheduler disabled can handle a pending context switch on resume", "[freertos]")
{
    isr_count = 0;
    TaskHandle_t counter_task;
    gptimer_handle_t gptimer = NULL;
    intr_handle_t intr_handle = NULL;
    isr_semaphore = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(isr_semaphore);

    counter_config_t count_config = {
        .trigger_sem = isr_semaphore,
        .counter = 0,
    };
    xTaskCreatePinnedToCore(counter_task_fn, "counter", 2048,
                            &count_config, UNITY_FREERTOS_PRIORITY + 1,
                            &counter_task, UNITY_FREERTOS_CPU);

    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_APB,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1MHz, 1 tick=1us
    };
    TEST_ESP_OK(gptimer_new_timer(&timer_config, &gptimer));
    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = 1000, // alarm period 1ms
        .flags.auto_reload_on_alarm = true,
    };
    gptimer_event_callbacks_t cbs = {
        .on_alarm = on_timer_alarm_cb,
    };
    TEST_ESP_OK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));
    TEST_ESP_OK(gptimer_set_alarm_action(gptimer, &alarm_config));
    TEST_ESP_OK(gptimer_start(gptimer));
    TEST_ESP_OK(gptimer_get_intr_handle(gptimer, &intr_handle));

    vTaskDelay(pdMS_TO_TICKS(20));

    // Check some counts have been triggered via the ISR
    TEST_ASSERT(count_config.counter > 10);
    TEST_ASSERT(isr_count > 10);

    for (int i = 0; i < 20; i++) {
#ifdef CONFIG_FREERTOS_SMP
        //Note: Scheduler suspension behavior changed in FreeRTOS SMP
        vTaskPreemptionDisable(NULL);
#else
        vTaskSuspendAll();
#endif // CONFIG_FREERTOS_SMP
        esp_intr_noniram_disable();

        unsigned no_sched_task = count_config.counter;

        // scheduler off on this CPU...
        esp_rom_delay_us(20 * 1000);

        TEST_ASSERT_EQUAL(count_config.counter, no_sched_task);

        // disable timer interrupts
        esp_intr_disable(intr_handle);

        // When we resume scheduler, we expect the counter task
        // will preempt and count at least one more item
        esp_intr_noniram_enable();
        esp_intr_enable(intr_handle);
#ifdef CONFIG_FREERTOS_SMP
        //Note: Scheduler suspension behavior changed in FreeRTOS SMP
        vTaskPreemptionEnable(NULL);
#else
        xTaskResumeAll();
#endif // CONFIG_FREERTOS_SMP

        TEST_ASSERT_NOT_EQUAL(count_config.counter, no_sched_task);
    }

    TEST_ESP_OK(gptimer_stop(gptimer));
    TEST_ESP_OK(gptimer_del_timer(gptimer));
    vTaskDelete(counter_task);
    vSemaphoreDelete(isr_semaphore);
}

/* Multiple tasks on different cores can be added to the pending ready list
   while scheduler is suspended, and should be started once the scheduler
   resumes.
*/
TEST_CASE("Scheduler disabled can wake multiple tasks on resume", "[freertos]")
{
#define TASKS_PER_PROC 4
    TaskHandle_t tasks[portNUM_PROCESSORS][TASKS_PER_PROC] = { 0 };
    counter_config_t counters[portNUM_PROCESSORS][TASKS_PER_PROC] = { 0 };

    /* Start all the tasks, they will block on isr_semaphore */
    for (int p = 0; p < portNUM_PROCESSORS; p++) {
        for (int t = 0; t < TASKS_PER_PROC; t++) {
            counters[p][t].trigger_sem = xSemaphoreCreateMutex();
            TEST_ASSERT_NOT_NULL( counters[p][t].trigger_sem );
            TEST_ASSERT( xSemaphoreTake(counters[p][t].trigger_sem, 0) );
            xTaskCreatePinnedToCore(counter_task_fn, "counter", 2048,
                                    &counters[p][t], UNITY_FREERTOS_PRIORITY + 1,
                                    &tasks[p][t], p);
            TEST_ASSERT_NOT_NULL( tasks[p][t] );
        }
    }

    /* takes a while to initialize tasks on both cores, sometimes... */
    vTaskDelay(TASKS_PER_PROC * portNUM_PROCESSORS * 3);

    /* Check nothing is counting, each counter should be blocked on its trigger_sem */
    for (int p = 0; p < portNUM_PROCESSORS; p++) {
        for (int t = 0; t < TASKS_PER_PROC; t++) {
            TEST_ASSERT_EQUAL(0, counters[p][t].counter);
        }
    }

    /* Suspend scheduler on this CPU */
#ifdef CONFIG_FREERTOS_SMP
    //Note: Scheduler suspension behavior changed in FreeRTOS SMP
    vTaskPreemptionDisable(NULL);
#else
    vTaskSuspendAll();
#endif // CONFIG_FREERTOS_SMP

    /* Give all the semaphores once. This will wake tasks immediately on the other
       CPU, but they are deferred here until the scheduler resumes.
     */
    for (int p = 0; p < portNUM_PROCESSORS; p++) {
        for (int t = 0; t < TASKS_PER_PROC; t++) {
            xSemaphoreGive(counters[p][t].trigger_sem);
        }
    }

    esp_rom_delay_us(200); /* Let the other CPU do some things */

    for (int p = 0; p < portNUM_PROCESSORS; p++) {
        for (int t = 0; t < TASKS_PER_PROC; t++) {
            int expected = (p == UNITY_FREERTOS_CPU) ? 0 : 1; // Has run if it was on the other CPU
            esp_rom_printf("Checking CPU %d task %d (expected %d actual %d)\n", p, t, expected, counters[p][t].counter);
            TEST_ASSERT_EQUAL(expected, counters[p][t].counter);
        }
    }

    /* Resume scheduler */
#ifdef CONFIG_FREERTOS_SMP
    //Note: Scheduler suspension behavior changed in FreeRTOS SMP
    vTaskPreemptionEnable(NULL);
#else
    xTaskResumeAll();
#endif // CONFIG_FREERTOS_SMP

    /* Now the tasks on both CPUs should have been woken once and counted once. */
    for (int p = 0; p < portNUM_PROCESSORS; p++) {
        for (int t = 0; t < TASKS_PER_PROC; t++) {
            esp_rom_printf("Checking CPU %d task %d (expected 1 actual %d)\n", p, t, counters[p][t].counter);
            TEST_ASSERT_EQUAL(1, counters[p][t].counter);
        }
    }

    /* Clean up */
    for (int p = 0; p < portNUM_PROCESSORS; p++) {
        for (int t = 0; t < TASKS_PER_PROC; t++) {
            vTaskDelete(tasks[p][t]);
            vSemaphoreDelete(counters[p][t].trigger_sem);
        }
    }
}

#ifndef CONFIG_FREERTOS_UNICORE
static volatile bool sched_suspended;
static void suspend_scheduler_5ms_task_fn(void *ignore)
{
#ifdef CONFIG_FREERTOS_SMP
    //Note: Scheduler suspension behavior changed in FreeRTOS SMP
    vTaskPreemptionDisable(NULL);
#else
    vTaskSuspendAll();
#endif // CONFIG_FREERTOS_SMP
    sched_suspended = true;
    for (int i = 0; i < 5; i++) {
        esp_rom_delay_us(1000);
    }
#ifdef CONFIG_FREERTOS_SMP
    //Note: Scheduler suspension behavior changed in FreeRTOS SMP
    vTaskPreemptionEnable(NULL);
#else
    xTaskResumeAll();
#endif // CONFIG_FREERTOS_SMP
    sched_suspended = false;
    vTaskDelete(NULL);
}

/* If the scheduler is disabled on one CPU (A) with a task blocked on something, and a task
   on B (where scheduler is running) wakes it, then the task on A should be woken on resume.
*/
TEST_CASE("Scheduler disabled on CPU B, tasks on A can wake", "[freertos]")
{
    TaskHandle_t counter_task;
    SemaphoreHandle_t wake_sem = xSemaphoreCreateMutex();
    xSemaphoreTake(wake_sem, 0);
    counter_config_t count_config = {
        .trigger_sem = wake_sem,
        .counter = 0,
    };
    xTaskCreatePinnedToCore(counter_task_fn, "counter", 2048,
                            &count_config, UNITY_FREERTOS_PRIORITY + 1,
                            &counter_task, !UNITY_FREERTOS_CPU);

    xTaskCreatePinnedToCore(suspend_scheduler_5ms_task_fn, "suspender", 2048,
                            NULL, UNITY_FREERTOS_PRIORITY - 1,
                            NULL, !UNITY_FREERTOS_CPU);

    /* counter task is now blocked on other CPU, waiting for wake_sem, and we expect
     that this CPU's scheduler will be suspended for 5ms shortly... */
    while (!sched_suspended) { }

    xSemaphoreGive(wake_sem);
    esp_rom_delay_us(1000);
    // Bit of a race here if the other CPU resumes its scheduler, but 5ms is a long time... */
    TEST_ASSERT(sched_suspended);
    TEST_ASSERT_EQUAL(0, count_config.counter); // the other task hasn't woken yet, because scheduler is off
    TEST_ASSERT(sched_suspended);

    /* wait for the rest of the 5ms... */
    while (sched_suspended) { }

    esp_rom_delay_us(100);
    TEST_ASSERT_EQUAL(1, count_config.counter); // when scheduler resumes, counter task should immediately count

    vTaskDelete(counter_task);
}
#endif
