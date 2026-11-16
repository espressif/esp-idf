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

#include "hci_common.h"
#if CONFIG_ENABLE_BLE_DTM_CONFIGURATION_COMMAND
#include "ble_dtm_config.h"
#endif

#define PROMPT_STR CONFIG_IDF_TARGET

#define TAG "HCI_CONSOLE"

/*
 * Reconfiguring the HCI UART pins goes through a different API depending on which
 * component owns the HCI UART on this target. Targets that cannot change the pins at
 * runtime, such as ESP32, do not register the command at all.
 */
#if (CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3) && CONFIG_BT_CTRL_HCI_MODE_UART_H4 && CONFIG_BT_CTRL_HCI_UART_INIT_BY_CONTROLLER
#define HCI_CONSOLE_PIN_RECONFIG_BT_CTRL 1
#define HCI_CONSOLE_PIN_RECONFIG         1
#elif CONFIG_BT_LE_HCI_INTERFACE_USE_UART || CONFIG_BT_CTRL_HCI_INTERFACE_USE_UART
#include "esp_hci_driver.h"
#define HCI_CONSOLE_PIN_RECONFIG_BT_CTRL 0
#define HCI_CONSOLE_PIN_RECONFIG         1
#else // esp32
#define HCI_CONSOLE_PIN_RECONFIG_BT_CTRL 0
#define HCI_CONSOLE_PIN_RECONFIG         0
#endif

#if HCI_CONSOLE_PIN_RECONFIG

static int hci_console_set_uart_pin(int tx_pin, int rx_pin)
{
#if HCI_CONSOLE_PIN_RECONFIG_BT_CTRL
    return esp_bt_hci_uart_reconfig_pin(tx_pin, rx_pin);
#else
    return hci_uart_reconfig_pin(tx_pin, rx_pin, -1, -1);
#endif
}

static struct {
    struct arg_int *tx_pin;
    struct arg_int *rx_pin;
    struct arg_end *end;
} hci_reconfig_uart_cmd_args;

static int hci_reconfig_uart_pins_command(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &hci_reconfig_uart_cmd_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, hci_reconfig_uart_cmd_args.end, argv[0]);
        return 1;
    }

    int tx_pin = hci_reconfig_uart_cmd_args.tx_pin->ival[0];
    int rx_pin = hci_reconfig_uart_cmd_args.rx_pin->ival[0];
    if (tx_pin < 0 || rx_pin < 0) {
        ESP_LOGE(TAG, "Invalid GPIO pin: tx=%d, rx=%d", tx_pin, rx_pin);
        return 1;
    }
    ESP_LOGI(TAG, "reconfig tx:'%d', rx: '%d'", tx_pin, rx_pin);

    int rc = hci_console_set_uart_pin(tx_pin, rx_pin);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to reconfig UART pins; rc=%d", rc);
        return 1;
    }

    return 0;
}

static esp_err_t esp_console_register_reconfig_uart_pin_command(void)
{
    hci_reconfig_uart_cmd_args.tx_pin = arg_int1("t", "tx", "<tx_pin>", "tx pin index");
    hci_reconfig_uart_cmd_args.rx_pin = arg_int1("r", "rx", "<rx_pin>", "rx pin index");
    hci_reconfig_uart_cmd_args.end = arg_end(2);

    esp_console_cmd_t command = {
        .command = "reconfig_hci_uart_pin",
        .help = "Reconfig the HCI UART TX/RX pins",
        .func = &hci_reconfig_uart_pins_command,
        .argtable = &hci_reconfig_uart_cmd_args
    };

    return esp_console_cmd_register(&command);
}

#else /* !HCI_CONSOLE_PIN_RECONFIG */

static esp_err_t esp_console_register_reconfig_uart_pin_command(void)
{
    return ESP_OK;
}

#endif /* HCI_CONSOLE_PIN_RECONFIG */

/**
 * @brief Register the command group shared by BLE and BR/EDR, e.g. HCI UART control.
 */
static esp_err_t hci_common_register_commands(void)
{
    return esp_console_register_reconfig_uart_pin_command();
}

esp_err_t hci_console_init(void)
{
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();

    repl_config.prompt = PROMPT_STR ">";
    repl_config.max_cmdline_length = 256;

    hci_common_register_commands();
#if CONFIG_ENABLE_BLE_DTM_CONFIGURATION_COMMAND
    ble_dtm_config_register_commands();
#endif

    esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &repl));
    ESP_ERROR_CHECK(esp_console_start_repl(repl));
    return ESP_OK;
}
