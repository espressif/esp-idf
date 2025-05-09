/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "hal/uart_types.h"
#include "test_shared.h"
#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_uart.h"
#include "ulp_lp_core_print.h"
#include "soc/soc_caps.h"
#include "ulp_lp_core_interrupts.h"

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
volatile uint8_t rx_len = 0;

/* LP Core print test variables */
volatile char test_string[25];
volatile char test_long_string[200];
volatile int test_signed_integer;
volatile uint32_t test_unsigned_integer;
volatile int test_hex;
volatile char test_character;

int main(void)
{
    /* Enable interrupts.
     * This does not affect how the LP UART functions
     * but it will add extra test coverage to make sure
     * the interrupt handler does not cause any issues.
     */
    ulp_lp_core_intr_enable();

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
            int bytes_received = 0;
            int idx = 0;
            while (1) {
                /* Read 1 byte at a time until we receive the end_pattern of 0xFEEDBEEF */
                bytes_received = lp_core_uart_read_bytes(LP_UART_PORT_NUM, rx_data + idx, 1, 100);
                if (bytes_received < 0) {
                    break;
                }
                if (idx > 3 && rx_data[idx] == 0xEF && rx_data[idx - 1] == 0xBE && rx_data[idx - 2] == 0xED && rx_data[idx - 3] == 0xFE) {
                    /* Break and notify HP core of test completion */
                    break;
                }
                idx += bytes_received;
            }
        }

        if (test_cmd == LP_CORE_LP_UART_MULTI_BYTE_READ_TEST) {
            int bytes_remaining = rx_len;
            int bytes_received = 0;
            int idx = 0;
            while (bytes_remaining > 0) {
                /* Read as much data as we can in one iteration */
                bytes_received = lp_core_uart_read_bytes(LP_UART_PORT_NUM, rx_data + idx, bytes_remaining, LP_UART_TRANS_WAIT_FOREVER);
                if (bytes_received < 0) {
                    break;
                }
                bytes_remaining -= bytes_received;
                idx += bytes_received;
            }
        }

        if (test_cmd == LP_CORE_LP_UART_PRINT_TEST) {
            /* Write various cases to test lp_core_printf to test various format specifiers */
            lp_core_printf("%s\r\n", test_string);
            lp_core_printf("%s\r\n", test_long_string);
            lp_core_printf("Test printf signed integer %d\r\n", test_signed_integer);
            lp_core_printf("Test printf unsigned integer %u\r\n", test_unsigned_integer);
            lp_core_printf("Test printf hex %x\r\n", test_hex);
            lp_core_printf("Test printf character %c\r\n", test_character);
            // TODO: Floating point prints are not supported
            // lp_core_printf("Test printf float %f\r\n", (float)0.99);
        }

        /* Synchronize with the HP core running the test */
        test_cmd = LP_CORE_NO_COMMAND;
        test_cmd_reply = LP_CORE_COMMAND_OK;

        /* Wake up the HP core */
#if SOC_LIGHT_SLEEP_SUPPORTED
        ulp_lp_core_wakeup_main_processor();
#endif /* SOC_LIGHT_SLEEP_SUPPORTED */
    }
}
