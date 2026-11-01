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

volatile int32_t spi_slave_sleep_ret;
uint8_t spi_slave_sleep_rx[LP_SLEEP_SPI_LEN];

int main(void)
{
    uint8_t tx[LP_SLEEP_SPI_LEN];
    lp_spi_transaction_t trans_desc = {
        .tx_length = LP_SLEEP_SPI_LEN,
        .rx_length = LP_SLEEP_SPI_LEN,
        .tx_buffer = tx,
        .rx_buffer = spi_slave_sleep_rx,
    };
    esp_err_t ret;

    ulp_lp_core_delay_us(LP_SLEEP_SETTLE_US);
    lp_sleep_fill_spi_slave_tx(tx);

    ret = lp_core_lp_spi_slave_arm(&trans_desc);
    if (ret == ESP_OK) {
        ret = lp_core_lp_spi_slave_wait(&trans_desc, LP_SLEEP_SPI_SLAVE_TIMEOUT_CYCLES);
    }
    spi_slave_sleep_ret = ret;
    lp_sleep_finish();
}
