/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_spi.h"
#include "lp_sleep_payload.h"
#include "lp_sleep_finish.h"

volatile int32_t spi_master_sleep_ret;
uint8_t spi_master_sleep_rx[LP_SLEEP_SPI_LEN];

int main(void)
{
    uint8_t tx[LP_SLEEP_SPI_LEN];
    lp_spi_transaction_t trans_desc = {
        .tx_length = LP_SLEEP_SPI_LEN,
        .rx_length = LP_SLEEP_SPI_LEN,
        .tx_buffer = tx,
        .rx_buffer = spi_master_sleep_rx,
    };

    ulp_lp_core_delay_us(LP_SLEEP_SETTLE_US + LP_SLEEP_SPI_MASTER_WAIT_US);
    lp_sleep_fill_spi_master_tx(tx);

    spi_master_sleep_ret = lp_core_lp_spi_master_transfer(&trans_desc, LP_SLEEP_IO_TIMEOUT_CYCLES);
    lp_sleep_finish();
}
