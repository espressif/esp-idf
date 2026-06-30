/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
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
volatile uint32_t spi_slave_tx_len = 0;

/* Set by the LP slave once the hardware is armed (W0..W15 preloaded,
 * reg_usr written). The HP slave-side test polls this before sending the
 * "LP SPI slave ready" signal that releases the master, so the master
 * cannot clock SCK while the slave is still in its arm prologue.
 */
volatile uint32_t spi_slave_armed = 0;

int main(void)
{
    /* Wait for the HP core to finish writing spi_rx_len, spi_slave_tx_len,
     * and spi_slave_tx_buf before we read them. The HP side sets
     * spi_test_cmd_reply to LP_CORE_COMMAND_INVALID as a "go" signal
     * after filling the shared-memory buffers.
     */
    while (spi_test_cmd_reply == LP_CORE_COMMAND_NOK) {
    }
    spi_test_cmd_reply = LP_CORE_COMMAND_NOK;

    /* Setup SPI transaction.
     * When spi_slave_tx_len > 0 the HP side has preloaded spi_slave_tx_buf
     * with echo data that the slave should drive on MISO.
     */
    lp_spi_transaction_t trans_desc = {
        .rx_length = spi_rx_len,
        .rx_buffer = (uint8_t *)spi_slave_rx_buf,
        .tx_length = spi_slave_tx_len,
        .tx_buffer = spi_slave_tx_len > 0 ? (uint8_t *)spi_slave_tx_buf : NULL,
    };

    /* Arm the slave hardware, then publish the armed flag so the HP test
     * can release the master only after the slave is ready to clock.
     */
    if (lp_core_lp_spi_slave_arm(&trans_desc) != ESP_OK) {
        spi_test_cmd_reply = LP_CORE_COMMAND_NOK;
        return 0;
    }
    spi_slave_armed = 1;

    /* Block until TRANS_DONE, then drain whatever the master clocked in. */
    lp_core_lp_spi_slave_wait(&trans_desc, -1);
    spi_slave_armed = 0;

    /* Synchronize with the HP core running the test */
    spi_test_cmd_reply = LP_CORE_COMMAND_OK;

    return 0;
}
