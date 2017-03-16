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


static volatile bool flag;

static void task_yield(void *param)
{
    QueueHandle_t queue = (QueueHandle_t) param;
    uint32_t ccount;
    RSR(CCOUNT, ccount);
    flag = true;
    xQueueSendToBack(queue, &ccount, 0);
    /* This is to ensure that higher priority task
       won't wake anyhow, due to this task terminating.
    */
    for (int i = 0; i < 1000; i++) {
        ets_delay_us(1000);
    }
    vTaskDelete(NULL);
}

TEST_CASE("Yield from lower priority task, same CPU", "[freertos]")
{
    /* Do this 3 times, mostly for the benchmark value - the first
       run includes a cache miss so uses more cycles than it should. */
    for (int i = 0; i < 3; i++) {
        QueueHandle_t queue = xQueueCreate(1, sizeof(uint32_t));
        flag = false;

        /* "yield" task sits on our CPU, lower priority to us */
        xTaskCreatePinnedToCore(task_yield, "YIELD", 2048, (void *)queue, UNITY_FREERTOS_PRIORITY - 1, NULL, UNITY_FREERTOS_CPU);

        uint32_t yield_ccount, now_ccount, delta;
        TEST_ASSERT( xQueueReceive(queue, &yield_ccount, 100 / portTICK_PERIOD_MS) );
        RSR(CCOUNT, now_ccount);
        TEST_ASSERT( flag );

        delta = now_ccount - yield_ccount;
        printf("Yielding from lower priority task took %u cycles\n", delta);
        TEST_ASSERT(delta < 10000);

        vQueueDelete(queue);
    }
}
