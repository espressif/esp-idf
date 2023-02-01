/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* Application Trace Basic Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_app_trace.h"
#include "esp_log.h"

static const char *TAG = "example";

void app_main(void)
{
    ESP_LOGI(TAG, "Waiting for OpenOCD connection");

    while (!esp_apptrace_host_is_connected(ESP_APPTRACE_DEST_JTAG)) {
        vTaskDelay(1);
    }

    ESP_LOGI(TAG, "Sending example data to the host...");

    for (unsigned int cnt = 1; cnt < 51; ++cnt) {
        char buf[32] = {0};
        snprintf(buf, sizeof(buf), "Apptrace test data[%d]:%d\n", cnt, cnt * cnt);
        esp_err_t res = esp_apptrace_write(ESP_APPTRACE_DEST_JTAG, buf, strlen(buf), ESP_APPTRACE_TMO_INFINITE);
        if (res != ESP_OK) {
            ESP_LOGE(TAG, "Failed to write data to host [0x%x] (%s)", res, esp_err_to_name(res));
        }
        esp_apptrace_flush(ESP_APPTRACE_DEST_JTAG, 1000);
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }

    ESP_LOGI(TAG, "Done!");
}
