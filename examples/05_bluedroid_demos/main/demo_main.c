#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


extern void bte_main_boot_entry(void *);
extern void bt_app_task_start_up(void);
extern void bt_app_core_start(void);

void pingTask(void *pvParameters)
{
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        printf("ping\n");
    }
}

void bt_app_main()
{
    xTaskCreatePinnedToCore(&pingTask, "pingTask", 2048, NULL, 5, NULL, 0);
    bt_app_task_start_up();
    bte_main_boot_entry(bt_app_core_start);
}
