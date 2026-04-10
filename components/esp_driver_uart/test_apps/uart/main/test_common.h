/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "driver/uart.h"
#include "sdkconfig.h"

typedef struct {
    uart_port_t port_num;
    soc_module_clk_t default_src_clk;
    int tx_pin_num;
    int rx_pin_num;
    uint32_t rx_flow_ctrl_thresh;
} uart_port_param_t;

bool port_select(uart_port_param_t *port_param);

#if CONFIG_IDF_TARGET_ESP32H4
#define TEST_UART_TX_PIN_NUM 2
#define TEST_UART_RX_PIN_NUM 3
#else
#define TEST_UART_TX_PIN_NUM 4
#define TEST_UART_RX_PIN_NUM 5
#endif
