/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "basic_freertos_smp_usage.h"


static QueueHandle_t msg_queue;
static const uint8_t msg_queue_len = 40;
static volatile bool timed_out;
const static char *TAG = "queue example";

static void print_q_msg(void *arg)
{
    int data;  // data type should be same as queue item type
    int to_wait_ms = 1000;  // the maximal blocking waiting time of millisecond
    const TickType_t xTicksToWait = pdMS_TO_TICKS(to_wait_ms);

    while (!timed_out) {
        if (xQueueReceive(msg_queue, (void *)&data, xTicksToWait) == pdTRUE) {
            ESP_LOGI(TAG, "received data = %d", data);
        } else {
            ESP_LOGI(TAG, "Did not received data in the past %d ms", to_wait_ms);
        }
    }

    vTaskDelete(NULL);
}

static void send_q_msg(void *arg)
{
    int sent_num  = 0;

    while (!timed_out) {
        // Try to add item to queue, fail immediately if queue is full
        if (xQueueGenericSend(msg_queue, (void *)&sent_num, portMAX_DELAY, queueSEND_TO_BACK) != pdTRUE) {
            ESP_LOGI(TAG, "Queue full\n");
        }
        ESP_LOGI(TAG, "sent data = %d", sent_num);
        sent_num++;

        // send an item for every 250ms
        vTaskDelay(250 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}

// Queue example: illustrate how queues can be used to synchronize between tasks
int comp_queue_entry_func(int argc, char **argv)
{
    timed_out = false;

    msg_queue = xQueueGenericCreate(msg_queue_len, sizeof(int), queueQUEUE_TYPE_SET);
    if (msg_queue == NULL) {
        ESP_LOGE(TAG, QUEUE_CREATE_ERR_STR);
        return 1;
    }
    xTaskCreatePinnedToCore(print_q_msg, "print_q_msg", 4096, NULL, TASK_PRIO_3, NULL, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(send_q_msg, "send_q_msg", 4096, NULL, TASK_PRIO_3, NULL, tskNO_AFFINITY);

    // time out and stop running after 5 seconds
    vTaskDelay(pdMS_TO_TICKS(COMP_LOOP_PERIOD));
    timed_out = true;
    // delay to let tasks finish the last loop
    vTaskDelay(500 / portTICK_PERIOD_MS);
    return 0;
}
