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


static const char *TAG = "gdbstub_example";
esp_log_level_t log_level = ESP_LOG_DEBUG;
esp_log_level_t update_log_level = ESP_LOG_DEBUG;

int call_counts[4] = {0};
void task4test(void *param)
{
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    int task_num = (int)param;
    volatile int core = xPortGetCoreID();
    while (1) {
        core = xPortGetCoreID();
        printf("CPU %i: To enter GDB please press \"Ctrl+C\"\n", core);
        ESP_LOGD(TAG, "DEBUG mode enabled. Task %i, Core ID %i, Call - %i.", task_num, core, call_counts[task_num]);
        ESP_LOGI(TAG, "INFO  mode enabled. Task %i, Core ID %i, Call - %i.", task_num, core, call_counts[task_num]);
        ESP_LOGW(TAG, "WARN  mode enabled. Task %i, Core ID %i, Call - %i.", task_num, core, call_counts[task_num]++);

        asm("test_point1:   nop;");
        for (int i = 0 ; i < 10000000; i++) {
            asm("test_point2:   nop;");
            asm("               nop;");
            asm("               nop;");
            asm("               nop;");
        }
        log_level = update_log_level;
        esp_log_level_set(TAG, log_level);
    }
}

void app_main(void)
{
    printf("Hello GDB example!\n");

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    xTaskCreatePinnedToCore(task4test, "task 3", 4096, (void *)0, 10, NULL, 0);
    xTaskCreatePinnedToCore(task4test, "task 4", 4096, (void *)1, 10, NULL, 1);
}
