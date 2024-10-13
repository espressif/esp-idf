/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "ulp_lp_core_spi.h"
#include "test_shared.h"

volatile lp_core_test_command_reply_t spi_test_cmd_reply = LP_CORE_COMMAND_NOK;

volatile uint8_t spi_slave_tx_buf[100] = {0};
volatile uint8_t spi_slave_rx_buf[100] = {0};
volatile uint32_t spi_rx_len = 0;

int main(void)
{
    /* Setup SPI transaction */
    lp_spi_transaction_t trans_desc = {
        .rx_length = spi_rx_len,
        .rx_buffer = (uint8_t *)spi_slave_rx_buf,
        .tx_buffer = NULL,
    };

    /* Receive data */
    lp_core_lp_spi_slave_transfer(&trans_desc, -1);

    /* Synchronize with the HP core running the test */
    spi_test_cmd_reply = LP_CORE_COMMAND_OK;

    return 0;
}
