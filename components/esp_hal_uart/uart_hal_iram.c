/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for UART (IRAM part)
#include "hal/uart_hal.h"

void uart_hal_txfifo_rst(uart_hal_context_t *hal)
{
    uart_ll_txfifo_rst(hal->dev);
}

void uart_hal_rxfifo_rst(uart_hal_context_t *hal)
{
    uart_ll_rxfifo_rst(hal->dev);
}

void uart_hal_tx_break(uart_hal_context_t *hal, uint32_t break_num)
{
    uart_ll_tx_break(hal->dev, break_num);
}

void uart_hal_write_txfifo(uart_hal_context_t *hal, const uint8_t *buf, uint32_t data_size, uint32_t *write_size)
{
    uint16_t fill_len = uart_ll_get_txfifo_len(hal->dev);
    if (fill_len > data_size) {
        fill_len = data_size;
    }
    *write_size = fill_len;
    uart_ll_write_txfifo(hal->dev, buf, fill_len);
}

void uart_hal_read_rxfifo(uart_hal_context_t *hal, uint8_t *buf, int *inout_rd_len)
{
    if (*inout_rd_len <= 0) {
        *inout_rd_len = uart_ll_get_rxfifo_len(hal->dev);
    }
    uart_ll_read_rxfifo(hal->dev, buf, *inout_rd_len);
}
