/* Task_Watchdog Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_task_wdt.h"

#ifndef CONFIG_TASK_WDT_LEGACY_BEHAVIOR

#define TIMEOUT 4000
#define TASK_PERIOD 2000

#define ASSERT_PRINT_ERROR(cond, str)    ({                                 \
            if(!(cond)){                                                    \
                printf("%s\n",str);                                         \
            }                                                               \
})

void feeding_task(void *arg){
    int core = xPortGetCoreID();
    //Simple task that periodically feeds watchdog
    while(1){
        if(esp_task_wdt_feed() == ESP_OK){
            printf("Feed Task %d\n", core);
        }
        vTaskDelay(pdMS_TO_TICKS(TASK_PERIOD));
    }
}

void app_main(void)
{
    //Initialize task watchdog
    if(esp_task_wdt_init(TIMEOUT, false) != ESP_OK){
        printf("Error\n");
    }

    //Create tasks
    TaskHandle_t handle_0;
    TaskHandle_t handle_1;
    xTaskCreatePinnedToCore(feeding_task, "Feed Task 0", 2000, NULL, 15, &handle_0, 0);
    xTaskCreatePinnedToCore(feeding_task, "Feed Task 1", 2000, NULL, 15, &handle_1, 1);

    //Add tasks to wdt
    ASSERT_PRINT_ERROR(esp_task_wdt_add(handle_0) == ESP_OK, "Add failed");
    ASSERT_PRINT_ERROR(esp_task_wdt_add(handle_1) == ESP_OK, "Add failed");

#ifndef CONFIG_TASK_WDT_CHECK_IDLE_TASK_CPU0
    //Adding Idles. Idle hook will call feed automatically
    TaskHandle_t idle_0 = xTaskGetIdleTaskHandleForCPU(0);
    ASSERT_PRINT_ERROR(idle_0 != NULL, "Get Idle handle failed");
    ASSERT_PRINT_ERROR(esp_task_wdt_add(idle_0) == ESP_OK, "Add failed");
#endif

#ifndef CONFIG_TASK_WDT_CHECK_IDLE_TASK_CPU1
    TaskHandle_t idle_1 = xTaskGetIdleTaskHandleForCPU(1);
    ASSERT_PRINT_ERROR(idle_1 != NULL, "Get Idle handle failed");
    ASSERT_PRINT_ERROR(esp_task_wdt_add(idle_1) == ESP_OK, "Add failed");
#endif

    //Wait 10 seconds
    vTaskDelay(pdMS_TO_TICKS(10000));

    //Remove tasks form wdt
    ASSERT_PRINT_ERROR(esp_task_wdt_delete(handle_0) == ESP_OK, "Failed to delete");
    ASSERT_PRINT_ERROR(esp_task_wdt_delete(handle_1) == ESP_OK, "Failed to delete");
    //Delete tasks
    vTaskDelete(handle_0);
    vTaskDelete(handle_1);

#ifndef CONFIG_TASK_WDT_CHECK_IDLE_TASK_CPU0
    ASSERT_PRINT_ERROR(esp_task_wdt_delete(idle_0) == ESP_OK, "Failed to delete");
#endif

#ifndef CONFIG_TASK_WDT_CHECK_IDLE_TASK_CPU1
    ASSERT_PRINT_ERROR(esp_task_wdt_delete(idle_1) == ESP_OK, "Failed to delete");
#endif


#ifndef CONFIG_TASK_WDT_CHECK_IDLE_TASK_CPU0
#ifndef CONFIG_TASK_WDT_CHECK_IDLE_TASK_CPU1
    //Deinit task watchdog. Can only do so once all tasks (including idle) are deleted from wdt
    ASSERT_PRINT_ERROR(esp_task_wdt_deinit() == ESP_OK, "deinit failed");
#endif
#endif

    printf("Complete\n");

}

#endif
