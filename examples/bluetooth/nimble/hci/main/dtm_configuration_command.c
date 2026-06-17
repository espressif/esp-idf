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

static int dtm_set_ble_tx_power_command(int argc, char **argv)
{
    esp_err_t ret = ESP_OK;
    int nerrors = arg_parse(argc, argv, (void **) &dtm_set_tx_power_cmd_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, dtm_set_tx_power_cmd_args.end, argv[0]);
        return 1;
    }

    ESP_LOGI(__func__, "Set tx power level '%d'", dtm_set_tx_power_cmd_args.cmd_params->ival[0]);
    if (dtm_set_tx_power_cmd_args.cmd_params->ival[0] > 15) {
        return 2;
    }

    ret = esp_ble_tx_power_set_enhanced(ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT, 0, dtm_set_tx_power_cmd_args.cmd_params->ival[0]);
    if (ret != ESP_OK) {
        return 3;
    }

    return 0;
}

static int dtm_get_ble_tx_power_command(int argc, char **argv)
{
    esp_power_level_t power_level;

    power_level = esp_ble_tx_power_get_enhanced(ESP_BLE_ENHANCED_PWR_TYPE_DEFAULT, 0);

    if (power_level == ESP_PWR_LVL_INVALID) {
        ESP_LOGI(__func__,"TX power is not available!\n");
        return 1;
    }

    ESP_LOGI(__func__,"\nCurrent BLE TX power is %d level\n", power_level);
    return 0;
}

static int dtm_reconfig_uart_pins_command(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &dtm_reconfig_uart_cmd_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, dtm_reconfig_uart_cmd_args.end, argv[0]);
        return 1;
    }

    ESP_LOGI(__func__, "reconfig tx:'%d', rx: '%d'",
             dtm_reconfig_uart_cmd_args.tx_pin->ival[0], dtm_reconfig_uart_cmd_args.rx_pin->ival[0]);
    hci_uart_reconfig_pin(dtm_reconfig_uart_cmd_args.tx_pin->ival[0],
                          dtm_reconfig_uart_cmd_args.rx_pin->ival[0], -1, -1);
    return 0;
}

extern int8_t esp_ble_get_dtm_rx_rssi(void);
static int dtm_get_ble_rx_rssi_command(int argc, char **argv)
{
    int8_t rx_rssi = 0x7F;
    if (esp_bt_controller_get_status() != ESP_BT_CONTROLLER_STATUS_ENABLED) {
        esp_rom_printf("\nPlease enable BLE DTM mode first by using the command enable_ble_dtm -e 1 before sending this command.\n");
        return 2;
    }

    rx_rssi = esp_ble_get_dtm_rx_rssi();
    if (rx_rssi == 0x7f) {
        esp_rom_printf("\nRx RSSI is not available!\n");
    } else {
        esp_rom_printf("\nRx RSSI is %d dBm\n", rx_rssi);
    }

    return 0;
}

esp_err_t esp_console_register_get_ble_rx_rssi_command(void)
{
    esp_console_cmd_t command = {
        .command = "get_ble_rx_rssi",
        .help = "Get ble rx rssi during DTM",
        .func = &dtm_get_ble_rx_rssi_command,
    };

    return esp_console_cmd_register(&command);
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

esp_err_t dtm_configuration_command_enable(void)
{
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    /* Prompt to be printed before each line.
     * This can be customized, made dynamic, etc.
     */
    repl_config.prompt = PROMPT_STR ">";
    repl_config.max_cmdline_length = 256;
    esp_console_register_set_ble_tx_power_command();
    esp_console_register_get_ble_tx_power_command();
    esp_console_register_reconfig_dtm_pins_command();
    esp_console_register_get_ble_rx_rssi_command();
    esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &repl));
    ESP_ERROR_CHECK(esp_console_start_repl(repl));
    return ESP_OK;
}
