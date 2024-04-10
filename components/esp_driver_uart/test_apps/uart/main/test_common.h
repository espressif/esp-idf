/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "driver/uart.h"

typedef struct {
    uart_port_t port_num;
    soc_module_clk_t default_src_clk;
    int tx_pin_num;
    int rx_pin_num;
    uint32_t rx_flow_ctrl_thresh;
} uart_port_param_t;

bool port_select(uart_port_param_t *port_param);
