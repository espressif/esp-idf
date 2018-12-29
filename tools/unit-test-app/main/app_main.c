#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "unity_config.h"

void unityTask(void *pvParameters)
{
    vTaskDelay(2); /* Delay a bit to let the main task be deleted */
    unity_run_menu(); /* Doesn't return */
}

void app_main()
{
    // Note: if unpinning this task, change the way run times are calculated in
    // unity_platform
    xTaskCreatePinnedToCore(unityTask, "unityTask", UNITY_FREERTOS_STACK_SIZE, NULL,
                            UNITY_FREERTOS_PRIORITY, NULL, UNITY_FREERTOS_CPU);
}
