/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "ulp_lp_core_spi.h"
#include "test_shared.h"

volatile lp_core_test_commands_t spi_test_cmd = LP_CORE_NO_COMMAND;

volatile uint8_t spi_master_tx_buf[100] = {0};
volatile uint8_t spi_master_rx_buf[100] = {0};
volatile uint32_t spi_tx_len = 0;

int main(void)
{
    /* Wait for the HP core to start the test */
    while (spi_test_cmd == LP_CORE_NO_COMMAND) {

    }

    /* Setup SPI transaction */
    lp_spi_transaction_t trans_desc = {
        .tx_length = spi_tx_len,
        .rx_length = spi_tx_len,
        .tx_buffer = (uint8_t *)spi_master_tx_buf,
        .rx_buffer = (uint8_t *)spi_master_rx_buf,
    };

    /* Transmit data */
    lp_core_lp_spi_master_transfer(&trans_desc, -1);

    /* Synchronize with the HP core running the test */
    spi_test_cmd = LP_CORE_NO_COMMAND;

    return 0;
}
