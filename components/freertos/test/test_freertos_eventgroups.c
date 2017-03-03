#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "unity.h"

#define BIT_CALL (1 << 0)
#define BIT_RESPONSE(TASK) (1 << (TASK+1))
#define ALL_RESPONSE_BITS (((1 << NUM_TASKS) - 1) << 1)

static const int NUM_TASKS = 8;
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
        TEST_ASSERT( xEventGroupWaitBits(eg, BIT_CALL, true, false, portMAX_DELAY) );

        /* Set our individual "response" bit */
        xEventGroupSetBits(eg, BIT_RESPONSE(task_num));
    }

    printf("Task %d done\n", task_num);
    xSemaphoreGive(done_sem);
    vTaskDelete(NULL);
}

TEST_CASE("FreeRTOS Event Groups", "[freertos]")
{
    eg = xEventGroupCreate();
    done_sem = xSemaphoreCreateCounting(NUM_TASKS, 0);

    /* Note: task_event_group_call_response all have higher priority than this task, so on this core
       they will always preempt this task.

       This is important because we need to know all tasks have blocked on BIT_CALL each time we signal it,
       or they get out of sync.
     */
    for (int c = 0; c < NUM_TASKS; c++) {
        xTaskCreatePinnedToCore(task_event_group_call_response, "tsk_call_resp", 4096, (void *)c, configMAX_PRIORITIES - 1, NULL, c % portNUM_PROCESSORS);
    }

    /* Tasks all start instantly, but this task will resume running at the same time as the higher priority tasks on the
       other processor may still be setting up, so give a tick for them to also block on BIT_CALL... */
    vTaskDelay(1);

    for (int i = 0; i < COUNT; i++) {
        /* signal all tasks with "CALL" bit... */
        xEventGroupSetBits(eg, BIT_CALL);

        TEST_ASSERT_EQUAL_HEX16(ALL_RESPONSE_BITS, xEventGroupWaitBits(eg, ALL_RESPONSE_BITS, true, true, 100 / portMAX_DELAY));
    }

    /* Ensure all tasks cleaned up correctly */
    for (int c = 0; c < NUM_TASKS; c++) {
        TEST_ASSERT( xSemaphoreTake(done_sem, 100/portTICK_PERIOD_MS) );
    }

    vSemaphoreDelete(done_sem);
    vEventGroupDelete(eg);
}

#define BIT_DONE(X) (1<<(NUM_TASKS+1+X))

static void task_test_sync(void *param)
{
    int task_num = (int)param;

    printf("Started %d\n", task_num);

    for (int i = 0; i < COUNT; i++) {
        /* set our bit, and wait on all tasks to set their bits */
        xEventGroupSync(eg, BIT_RESPONSE(task_num), ALL_RESPONSE_BITS, portMAX_DELAY);
        /* clear our bit */
        xEventGroupClearBits(eg, BIT_RESPONSE(task_num));
    }
    int after_done = xEventGroupSetBits(eg, BIT_DONE(task_num));

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
        printf("Waiting on %d (0x%08x)\n", c, BIT_DONE(c));
        TEST_ASSERT( xEventGroupWaitBits(eg, BIT_DONE(c), false, false, portMAX_DELAY) );
    }

    /* Ensure all tasks cleaned up correctly */
    for (int c = 0; c < NUM_TASKS; c++) {
        TEST_ASSERT( xSemaphoreTake(done_sem, 100/portTICK_PERIOD_MS) );
    }

    vSemaphoreDelete(done_sem);
    vEventGroupDelete(eg);
}

