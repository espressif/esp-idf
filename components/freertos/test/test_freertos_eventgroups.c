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

static const int NUM_TASKS = 4;
static const int COUNT = 4000;
static EventGroupHandle_t eg;

static void task_event_group_call_response(void *param)
{
    int task_num = (int)param;

    printf("Started %d\n", task_num);

    for (int i = 0; i < COUNT; i++) {
        /* Wait until the common "call" bit is set, starts off all tasks
           (clear on return) */
        while (!xEventGroupWaitBits(eg, BIT_CALL, true, false, portMAX_DELAY)) {
        }

        /* Set our individual "response" bit */
        xEventGroupSetBits(eg, BIT_RESPONSE(task_num));
    }

    printf("Task %d done\n", task_num);

    /* Delay is due to not-yet-fixed bug with deleting tasks at same time */
    vTaskDelay(100 / portTICK_PERIOD_MS);
    vTaskDelete(NULL);
}

TEST_CASE("FreeRTOS Event Groups", "[freertos][ignore]")
{
    eg = xEventGroupCreate();

    /* Note: task_event_group_call_response all have higher priority than us, so will block together.

       This is important because we need to know they'll all have blocked on BIT_CALL each time we
       signal it, or they get out of sync.
     */
    for (int c = 0; c < NUM_TASKS; c++) {
        xTaskCreatePinnedToCore(task_event_group_call_response, "tsk_call_resp", 4096, (void *)c, configMAX_PRIORITIES - 1, NULL, c % portNUM_PROCESSORS);
    }
    /* Scheduler weirdness, if we don't sleep a few ticks here then the tasks on the other CPU aren't running yet... */
    vTaskDelay(10);

    for (int i = 0; i < COUNT; i++) {
        if (i % 100 == 0) {
            //printf("Call %d\n", i);
        }
        /* signal all tasks with "CALL" bit... */
        xEventGroupSetBits(eg, BIT_CALL);

        while (xEventGroupWaitBits(eg, ALL_RESPONSE_BITS, true, true, portMAX_DELAY) != ALL_RESPONSE_BITS) {
        }
    }
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

    printf("Done %d = %x\n", task_num, after_done);

    /* Delay is due to not-yet-fixed bug with deleting tasks at same time */
    vTaskDelay(100 / portTICK_PERIOD_MS);
    vTaskDelete(NULL);
}

TEST_CASE("FreeRTOS Event Group Sync", "[freertos][ignore]")
{
    eg = xEventGroupCreate();

    for (int c = 0; c < NUM_TASKS; c++) {
        xTaskCreatePinnedToCore(task_test_sync, "task_test_sync", 4096, (void *)c, configMAX_PRIORITIES - 1, NULL, c % portNUM_PROCESSORS);
    }

    for (int c = 0; c < NUM_TASKS; c++) {
        printf("Waiting on %d (%x)\n", c, BIT_DONE(c));
        xEventGroupWaitBits(eg, BIT_DONE(c), false, false, portMAX_DELAY);
    }
}

