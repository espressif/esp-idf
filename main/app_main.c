#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


void pingTask(void *pvParameters)
{
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        printf("ping\n");
    }
}

void app_main()
{
    xTaskCreatePinnedToCore(&pingTask, "pingTask", 2048, NULL, 5, NULL, 0);
}
