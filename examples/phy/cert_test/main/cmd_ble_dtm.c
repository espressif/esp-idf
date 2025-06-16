/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "esp_log.h"
#include "esp_bt.h"

#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "esp_hci_driver.h"

#define PROMPT_STR CONFIG_IDF_TARGET
static struct {
    struct arg_int *cmd_params;
    struct arg_end *end;
} dtm_set_tx_power_cmd_args;

static struct {
    struct arg_int *tx_pin;
    struct arg_int *rx_pin;
    struct arg_end *end;
} dtm_reconfig_uart_cmd_args;

static struct {
    struct arg_int *cmd_params;
    struct arg_end *end;
} dtm_enable_cmd_args;

static int dtm_set_ble_tx_power_command(int argc, char **argv)
{
    esp_err_t ret = ESP_OK;
    int nerrors = arg_parse(argc, argv, (void **) &dtm_set_tx_power_cmd_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, dtm_set_tx_power_cmd_args.end, argv[0]);
        return 1;
    }

    if (esp_bt_controller_get_status() != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        esp_rom_printf("\nPlease enable BLE DTM mode first by using the command enable_ble_dtm -e 1 before sending this command.\n");
        return 2;
    }

    ESP_LOGI(__func__, "Set tx power level '%d'", dtm_set_tx_power_cmd_args.cmd_params->ival[0]);
    if (dtm_set_tx_power_cmd_args.cmd_params->ival[0] > 15) {
        return 3;
    }

    ret = esp_ble_tx_power_set_enhanced(ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT, 0, dtm_set_tx_power_cmd_args.cmd_params->ival[0]);
    if (ret != ESP_OK) {
        return 4;
    }

    return 0;
}

static int dtm_get_ble_tx_power_command(int argc, char **argv)
{
    esp_power_level_t power_level = 0xFF;
    if (esp_bt_controller_get_status() != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        esp_rom_printf("\nPlease enable BLE DTM mode first by using the command enable_ble_dtm -e 1 before sending this command.\n");
        return 2;
    }

    power_level = esp_ble_tx_power_get_enhanced(ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT, 0);
    esp_rom_printf("\nCurrent BLE TX power is %d level\n", power_level);
    return 0;
}

static int dtm_reconfig_uart_pins_command(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &dtm_reconfig_uart_cmd_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, dtm_reconfig_uart_cmd_args.end, argv[0]);
        return 1;
    }

    if (esp_bt_controller_get_status() != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        esp_rom_printf("\nPlease enable BLE DTM mode first by using the command enable_ble_dtm -e 1 before sending this command.\n");
        return 2;
    }

    ESP_LOGI(__func__, "reconfig tx:'%d', rx: '%d'",
             dtm_reconfig_uart_cmd_args.tx_pin->ival[0], dtm_reconfig_uart_cmd_args.rx_pin->ival[0]);
    hci_uart_reconfig_pin(dtm_reconfig_uart_cmd_args.tx_pin->ival[0],
                          dtm_reconfig_uart_cmd_args.rx_pin->ival[0], -1, -1);
    return 0;
}

static int dtm_test_enable_command(int argc, char **argv)
{
    int dtm_enable = 0;
    esp_bt_controller_config_t config_opts = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    int nerrors = arg_parse(argc, argv, (void **) &dtm_enable_cmd_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, dtm_enable_cmd_args.end, argv[0]);
        return 1;
    }

    dtm_enable = dtm_enable_cmd_args.cmd_params->ival[0];
    ESP_LOGI(__func__, "Enable DTM Test '%d'", dtm_enable);
    if (dtm_enable > 2) {
        return 2;
    }

    if (dtm_enable) {
        if (esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_IDLE) {
            /* Initialize Bluetooth Controller parameters. */
            ESP_ERROR_CHECK(esp_bt_controller_init(&config_opts));

            /* Enable the task stack of the Bluetooth Controller. */
            ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));
        }
    } else {
        if (esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_ENABLED) {
            ESP_ERROR_CHECK(esp_bt_controller_disable());
            ESP_ERROR_CHECK(esp_bt_controller_deinit());
        }
    }

    return 0;
}

esp_err_t esp_console_register_set_ble_tx_power_command(void)
{
    dtm_set_tx_power_cmd_args.cmd_params = arg_int1("i", "index", "<index>","tx power level index");
    dtm_set_tx_power_cmd_args.end = arg_end(1);

    esp_console_cmd_t command = {
        .command = "set_ble_tx_power",
        .help = "Set ble tx power during DTM",
        .func = &dtm_set_ble_tx_power_command,
        .argtable = &dtm_set_tx_power_cmd_args
    };

    return esp_console_cmd_register(&command);
}

esp_err_t esp_console_register_get_ble_tx_power_command(void)
{
    esp_console_cmd_t command = {
        .command = "get_ble_tx_power",
        .help = "Get ble tx power during DTM",
        .func = &dtm_get_ble_tx_power_command,
    };

    return esp_console_cmd_register(&command);
}


esp_err_t esp_console_register_reconfig_dtm_pins_command(void)
{
    dtm_reconfig_uart_cmd_args.tx_pin = arg_int1("t", "tx", "<tx_pin>","tx pin index");
    dtm_reconfig_uart_cmd_args.rx_pin = arg_int1("r", "rx", "<rx_pin>","rx pin index");
    dtm_reconfig_uart_cmd_args.end = arg_end(2);

    esp_console_cmd_t command = {
        .command = "reconfig_dtm_uart_pin",
        .help = "Reconfig dtm uart pins during DTM",
        .func = &dtm_reconfig_uart_pins_command,
        .argtable = &dtm_reconfig_uart_cmd_args
    };

    return esp_console_cmd_register(&command);
}

esp_err_t esp_console_register_enable_ble_dtm_command(void)
{
    dtm_enable_cmd_args.cmd_params = arg_int1("e", "enable", "<enable>","enable/disable ble dtm test");
    dtm_enable_cmd_args.end = arg_end(1);

    esp_console_cmd_t command = {
        .command = "enable_ble_dtm",
        .help = "Enable BLE DTM Test",
        .func = &dtm_test_enable_command,
        .argtable = &dtm_enable_cmd_args
    };

    return esp_console_cmd_register(&command);
}

esp_err_t dtm_configuration_command_register(void)
{
    esp_console_register_set_ble_tx_power_command();
    esp_console_register_get_ble_tx_power_command();
    esp_console_register_reconfig_dtm_pins_command();
    esp_console_register_enable_ble_dtm_command();
    return ESP_OK;
}
