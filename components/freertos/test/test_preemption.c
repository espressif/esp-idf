/*
 Unit tests for FreeRTOS preemption
*/

#include <esp_types.h>
#include <stdio.h>
#include "rom/ets_sys.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"
#include "unity.h"
#include "soc/cpu.h"

static volatile bool trigger;
static volatile bool flag;

/* Task:
   - Waits for 'trigger' variable to be set
   - Reads the cycle count on this CPU
   - Pushes it into a queue supplied as a param
   - Busy-waits until the main task terminates it
*/
static void task_send_to_queue(void *param)
{
    QueueHandle_t queue = (QueueHandle_t) param;
    uint32_t ccount;

    while(!trigger) {}

    RSR(CCOUNT, ccount);
    flag = true;
    xQueueSendToBack(queue, &ccount, 0);
    /* This is to ensure that higher priority task
       won't wake anyhow, due to this task terminating.

       The task runs until terminated by the main task.
    */
    while(1) {}
}

TEST_CASE("Yield from lower priority task, same CPU", "[freertos]")
{
    /* Do this 3 times, mostly for the benchmark value - the first
       run includes a cache miss so uses more cycles than it should. */
    for (int i = 0; i < 3; i++) {
        TaskHandle_t sender_task;
        QueueHandle_t queue = xQueueCreate(1, sizeof(uint32_t));
        flag = false;
        trigger = false;

        /* "yield" task sits on our CPU, lower priority to us */
        xTaskCreatePinnedToCore(task_send_to_queue, "YIELD", 2048, (void *)queue, UNITY_FREERTOS_PRIORITY - 1, &sender_task, UNITY_FREERTOS_CPU);

        vTaskDelay(1); /* make sure everything is set up */
        trigger = true;

        uint32_t yield_ccount, now_ccount, delta;
        TEST_ASSERT( xQueueReceive(queue, &yield_ccount, 100 / portTICK_PERIOD_MS) );
        RSR(CCOUNT, now_ccount);
        TEST_ASSERT( flag );

        delta = now_ccount - yield_ccount;
        printf("Yielding from lower priority task took %u cycles\n", delta);
        TEST_ASSERT(delta < 10000);

        vTaskDelete(sender_task);
        vQueueDelete(queue);
    }
}


#if portNUM_PROCESSORS == 2
TEST_CASE("Yield from lower priority task, other CPU", "[freertos]")
{
    uint32_t trigger_ccount, yield_ccount, now_ccount, delta;

    /* Do this 3 times, mostly for the benchmark value - the first
       run includes a cache miss so uses more cycles than it should. */
    for (int i = 0; i < 3; i++) {
        TaskHandle_t sender_task;
        QueueHandle_t queue = xQueueCreate(1, sizeof(uint32_t));
        trigger = false;
        flag = false;

        /* "send_to_queue" task sits on the other CPU, lower priority to us */
        xTaskCreatePinnedToCore(task_send_to_queue, "YIELD", 2048, (void *)queue, UNITY_FREERTOS_PRIORITY - 1,
                                &sender_task, !UNITY_FREERTOS_CPU);

        vTaskDelay(2); /* make sure everything is set up */
        trigger = true;
        RSR(CCOUNT, trigger_ccount);

        // yield_ccount is not useful in this test as it's the other core's CCOUNT
        // so we use trigger_ccount instead
        TEST_ASSERT( xQueueReceive(queue, &yield_ccount, 100 / portTICK_PERIOD_MS) );
        RSR(CCOUNT, now_ccount);
        TEST_ASSERT( flag );

        delta = now_ccount - trigger_ccount;
        printf("Yielding from task on other core took %u cycles\n", delta);
        TEST_ASSERT(delta < 10000);

        vQueueDelete(queue);
        vTaskDelete(sender_task);
    }
}
#endif // portNUM_PROCESSORS == 2
