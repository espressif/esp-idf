/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "hal/uart_types.h"
#include "test_shared.h"
#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_uart.h"

#define LP_UART_PORT_NUM            LP_UART_NUM_0
#define LP_UART_BUFFER_LEN          UART_BUF_SIZE
#define LP_UART_TRANS_WAIT_FOREVER  -1

volatile lp_core_test_commands_t test_cmd = LP_CORE_NO_COMMAND;
volatile lp_core_test_command_reply_t test_cmd_reply = LP_CORE_COMMAND_INVALID;

/* Tx and Rx buffers for LP UART */
uint8_t tx_data[LP_UART_BUFFER_LEN] = {};
uint8_t rx_data[LP_UART_BUFFER_LEN] = {};

/* Data transmission length */
volatile uint8_t tx_len = 0;

int main(void)
{
    while (1) {
        /* Wait for the HP core to start the test */
        while (test_cmd == LP_CORE_NO_COMMAND) {

        }

        if (test_cmd == LP_CORE_LP_UART_WRITE_TEST) {
            /* Write data on LP UART */
            lp_core_uart_write_bytes(LP_UART_PORT_NUM, (const char *)tx_data, tx_len, LP_UART_TRANS_WAIT_FOREVER);
        }

        if (test_cmd == LP_CORE_LP_UART_READ_TEST) {
            /* Read data from LP UART */
            int bytes_remaining = tx_len;
            int bytes_received = 0;
            int idx = 0;
            while (bytes_remaining > 0) {
                bytes_received = lp_core_uart_read_bytes(LP_UART_PORT_NUM, rx_data + idx, tx_len, LP_UART_TRANS_WAIT_FOREVER);
                idx += bytes_received;
                bytes_remaining -= bytes_received;
            }
        }

        /* Synchronize with the HP core running the test */
        test_cmd = LP_CORE_NO_COMMAND;
        test_cmd_reply = LP_CORE_COMMAND_OK;
    }
}
