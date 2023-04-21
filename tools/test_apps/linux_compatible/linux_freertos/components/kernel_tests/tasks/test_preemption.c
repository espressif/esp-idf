/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 Unit tests for FreeRTOS preemption
*/

#include <esp_types.h>
#include <stdio.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/idf_additions.h"
#include "unity.h"
#include "sdkconfig.h"

static volatile bool trigger;
static volatile bool flag;

#ifndef CONFIG_FREERTOS_SMP
#define MAX_YIELD_COUNT 10000
#else
//TODO: IDF-5081
#define MAX_YIELD_COUNT 17000
#endif // CONFIG_FREERTOS_SMP


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

    ccount = 0;
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
        xTaskCreatePinnedToCore(task_send_to_queue, "YIELD", 2048, (void *)queue, CONFIG_UNITY_FREERTOS_PRIORITY - 1, &sender_task, CONFIG_UNITY_FREERTOS_CPU);

        vTaskDelay(1); /* make sure everything is set up */
        trigger = true;

        uint32_t yield_ccount;
        TEST_ASSERT( xQueueReceive(queue, &yield_ccount, 100 / portTICK_PERIOD_MS) );
        TEST_ASSERT( flag );

        vTaskDelete(sender_task);
        vQueueDelete(queue);
    }
}
