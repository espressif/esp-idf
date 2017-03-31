#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"


void unityTask(void *pvParameters) 
{
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    unity_run_menu();
    while(1);
}

void app_main()
{
    // Note: if unpinning this task, change the way run times are calculated in
    // unity_platform
    xTaskCreatePinnedToCore(unityTask, "unityTask", 4096, NULL, 5, NULL, 0);
}

