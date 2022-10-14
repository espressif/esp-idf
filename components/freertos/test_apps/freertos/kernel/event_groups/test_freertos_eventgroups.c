/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "driver/gptimer.h"
#include "unity.h"
#include "unity_test_utils.h"

#define NUM_TASKS               8
#define ALL_TASK_BITS           ((1 << NUM_TASKS) - 1)
#define BIT_CALL(task)          (1 << (task))
#define BIT_RESPONSE(task)      (1 << ((task) + NUM_TASKS))
#define ALL_CALL_BITS           (ALL_TASK_BITS)
#define ALL_RESPONSE_BITS       (ALL_TASK_BITS << NUM_TASKS)

static const int COUNT = 1000;
static EventGroupHandle_t eg;
static SemaphoreHandle_t done_sem;

static void task_event_group_call_response(void *param)
{
    int task_num = (int)param;

    printf("Started %d\n", task_num);

    for (int i = 0; i < COUNT; i++) {
        /* Wait until the common "call" bit is set, starts off all tasks
           (clear on return) */
        TEST_ASSERT( xEventGroupWaitBits(eg, BIT_CALL(task_num), true, false, portMAX_DELAY) );

        /* Set our individual "response" bit */
        xEventGroupSetBits(eg, BIT_RESPONSE(task_num));
    }

    printf("Task %d done\n", task_num);
    xSemaphoreGive(done_sem);
    // Wait to be deleted
    vTaskSuspend(NULL);
}

TEST_CASE("FreeRTOS Event Groups", "[freertos]")
{
    eg = xEventGroupCreate();
    done_sem = xSemaphoreCreateCounting(NUM_TASKS, 0);

    TaskHandle_t task_handles[NUM_TASKS];

    /* Note: task_event_group_call_response all have higher priority than this task, so on this core
       they will always preempt this task.

       This is important because we need to know all tasks have blocked on their own BIT_CALL(task_num) each time we
       signal it, or they get out of sync.
     */
    for (int c = 0; c < NUM_TASKS; c++) {
        xTaskCreatePinnedToCore(task_event_group_call_response, "tsk_call_resp", 4096, (void *)c, configMAX_PRIORITIES - 1, &task_handles[c], c % portNUM_PROCESSORS);
    }

    /* Tasks all start instantly, but this task will resume running at the same time as the higher priority tasks on the
       other processor may still be setting up, so allow time for them to also block on BIT_CALL()... */
    vTaskDelay(10);

    for (int i = 0; i < COUNT; i++) {
        /* signal all the "CALL" bits of each task */
        xEventGroupSetBits(eg, ALL_CALL_BITS);

        /* Wait until all tasks have set their respective response bits */
        TEST_ASSERT_EQUAL_HEX16(ALL_RESPONSE_BITS, xEventGroupWaitBits(eg, ALL_RESPONSE_BITS, true, true, portMAX_DELAY));
    }

    /* Ensure all tasks have suspend themselves */
    for (int c = 0; c < NUM_TASKS; c++) {
        TEST_ASSERT( xSemaphoreTake(done_sem, 100 / portTICK_PERIOD_MS) );
    }

    for (int c = 0; c < NUM_TASKS; c++) {
        unity_utils_task_delete(task_handles[c]);
    }

    vSemaphoreDelete(done_sem);
    vEventGroupDelete(eg);
}

static void task_test_sync(void *param)
{
    int task_num = (int)param;

    printf("Started %d\n", task_num);

    for (int i = 0; i < COUNT; i++) {
        /* set our bit, and wait on all tasks to set their bits */
        xEventGroupSync(eg, BIT_CALL(task_num), ALL_CALL_BITS, portMAX_DELAY);
        /* clear our bit */
        xEventGroupClearBits(eg, BIT_CALL(task_num));
    }
    int after_done = xEventGroupSetBits(eg, BIT_RESPONSE(task_num));

    printf("Done %d = 0x%08x\n", task_num, after_done);

    xSemaphoreGive(done_sem);
    vTaskDelete(NULL);
}

TEST_CASE("FreeRTOS Event Group Sync", "[freertos]")
{
    eg = xEventGroupCreate();
    done_sem = xSemaphoreCreateCounting(NUM_TASKS, 0);

    for (int c = 0; c < NUM_TASKS; c++) {
        xTaskCreatePinnedToCore(task_test_sync, "task_test_sync", 4096, (void *)c, configMAX_PRIORITIES - 1, NULL, c % portNUM_PROCESSORS);
    }

    for (int c = 0; c < NUM_TASKS; c++) {
        printf("Waiting on %d (0x%08x)\n", c, BIT_RESPONSE(c));
        TEST_ASSERT( xEventGroupWaitBits(eg, BIT_RESPONSE(c), false, false, portMAX_DELAY) );
    }

    /* Ensure all tasks cleaned up correctly */
    for (int c = 0; c < NUM_TASKS; c++) {
        TEST_ASSERT( xSemaphoreTake(done_sem, 100 / portTICK_PERIOD_MS) );
    }

    vSemaphoreDelete(done_sem);
    vEventGroupDelete(eg);
}

/*-----------------Test case for event group trace facilities-----------------*/
#ifdef  CONFIG_FREERTOS_USE_TRACE_FACILITY
/*
 * Test event group Trace Facility functions such as
 * xEventGroupClearBitsFromISR(), xEventGroupSetBitsFromISR()
 */

//Use a timer to trigger an ISr
#define BITS            0xAA
static gptimer_handle_t gptimer;
static bool test_set_bits;
static bool test_clear_bits;

static bool on_timer_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    portBASE_TYPE task_woken = pdFALSE;

    gptimer_stop(timer);

    if (test_set_bits) {
        xEventGroupSetBitsFromISR(eg, BITS, &task_woken);
        test_set_bits = false;
    } else if (test_clear_bits) {
        xEventGroupClearBitsFromISR(eg, BITS);
        xSemaphoreGiveFromISR(done_sem, &task_woken);
        test_clear_bits = false;
    }
    //Switch context if necessary
    return task_woken == pdTRUE;
}

TEST_CASE("FreeRTOS Event Group ISR", "[freertos]")
{
    done_sem = xSemaphoreCreateBinary();
    eg = xEventGroupCreate();
    test_set_bits = false;
    test_clear_bits = false;
    //Setup timer for ISR
    gptimer_config_t config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000,
    };
    TEST_ESP_OK(gptimer_new_timer(&config, &gptimer));
    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = 200000,
    };
    gptimer_event_callbacks_t cbs = {
        .on_alarm = on_timer_alarm_cb,
    };
    TEST_ESP_OK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));
    TEST_ESP_OK(gptimer_enable(gptimer));
    TEST_ESP_OK(gptimer_set_alarm_action(gptimer, &alarm_config));

    //Test set bits
    printf("test set bits\r\n");
    test_set_bits = true;
    TEST_ESP_OK(gptimer_start(gptimer));
    TEST_ASSERT_EQUAL(BITS, xEventGroupWaitBits(eg, BITS, pdFALSE, pdTRUE, portMAX_DELAY));     //Let ISR set event group bits

    //Test clear bits
    printf("test clear bits\r\n");
    xEventGroupSetBits(eg, BITS);                   //Set bits to be cleared
    test_clear_bits = true;
    TEST_ESP_OK(gptimer_set_raw_count(gptimer, 0));
    TEST_ESP_OK(gptimer_start(gptimer));
    xSemaphoreTake(done_sem, portMAX_DELAY);        //Wait for ISR to clear bits
    vTaskDelay(10);                                 //Event group clear bits runs via daemon task, delay so daemon can run
    TEST_ASSERT_EQUAL(0, xEventGroupGetBits(eg));   //Check bits are cleared

    //Clean up
    TEST_ESP_OK(gptimer_disable(gptimer));
    TEST_ESP_OK(gptimer_del_timer(gptimer));
    vEventGroupDelete(eg);
    vSemaphoreDelete(done_sem);
    vTaskDelay(10);     //Give time for idle task to clear up deleted tasks
}
#endif      //CONFIG_FREERTOS_USE_TRACE_FACILITY
