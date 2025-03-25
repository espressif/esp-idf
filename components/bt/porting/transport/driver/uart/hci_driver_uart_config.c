/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "driver/uart.h"
#include "hci_driver_uart.h"

static const char *TAG = "hci_uart_config";
static uart_config_t s_uart_cfg;
static hci_driver_uart_params_config_t s_hci_driver_uart_params = BT_HCI_DRIVER_UART_CONFIG_DEFAULT();

int hci_driver_uart_config(hci_driver_uart_params_config_t *uart_config)
{
    uart_config_t *uart_cfg;
    uart_cfg = &s_uart_cfg;
    uart_cfg->baud_rate = uart_config->hci_uart_baud;
    uart_cfg->data_bits = uart_config->hci_uart_data_bits;
    uart_cfg->stop_bits = uart_config->hci_uart_stop_bits;
    uart_cfg->parity    = uart_config->hci_uart_parity;
    uart_cfg->flow_ctrl = uart_config->hci_uart_flow_ctrl;
    uart_cfg->source_clk= UART_SCLK_DEFAULT;
    uart_cfg->rx_flow_ctrl_thresh = UART_FIFO_LEN - 1;

    ESP_LOGI(TAG,"set uart pin tx:%d, rx:%d.\n", uart_config->hci_uart_tx_pin, uart_config->hci_uart_rx_pin);
    ESP_LOGI(TAG,"set rts:%d, cts:%d.\n", uart_config->hci_uart_rts_pin, uart_config->hci_uart_cts_pin);
    ESP_LOGI(TAG,"set baud_rate:%d.\n", uart_config->hci_uart_baud);
    ESP_LOGI(TAG,"set flow_ctrl:%d.\n", uart_config->hci_uart_flow_ctrl);

    ESP_ERROR_CHECK(uart_driver_delete(uart_config->hci_uart_port));
    ESP_ERROR_CHECK(uart_param_config(uart_config->hci_uart_port, uart_cfg));
    ESP_ERROR_CHECK(uart_set_pin(uart_config->hci_uart_port, uart_config->hci_uart_tx_pin, uart_config->hci_uart_rx_pin,
                                 uart_config->hci_uart_rts_pin, uart_config->hci_uart_cts_pin));
    return 0;
}

int
hci_driver_uart_pin_update(int tx_pin, int rx_pin, int cts_pin, int rts_pin)
{
    hci_driver_uart_params_config_t *uart_param = &s_hci_driver_uart_params;
    uart_param->hci_uart_tx_pin = tx_pin;
    uart_param->hci_uart_rx_pin = rx_pin;
    uart_param->hci_uart_rts_pin = rts_pin;
    uart_param->hci_uart_cts_pin = cts_pin;
    return hci_driver_uart_config(uart_param);
}

hci_driver_uart_params_config_t *
hci_driver_uart_config_param_get(void)
{
    return &s_hci_driver_uart_params;
}
