/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"

#define PROMPT_STR CONFIG_IDF_TARGET
static struct {
    struct arg_int *cmd_params;
    struct arg_end *end;
} rf_testing_set_tx_power_cmd_args;

static int rf_testing_set_ble_tx_power_command(int argc, char **argv)
{
    esp_err_t ret = ESP_OK;
    int nerrors = arg_parse(argc, argv, (void **) &rf_testing_set_tx_power_cmd_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, rf_testing_set_tx_power_cmd_args.end, argv[0]);
        return 1;
    }

    ESP_LOGI(__func__, "Set tx power level '%d'", rf_testing_set_tx_power_cmd_args.cmd_params->ival[0]);
    if (rf_testing_set_tx_power_cmd_args.cmd_params->ival[0] > 15) {
        return 2;
    }

    ret = esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, rf_testing_set_tx_power_cmd_args.cmd_params->ival[0]);
    if (ret != ESP_OK) {
        return 3;
    }

    return 0;
}

static int rf_testing_get_ble_tx_power_command(int argc, char **argv)
{
    esp_power_level_t power_level = 0xFF;
    power_level = esp_ble_tx_power_get(ESP_BLE_PWR_TYPE_DEFAULT);
    printf("\nCurrent BLE TX power is %d level\n", power_level);
    return 0;
}


esp_err_t esp_console_register_set_ble_tx_power_command(void)
{
    rf_testing_set_tx_power_cmd_args.cmd_params = arg_int1("i", "index", "<index>","tx power level index");
    rf_testing_set_tx_power_cmd_args.end = arg_end(1);

    esp_console_cmd_t command = {
        .command = "set_ble_tx_power",
        .help = "Set default ble tx power",
        .func = &rf_testing_set_ble_tx_power_command,
        .argtable = &rf_testing_set_tx_power_cmd_args
    };

    return esp_console_cmd_register(&command);
}

esp_err_t esp_console_register_get_ble_tx_power_command(void)
{
    esp_console_cmd_t command = {
        .command = "get_ble_tx_power",
        .help = "Get ble tx power during RF test",
        .func = &rf_testing_get_ble_tx_power_command,
    };

    return esp_console_cmd_register(&command);
}


esp_err_t rf_testing_configuration_command_enable(void)
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
    esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &repl));
    ESP_ERROR_CHECK(esp_console_start_repl(repl));
    return ESP_OK;
}
