/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#include "sdkconfig.h"

#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_trace.h"

static const char *TAG = "main";

esp_trace_open_params_t esp_trace_get_user_params(void)
{
    esp_trace_open_params_t trace_params = {
        .core_cfg = NULL,
        .encoder_name = "ext_trace_lib",
        .encoder_cfg = NULL,
        .transport_name = "usb_serial_jtag",
        .transport_cfg = NULL,
    };
    return trace_params;
}

static QueueHandle_t s_q;

/* Producer: sends a counter value to s_q every 50 ms. Generates Q_SEND
 * trace events and unblocks the consumer (driving TASK_IN switches). */
static void producer(void *arg)
{
    uint32_t v = 0;
    while (1) {
        xQueueSend(s_q, &v, portMAX_DELAY);
        v++;
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

/* Consumer: blocks on s_q indefinitely. Each receive wakes this task and
 * fires a TASK_IN trace event when the scheduler switches us in. */
static void consumer(void *arg)
{
    uint32_t v;
    while (1) {
        xQueueReceive(s_q, &v, portMAX_DELAY);
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "Start of trace session");

    // Wait some time for host to be ready
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    esp_trace_start();

    s_q = xQueueCreate(4, sizeof(uint32_t));

    xTaskCreatePinnedToCore(producer, "producer", 2048, NULL, 5, NULL, 0);
    xTaskCreatePinnedToCore(consumer, "consumer", 2048, NULL, 5, NULL, portNUM_PROCESSORS - 1);

    // 1 second delay is enough to generate the expected number of trace events.
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    esp_trace_stop();
    esp_trace_flush();

    ESP_LOGI(TAG, "End of trace session");
}
