#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "bt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_log.h"

#define A2DP_SINK_TAG    "A2DP_SINK"

extern void bte_main_boot_entry(void *);
extern void bt_app_task_start_up(void);
extern void bt_app_core_start(void);

void app_main()
{
    esp_err_t ret;
    nvs_flash_init();

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(A2DP_SINK_TAG, "%s initialize controller failed\n", __func__);
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BTDM);
    if (ret) {
        ESP_LOGE(A2DP_SINK_TAG, "%s enable controller failed\n", __func__);
        return;
    }
    
    bt_app_task_start_up();
}
