/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"


int call_count = 0;

static const char *TAG = "gdbstub_example";
esp_log_level_t log_level = ESP_LOG_DEBUG;
esp_log_level_t update_log_level = ESP_LOG_DEBUG;

void app_main(void)
{
    printf("Hello GDB example!\n");

    call_count = 0;

    for (;;) {
        ESP_LOGD(TAG, "DEBUG mode enabled. Call - %i. To enter GDB please press \"Ctrl+C\"", call_count);
        ESP_LOGI(TAG, "INFO  mode enabled. Call - %i. To enter GDB please press \"Ctrl+C\"", call_count);
        ESP_LOGW(TAG, "WARN  mode enabled. Call - %i. To enter GDB please press \"Ctrl+C\"", call_count++);
        if (update_log_level != log_level) {
            log_level = update_log_level;
            esp_log_level_set(TAG, log_level);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
