/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"

#include "ble_dtm_config.h"

#if CONFIG_IDF_TARGET_ESP32
#if !CONFIG_BTDM_CTRL_HCI_MODE_UART_H4
#error "Please enable HCI UART(H4) in menuconfig"
#endif
#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3
#if !CONFIG_BT_CTRL_HCI_MODE_UART_H4
#error "Please enable HCI UART(H4) in menuconfig"
#endif
#elif CONFIG_SOC_ESP_NIMBLE_CONTROLLER
#if !CONFIG_BT_LE_HCI_INTERFACE_USE_UART && !CONFIG_BT_CTRL_HCI_INTERFACE_USE_UART
#error "Please enable UART for HCI in menuconfig"
#endif
#endif

#define TAG "BLE_DTM_CONFIG"

static struct {
    struct arg_int *cmd_params;
    struct arg_end *end;
} dtm_set_tx_power_cmd_args;

static int dtm_set_ble_tx_power_command(int argc, char **argv)
{
    esp_err_t ret = ESP_OK;
    int nerrors = arg_parse(argc, argv, (void **) &dtm_set_tx_power_cmd_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, dtm_set_tx_power_cmd_args.end, argv[0]);
        return 1;
    }

    ESP_LOGI(TAG, "Set tx power level '%d'", dtm_set_tx_power_cmd_args.cmd_params->ival[0]);
    if (dtm_set_tx_power_cmd_args.cmd_params->ival[0] < 0 || dtm_set_tx_power_cmd_args.cmd_params->ival[0] > 15) {
        return 2;
    }

#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3
    ret = esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, dtm_set_tx_power_cmd_args.cmd_params->ival[0]);
#else
    ret = esp_ble_tx_power_set_enhanced(ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT, 0, dtm_set_tx_power_cmd_args.cmd_params->ival[0]);
#endif
    if (ret != ESP_OK) {
        return 3;
    }

    return 0;
}

static int dtm_get_ble_tx_power_command(int argc, char **argv)
{
    esp_power_level_t power_level;

    if (esp_bt_controller_get_status() != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        ESP_LOGE(TAG, "Please enable the Bluetooth controller before sending this command.");
        return 2;
    }

#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3
    power_level = esp_ble_tx_power_get(ESP_BLE_PWR_TYPE_DEFAULT);
#else
    power_level = esp_ble_tx_power_get_enhanced(ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT, 0);
#endif
    if (power_level == ESP_PWR_LVL_INVALID) {
        ESP_LOGI(TAG, "TX power is not available!");
        return 1;
    }
    ESP_LOGI(TAG, "Current BLE TX power is %d level", power_level);
    return 0;
}

#if !CONFIG_IDF_TARGET_ESP32C3 && !CONFIG_IDF_TARGET_ESP32S3
extern int8_t esp_ble_get_dtm_rx_rssi(void);

static int dtm_get_ble_rx_rssi_command(int argc, char **argv)
{
    int8_t rx_rssi = 0x7F;
    if (esp_bt_controller_get_status() != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        ESP_LOGE(TAG, "Please enable BLE DTM mode first before sending this command.");
        return 2;
    }

    rx_rssi = esp_ble_get_dtm_rx_rssi();
    if (rx_rssi == 0x7f) {
        ESP_LOGI(TAG, "Rx RSSI is not available!");
    } else {
        ESP_LOGI(TAG, "Rx RSSI is %d dBm", rx_rssi);
    }

    return 0;
}

static esp_err_t esp_console_register_get_ble_rx_rssi_command(void)
{
    esp_console_cmd_t command = {
        .command = "get_ble_rx_rssi",
        .help = "Get ble rx rssi during DTM",
        .func = &dtm_get_ble_rx_rssi_command,
    };

    return esp_console_cmd_register(&command);
}
#endif

static esp_err_t esp_console_register_set_ble_tx_power_command(void)
{
    dtm_set_tx_power_cmd_args.cmd_params = arg_int1("i", "index", "<index>", "tx power level index");
    dtm_set_tx_power_cmd_args.end = arg_end(1);

    esp_console_cmd_t command = {
        .command = "set_ble_tx_power",
        .help = "Set ble tx power during DTM",
        .func = &dtm_set_ble_tx_power_command,
        .argtable = &dtm_set_tx_power_cmd_args
    };

    return esp_console_cmd_register(&command);
}

static esp_err_t esp_console_register_get_ble_tx_power_command(void)
{
    esp_console_cmd_t command = {
        .command = "get_ble_tx_power",
        .help = "Get ble tx power during DTM",
        .func = &dtm_get_ble_tx_power_command,
    };

    return esp_console_cmd_register(&command);
}

esp_err_t ble_dtm_config_register_commands(void)
{
    esp_console_register_set_ble_tx_power_command();
    esp_console_register_get_ble_tx_power_command();
#if !CONFIG_IDF_TARGET_ESP32C3 && !CONFIG_IDF_TARGET_ESP32S3
    esp_console_register_get_ble_rx_rssi_command();
#endif
    return ESP_OK;
}
