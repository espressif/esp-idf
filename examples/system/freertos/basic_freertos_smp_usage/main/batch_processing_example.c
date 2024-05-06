/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "basic_freertos_smp_usage.h"

#define DATA_BATCH_SIZE 5

// static TaskHandle_t proc_data_task_hdl;
static QueueHandle_t msg_queue;
static const uint8_t msg_queue_len = 10;
static SemaphoreHandle_t s_mutex;  // mutex to protect shared resource "s_rcv_item_num"
static volatile int s_rcv_item_num;  // received data item number
static volatile bool timed_out;
const static char *TAG = "batch processing example";


/* This example describes a realistic scenario where there are 2 tasks, one of them receives irregularly arrived external data,
and the other task is responsible for processing the received data items. For some reason, every 5 data items form a batch
and they are meant to be processed together. Once the receiving data obtains a data item, it will increment a global variable
named s_rcv_item_num by 1, then push the data into a queue, of which the maximal size is 10; when s_rcv_item_num is not less
than 5, the receiving thread sends a task notification to the processing thread, which is blocked waiting for this signal to
proceed. Processing thread dequeues the first 5 data items from the queue and process them, and finally decrease the s_rcv_item_num by 5.
Please refer to README.md for more details.
*/

static void rcv_data_task(void *arg)
{
    int random_delay_ms;
    int data;
    TaskHandle_t proc_data_task_hdl = (TaskHandle_t)arg;

    while (!timed_out) {
        // random delay to mimic this thread receives data irregularly
        data = rand() % 100;
        random_delay_ms = (rand() % 500 + 200);
        vTaskDelay(random_delay_ms / portTICK_PERIOD_MS);
        // increase receive item num by 1
        if (xSemaphoreTake(s_mutex, portMAX_DELAY) == pdTRUE) {
            s_rcv_item_num += 1;
            xSemaphoreGive(s_mutex);
        }
        // enqueue the received data
        (void)xQueueGenericSend(msg_queue, (void *)&data, portMAX_DELAY, queueSEND_TO_BACK);
        ESP_LOGI(TAG, "enqueue data = %d", data);

        // if s_rcv_item_num >= batch size, send task notification to proc thread to process them together
        if (s_rcv_item_num >= DATA_BATCH_SIZE) {
            xTaskNotifyGive(proc_data_task_hdl);
        }
    }

    vTaskDelete(NULL);
}

static void proc_data_task(void *arg)
{
    int rcv_data_buffer[DATA_BATCH_SIZE] ;
    int rcv_item_num;
    int data_idx;
    while (!timed_out) {
        // blocking wait for task notification
        while (ulTaskNotifyTake(pdFALSE, portMAX_DELAY)) {
            // every time this task receives notification, reset received data item number
            rcv_item_num = 0;
            for (data_idx = 0; data_idx < DATA_BATCH_SIZE; data_idx++) {
                // keep reading message queue until it's empty
                if (xQueueReceive(msg_queue, (void *)&rcv_data_buffer[data_idx], 0) == pdTRUE) {
                    ESP_LOGI(TAG, "dequeue data = %d", rcv_data_buffer[data_idx]);
                    rcv_item_num += 1;
                } else {
                    break;
                }
            }

            // mimic to process the data in buffer and then clean it
            for (data_idx = 0; data_idx < rcv_item_num; data_idx++) {
                rcv_data_buffer[data_idx] = 0;
            }

            // decrease the s_rcv_item_num by batch size if it's not less the batch size, else set it as 0
            if (xSemaphoreTake(s_mutex, portMAX_DELAY) == pdTRUE) {
                s_rcv_item_num -= rcv_item_num;
                xSemaphoreGive(s_mutex);
                ESP_LOGI(TAG, "decrease s_rcv_item_num to %d", s_rcv_item_num);
            }
        }
    }

    vTaskDelete(NULL);
}

// batch processing example: demonstrate how to use task notification to implement batch processing
// use queue to transmit data between tasks, and use mutex to protect a shared global number
int comp_batch_proc_example_entry_func(int argc, char **argv)
{
    timed_out = false;

    s_mutex = xSemaphoreCreateMutex();
    if (s_mutex == NULL) {
        ESP_LOGE(TAG, SEM_CREATE_ERR_STR);
        return 1;
    }
    msg_queue = xQueueGenericCreate(msg_queue_len, sizeof(int), queueQUEUE_TYPE_SET);
    if (msg_queue == NULL) {
        ESP_LOGE(TAG, QUEUE_CREATE_ERR_STR);
        return 1;
    }
    TaskHandle_t proc_data_task_hdl;
    xTaskCreatePinnedToCore(proc_data_task, "proc_data_task", 4096, NULL, TASK_PRIO_3, &proc_data_task_hdl, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(rcv_data_task, "rcv_data_task", 4096, proc_data_task_hdl, TASK_PRIO_3, NULL, tskNO_AFFINITY);

    // time out and stop running after COMP_LOOP_PERIOD milliseconds
    vTaskDelay(pdMS_TO_TICKS(COMP_LOOP_PERIOD));
    timed_out = true;
    // delay to let tasks finish the last loop
    vTaskDelay(1500 / portTICK_PERIOD_MS);
    return 0;
}
