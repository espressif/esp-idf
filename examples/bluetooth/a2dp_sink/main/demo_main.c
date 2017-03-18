#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "bt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "nvs_flash.h"
#include "esp_system.h"

extern void bte_main_boot_entry(void *);
extern void bt_app_task_start_up(void);
extern void bt_app_core_start(void);

void app_main()
{
    nvs_flash_init();
    esp_bt_controller_init();

    if (esp_bt_controller_enable(ESP_BT_MODE_BTDM) != ESP_OK) {
        return;
    }
    bt_app_task_start_up();
}
