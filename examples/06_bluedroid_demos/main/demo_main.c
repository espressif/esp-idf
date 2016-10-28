#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "bt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "string.h"


extern void bte_main_boot_entry(void *);
extern void bt_app_task_start_up(void);
extern void bt_app_core_start(void);

void pingTask(void *pvParameters)
{
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        //printf("ping\n");
    }
}

void app_main()
{
    bt_controller_init();
    xTaskCreatePinnedToCore(&pingTask, "pingTask", 2048, NULL, 5, NULL, 0);
    bt_app_task_start_up();
    bte_main_boot_entry(bt_app_core_start);
}
