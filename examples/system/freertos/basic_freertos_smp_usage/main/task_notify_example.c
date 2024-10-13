/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "basic_freertos_smp_usage.h"

static volatile bool timed_out;
const static char *TAG = "task notify example";

/* In this example, there is a thread waiting for a synchronization signal from another thread before it start processing
Task synchronization could also be achieved with `xSemaphoreTake`, but FreeRTOS suggest using task notifications
as a faster and more lightweight alternative.
*/

static void notification_rcv_func(void *arg)
{
    int pending_notification_task_num;
    while (!timed_out) {
        pending_notification_task_num = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        {
            ESP_LOGI(TAG, "%d tasks pending", pending_notification_task_num);
            while (pending_notification_task_num > 0) {
                // do something to process the received notification
                ESP_LOGI(TAG, "rcv_task is processing this task notification");
                pending_notification_task_num--;
            }
        }
    }

    vTaskDelete(NULL);
}

static void notification_send_func(void *arg)
{
    TaskHandle_t rcv_task_hdl = (TaskHandle_t)arg;
    // send a task notification every 1000 ms
    while (!timed_out) {
        xTaskNotifyGive(rcv_task_hdl);
        ESP_LOGI(TAG, "send_task sends a notification");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}

int comp_task_notification_entry_func(int argc, char **argv)
{
    timed_out = false;
    TaskHandle_t rcv_task_hdl;
    xTaskCreatePinnedToCore(notification_rcv_func, NULL, 8192, NULL, TASK_PRIO_3, &rcv_task_hdl, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(notification_send_func, NULL, 8192, rcv_task_hdl, TASK_PRIO_3, NULL, tskNO_AFFINITY);

    // time out and stop running after 5 seconds
    vTaskDelay(pdMS_TO_TICKS(COMP_LOOP_PERIOD));
    timed_out = true;
    // delay to let tasks finish the last loop
    vTaskDelay(500 / portTICK_PERIOD_MS);
    return 0;
}
