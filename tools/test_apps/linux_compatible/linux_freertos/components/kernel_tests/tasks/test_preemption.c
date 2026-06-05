/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
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

/*
 * Test: A lower-priority task must be able to run and send data to a queue
 * when the higher-priority task yields (blocks).
 *
 * Pattern: The lower-priority task blocks on a semaphore until told to proceed,
 * then sends to a queue. The higher-priority task gives the semaphore and
 * immediately blocks on the queue receive.
 */

static void task_send_to_queue(void *param)
{
    void **args = (void **)param;
    QueueHandle_t queue = (QueueHandle_t)args[0];
    SemaphoreHandle_t sem = (SemaphoreHandle_t)args[1];

    /* Block until the main task tells us to go */
    xSemaphoreTake(sem, portMAX_DELAY);

    uint32_t value = 42;
    xQueueSendToBack(queue, &value, 0);

    /* Stay alive until deleted */
    vTaskSuspend(NULL);
}

TEST_CASE("Yield from lower priority task, same CPU", "[freertos]")
{
    for (int i = 0; i < 3; i++) {
        TaskHandle_t sender_task;
        QueueHandle_t queue = xQueueCreate(1, sizeof(uint32_t));
        SemaphoreHandle_t sem = xSemaphoreCreateBinary();
        void *args[2] = { queue, sem };

        /* Lower-priority task blocks on the semaphore */
        xTaskCreatePinnedToCore(task_send_to_queue, "YIELD", 2048, args,
                                CONFIG_UNITY_FREERTOS_PRIORITY - 1, &sender_task,
                                CONFIG_UNITY_FREERTOS_CPU);

        /* Let the task start and block on the semaphore */
        vTaskDelay(pdMS_TO_TICKS(10));

        /* Give the semaphore — the lower-priority task won't run yet because
         * we (higher priority) are still ready. Then block on queue receive,
         * which yields the CPU to the lower-priority task. */
        xSemaphoreGive(sem);

        uint32_t received;
        TEST_ASSERT(xQueueReceive(queue, &received, pdMS_TO_TICKS(100)));
        TEST_ASSERT_EQUAL(42, received);

        vTaskDelete(sender_task);
        vQueueDelete(queue);
        vSemaphoreDelete(sem);
    }
}
