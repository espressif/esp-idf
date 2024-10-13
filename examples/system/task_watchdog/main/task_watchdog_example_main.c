/* Task_Watchdog Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "sdkconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_task_wdt.h"

#define TWDT_TIMEOUT_MS         3000
#define TASK_RESET_PERIOD_MS    2000
#define MAIN_DELAY_MS           10000

static volatile bool run_loop;
static esp_task_wdt_user_handle_t func_a_twdt_user_hdl;
static esp_task_wdt_user_handle_t func_b_twdt_user_hdl;

static void func_a(void)
{
    esp_task_wdt_reset_user(func_a_twdt_user_hdl);
}

static void func_b(void)
{
    esp_task_wdt_reset_user(func_b_twdt_user_hdl);
}

void task_func(void *arg)
{
    // Subscribe this task to TWDT, then check if it is subscribed
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));
    ESP_ERROR_CHECK(esp_task_wdt_status(NULL));

    // Subscribe func_a and func_b as users of the the TWDT
    ESP_ERROR_CHECK(esp_task_wdt_add_user("func_a", &func_a_twdt_user_hdl));
    ESP_ERROR_CHECK(esp_task_wdt_add_user("func_b", &func_b_twdt_user_hdl));

    printf("Subscribed to TWDT\n");

    while (run_loop) {
        // Reset the task and each user periodically
        /*
        Note: Comment out any one of the calls below to trigger the TWDT
        */
        esp_task_wdt_reset();
        func_a();
        func_b();

        vTaskDelay(pdMS_TO_TICKS(TASK_RESET_PERIOD_MS));
    }

    // Unsubscribe this task, func_a, and func_b
    ESP_ERROR_CHECK(esp_task_wdt_delete_user(func_a_twdt_user_hdl));
    ESP_ERROR_CHECK(esp_task_wdt_delete_user(func_b_twdt_user_hdl));
    ESP_ERROR_CHECK(esp_task_wdt_delete(NULL));

    printf("Unsubscribed from TWDT\n");

    // Notify main task of deletion
    xTaskNotifyGive((TaskHandle_t)arg);
    vTaskDelete(NULL);
}

void app_main(void)
{
#if !CONFIG_ESP_TASK_WDT_INIT
    // If the TWDT was not initialized automatically on startup, manually intialize it now
    esp_task_wdt_config_t twdt_config = {
        .timeout_ms = TWDT_TIMEOUT_MS,
        .idle_core_mask = (1 << CONFIG_FREERTOS_NUMBER_OF_CORES) - 1,    // Bitmask of all cores
        .trigger_panic = false,
    };
    ESP_ERROR_CHECK(esp_task_wdt_init(&twdt_config));
    printf("TWDT initialized\n");
#endif // CONFIG_ESP_TASK_WDT_INIT

    // Create a task
    run_loop = true;
    xTaskCreatePinnedToCore(task_func, "task", 2048, xTaskGetCurrentTaskHandle(), 10, NULL, 0);

    // Let the created task run for a while
    printf("Delay for %d seconds\n", MAIN_DELAY_MS/1000);
    vTaskDelay(pdMS_TO_TICKS(MAIN_DELAY_MS));

    // Stop the created task
    run_loop = false;
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

#if !CONFIG_ESP_TASK_WDT_INIT
    // If we manually initialized the TWDT, deintialize it now
    ESP_ERROR_CHECK(esp_task_wdt_deinit());
    printf("TWDT deinitialized\n");
#endif // CONFIG_ESP_TASK_WDT_INIT
    printf("Example complete\n");
}
