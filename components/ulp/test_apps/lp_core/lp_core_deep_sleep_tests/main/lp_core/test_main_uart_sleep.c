/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_uart.h"
#include "ulp_lp_core_print.h"
#include "ulp_lp_core_lp_uart_shared.h"
#include "lp_sleep_payload.h"
#include "lp_sleep_finish.h"

volatile int32_t uart_sleep_rx_len;
uint8_t uart_sleep_rx[LP_SLEEP_UART_LEN];

int main(void)
{
    uint8_t tx[LP_SLEEP_UART_LEN];

    ulp_lp_core_delay_us(LP_SLEEP_SETTLE_US);
    lp_sleep_fill_uart_tx(tx);
    lp_core_uart_write_bytes(LP_UART_NUM_0, (const char *)tx, LP_SLEEP_UART_LEN, LP_SLEEP_IO_TIMEOUT_CYCLES);
    lp_core_printf(LP_SLEEP_PRINT "\r\n");
    lp_core_uart_tx_flush(LP_UART_NUM_0);
    /* Deep-sleep entry on C6 leaves one 0xFF in the LP UART RX FIFO.
     * The peer does not send it. Clear the FIFO so the reply is read
     * from its first byte. */
    lp_core_uart_clear_buf();
    uart_sleep_rx_len = lp_core_uart_read_bytes(LP_UART_NUM_0, uart_sleep_rx, LP_SLEEP_UART_LEN,
                                                LP_SLEEP_IO_TIMEOUT_CYCLES);
    lp_sleep_finish();
}
