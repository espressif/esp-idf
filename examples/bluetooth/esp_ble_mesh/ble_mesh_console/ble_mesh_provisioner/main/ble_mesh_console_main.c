// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "esp_console.h"
#include "esp_vfs_dev.h"

#include "esp_vfs_fat.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "esp_bt.h"
#include "esp_bt_main.h"

#include "ble_mesh_console_decl.h"

#define TAG "ble_mesh_test"

#if CONFIG_STORE_HISTORY

#define MOUNT_PATH "/data"
#define HISTORY_PATH MOUNT_PATH "/history.txt"

static void initialize_filesystem(void)
{
    static wl_handle_t wl_handle;
    const esp_vfs_fat_mount_config_t mount_config = {
        .max_files = 4,
        .format_if_mount_failed = true
    };
    esp_err_t err = esp_vfs_fat_spiflash_mount(MOUNT_PATH, "storage", &mount_config, &wl_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount FATFS (0x%x)", err);
        return;
    }
}
#endif // CONFIG_STORE_HISTORY

esp_err_t bluetooth_init(void)
{
    esp_err_t ret;

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(TAG, "%s failed to initialize controller\n", __func__);
        return ret;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(TAG, "%s failed to enable controller\n", __func__);
        return ret;
    }
    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(TAG, "%s failed to initialize bluetooth\n", __func__);
        return ret;
    }
    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(TAG, "%s failed to enable bluetooth\n", __func__);
        return ret;
    }

    esp_log_level_set("*", ESP_LOG_ERROR);
    esp_log_level_set("ble_mesh_prov_console", ESP_LOG_INFO);

    return ret;
}

void app_main(void)
{
    esp_err_t res;

    nvs_flash_init();

    // init and enable bluetooth
    res = bluetooth_init();
    if (res) {
        printf("esp32_bluetooth_init failed (ret %d)", res);
    }

    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    const char* prompt = LOG_COLOR_I "esp32> " LOG_RESET_COLOR;
    repl_config.prompt = prompt;
#if CONFIG_STORE_HISTORY
    initialize_filesystem();
    repl_config.his_save_path = HISTORY_PATH;
#endif
    // init console REPL environment
    ESP_ERROR_CHECK(esp_console_repl_init(&repl_config));

    /* Register commands */
    register_system();
    register_bluetooth();
    ble_mesh_register_mesh_node();
    ble_mesh_register_mesh_test_performance_client();
#if (CONFIG_BLE_MESH_GENERIC_ONOFF_CLI)
    ble_mesh_register_gen_onoff_client();
#endif
#if (CONFIG_BLE_MESH_PROVISIONER)
    ble_mesh_register_mesh_provisioner();
#endif
#if (CONFIG_BLE_MESH_CFG_CLI)
    ble_mesh_register_configuration_client_model();
#endif

    // start console REPL
    ESP_ERROR_CHECK(esp_console_repl_start());
}
