/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "esp_log.h"
#include "esp_vfs_dev.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_vfs_fat.h"
#include "esp_console.h"
#include "ble_mesh_console_decl.h"
#include "ble_mesh_example_init.h"

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
    esp_err_t err = esp_vfs_fat_spiflash_mount_rw_wl(MOUNT_PATH, "storage", &mount_config, &wl_handle);
    if (err != ESP_OK) {
        return;
    }
}
#endif // CONFIG_STORE_HISTORY

void app_main(void)
{
    esp_err_t res;

    nvs_flash_init();

    // init and enable bluetooth
    res = bluetooth_init();
    if (res) {
        printf("esp32_bluetooth_init failed (ret %d)", res);
    }

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("ble_mesh_console", ESP_LOG_INFO);

    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
#if CONFIG_STORE_HISTORY
    initialize_filesystem();
    repl_config.history_save_path = HISTORY_PATH;
#endif

#if CONFIG_IDF_TARGET_ESP32C3
    repl_config.prompt = "esp32c3>";
#elif CONFIG_IDF_TARGET_ESP32S3
    repl_config.prompt = "esp32s3>";
#else
    repl_config.prompt = "esp32>";
#endif

    // init console REPL environment
    repl_config.max_history_len = 1;
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &repl));

    /* Register commands */
    register_system();
    register_bluetooth();
    ble_mesh_register_mesh_node();
    ble_mesh_register_mesh_test_performance_client();
    ble_mesh_register_server();
#if (CONFIG_BLE_MESH_GENERIC_ONOFF_CLI)
    ble_mesh_register_gen_onoff_client();
#endif
#if (CONFIG_BLE_MESH_CFG_CLI)
    ble_mesh_register_configuration_client_model();
#endif
    printf("!!!ready!!!\n");
    // start console REPL
    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}
