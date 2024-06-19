/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc_caps.h"

#if SOC_LP_SPI_SUPPORTED

#include <stdint.h>
#include <string.h>
#include "esp_err.h"
#include "ulp_lp_core_spi.h"
#include "soc/lp_spi_struct.h"

/* Use the register structure to access LP_SPI module registers */
lp_spi_dev_t *lp_spi_dev = &LP_SPI;

static inline esp_err_t lp_core_spi_wait_for_interrupt(int32_t ticks_to_wait)
{
    uint32_t to = 0;
    while (!lp_spi_dev->spi_dma_int_raw.reg_trans_done_int_raw) {
        if (ticks_to_wait > -1) {
            /* If the ticks_to_wait value is not -1, keep track of ticks and
             * break from the loop once the timeout is reached.
             */
            to++;
            if (to >= ticks_to_wait) {
                /* Clear interrupt bits */
                lp_spi_dev->spi_dma_int_clr.reg_trans_done_int_clr = 1;
                return ESP_ERR_TIMEOUT;
            }
        }
    }

    return ESP_OK;
}

//////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Public APIs ///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

esp_err_t lp_core_lp_spi_master_transfer(lp_spi_transaction_t *trans_desc, int32_t ticks_to_wait)
{
    esp_err_t ret = ESP_OK;

    /* Argument sanity check
     * Note: The Tx buffer is mandatory for this API.
     */
    if (trans_desc == NULL || trans_desc->tx_buffer == NULL || trans_desc->tx_length == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Reset the Tx and Rx FIFOs */
    lp_spi_dev->spi_dma_conf.reg_rx_afifo_rst = 1;
    lp_spi_dev->spi_dma_conf.reg_rx_afifo_rst = 0;
    lp_spi_dev->spi_dma_conf.reg_buf_afifo_rst = 1;
    lp_spi_dev->spi_dma_conf.reg_buf_afifo_rst = 0;

    /* Clear any previous interrupts.
     * Note: LP SPI does not have any DMA access but the interrupt bit lives in the DMA interrupt register.
     */
    lp_spi_dev->spi_dma_int_clr.reg_trans_done_int_clr = 1;

    /* Make sure that we do not have any ongoing transactions */
    if (lp_spi_dev->spi_cmd.reg_usr) {
        return ESP_ERR_INVALID_STATE;
    }

    /* Configure dummy bits */
    lp_spi_dev->spi_user.reg_usr_dummy = trans_desc->dummy_bits ? 1 : 0;
    if (trans_desc->dummy_bits) {
        lp_spi_dev->spi_user1.reg_usr_dummy_cyclelen = trans_desc->dummy_bits - 1;
    }

    /* Configure the command and command bit length */
    lp_spi_dev->spi_user.reg_usr_command = trans_desc->command_bits ? 1 : 0;
    if (trans_desc->command_bits) {
        lp_spi_dev->spi_user2.reg_usr_command_bitlen = trans_desc->command_bits - 1;
        lp_spi_dev->spi_user2.reg_usr_command_value = lp_spi_dev->spi_ctrl.reg_wr_bit_order ? trans_desc->command : __builtin_bswap32(trans_desc->command << (32 - trans_desc->command_bits));
    }

    /* Configure the address and address bit length */
    lp_spi_dev->spi_user.reg_usr_addr = trans_desc->address_bits ? 1 : 0;
    if (trans_desc->address_bits) {
        lp_spi_dev->spi_user1.reg_usr_addr_bitlen = trans_desc->address_bits - 1;
        lp_spi_dev->spi_addr.reg_usr_addr_value = lp_spi_dev->spi_ctrl.reg_wr_bit_order ? __builtin_bswap32(trans_desc->address) : trans_desc->address << (32 - trans_desc->address_bits);
    }

    /* Set data lines */
    lp_spi_dev->spi_user.reg_usr_mosi = 1;
    lp_spi_dev->spi_user.reg_usr_miso = trans_desc->rx_buffer ? 1 : 0;

    /* Configure the transaction bit length */
    int tx_bitlen = trans_desc->tx_length * 8;
    lp_spi_dev->spi_ms_dlen.reg_ms_data_bitlen = tx_bitlen - 1;

    /* Prepare the data to be transmitted */
    uint32_t tx_idx = 0;
    uint32_t rx_idx = 0;

    /* The TRM suggests that the data is sent from and received in the LP_SPI_W0_REG ~ LP_SPI_W15_REG registers.
     * The following rules apply:
     * 1. The first 64 bytes are sent from/received in LP_SPI_W0_REG ~ LP_SPI_W15_REG
     * 2. Bytes 64 - 255 are repeatedly sent from or received in LP_SPI_W15_REG[31:24]
     * 3. Subsequent blocks of 256 bytes of data continue to follow the above rules
     *
     * This driver, however, avoids using the LP_SPI_W15_REG altogether. In other words,
     * this driver sends or receives data in chunks of 60 bytes (LP_SPI_W0_REG ~ LP_SPI_W14_REG)
     * and does not handle the repeated use of the high-byte of LP_SPI_W15_REG. This design approach
     * has been chosen to simplify the data handling logic.
     */
    uint8_t max_data_reg_num = (SOC_LP_SPI_MAXIMUM_BUFFER_SIZE / 4) - 1; // 15
    uint8_t max_data_chunk_size = max_data_reg_num * 4; // 60
    while (tx_idx < trans_desc->tx_length) {
        /* Store 4 bytes of data in the data buffer registers serially. */
        lp_spi_dev->data_buf[(tx_idx / 4) & max_data_reg_num].reg_buf = *(uint32_t *)(trans_desc->tx_buffer + tx_idx);
        tx_idx += 4;

        /* Begin transmission of the data if we have pushed all the data or if we have reached the maximum data chunk size */
        if ((tx_idx >= trans_desc->tx_length) || (tx_idx % max_data_chunk_size) == 0) {
            /* Apply the configuration */
            lp_spi_dev->spi_cmd.reg_update = 1;
            while (lp_spi_dev->spi_cmd.reg_update) {
                ;
            }

            /* Start the transaction */
            lp_spi_dev->spi_cmd.reg_usr = 1;

            /* Wait for the transaction to complete */
            ret = lp_core_spi_wait_for_interrupt(ticks_to_wait);
            if (ret != ESP_OK) {
                return ret;
            }

            /* Clear the transaction done interrupt */
            lp_spi_dev->spi_dma_int_clr.reg_trans_done_int_clr = 1;

            /* Fetch the received data if an Rx buffer is provided */
            if (trans_desc->rx_buffer != NULL) {
                while (rx_idx < tx_idx) {
                    *(uint32_t *)(trans_desc->rx_buffer + rx_idx) = lp_spi_dev->data_buf[(rx_idx / 4) & max_data_reg_num].reg_buf;
                    rx_idx += 4;
                    // This loop would exit even if we haven't received all the data.
                }
            }
        }
    }

    return ret;
}

esp_err_t lp_core_lp_spi_slave_transfer(lp_spi_transaction_t *trans_desc, int32_t ticks_to_wait)
{
    esp_err_t ret = ESP_OK;

    /* Argument sanity check
     * Note: The Rx buffer is mandatory for this API.
     */
    if (trans_desc == NULL || trans_desc->rx_buffer == NULL || trans_desc->rx_length == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Reset the Tx and Rx FIFOs */
    lp_spi_dev->spi_dma_conf.reg_rx_afifo_rst = 1;
    lp_spi_dev->spi_dma_conf.reg_rx_afifo_rst = 0;
    lp_spi_dev->spi_dma_conf.reg_buf_afifo_rst = 1;
    lp_spi_dev->spi_dma_conf.reg_buf_afifo_rst = 0;

    /* Clear any previous interrupts.
     * Note: LP SPI does not have any DMA access but the interrupt bit lives in the DMA interrupt register.
     */
    lp_spi_dev->spi_dma_int_clr.reg_trans_done_int_clr = 1;

    /* Set data lines */
    lp_spi_dev->spi_user.reg_usr_mosi = 1;
    lp_spi_dev->spi_user.reg_usr_miso = 1;

    /* Configure the transaction bit length */
    int rx_bitlen = trans_desc->rx_length * 8;
    lp_spi_dev->spi_ms_dlen.reg_ms_data_bitlen = rx_bitlen - 1;

    /* Prepare the data to be received */
    uint32_t rx_idx = 0;
    uint32_t rcvd_bitlen = 0;
    uint32_t rcvd_length_in_bytes = 0;

    /* The LP SPI slave receives data in the LP_SPI_W0_REG ~ LP_SPI_W15_REG registers.
     * The following rules apply:
     * 1. The first 64 bytes are received in LP_SPI_W0_REG ~ LP_SPI_W15_REG
     * 2. The next 64 bytes are overwritten in LP_SPI_W0_REG ~ LP_SPI_W15_REG
     *
     * Since the peripheral has no protection against overwriting the data, we restrict the
     * driver to receive up to 64 bytes of data at a time.
     */
    uint32_t length_in_bytes = trans_desc->rx_length;
    if (trans_desc->rx_length > SOC_LP_SPI_MAXIMUM_BUFFER_SIZE) {
        /* Truncate the length to the maximum buffer size */
        length_in_bytes = SOC_LP_SPI_MAXIMUM_BUFFER_SIZE;
    }

    while (rx_idx < length_in_bytes) {
        /* Wait for the transmission to complete */
        ret = lp_core_spi_wait_for_interrupt(ticks_to_wait);
        if (ret != ESP_OK) {
            return ret;
        }

        /* Fetch the received bit length */
        rcvd_bitlen = lp_spi_dev->spi_slave1.reg_slv_data_bitlen > (trans_desc->rx_length * 8) ? (trans_desc->rx_length * 8) : lp_spi_dev->spi_slave1.reg_slv_data_bitlen;
        rcvd_length_in_bytes = (rcvd_bitlen + 7) / 8;

        /* Read the received data */
        while (rx_idx < rcvd_length_in_bytes) {
            *(uint32_t *)(trans_desc->rx_buffer + rx_idx) = lp_spi_dev->data_buf[(rx_idx / 4)].reg_buf;
            rx_idx += 4;
        }

        /* Clear the transaction done interrupt */
        lp_spi_dev->spi_dma_int_clr.reg_trans_done_int_clr = 1;
    }

    /* Prepare data for transmission if a Tx buffer is provided */
    if (trans_desc->tx_buffer != NULL) {
        uint32_t tx_idx = 0;
        uint32_t length_in_bytes = trans_desc->tx_length;
        if (length_in_bytes > SOC_LP_SPI_MAXIMUM_BUFFER_SIZE) {
            /* Truncate the length to the maximum buffer size */
            length_in_bytes = SOC_LP_SPI_MAXIMUM_BUFFER_SIZE;
        }

        while (tx_idx < length_in_bytes) {
            /* Store 4 bytes of data in the data buffer registers serially. */
            lp_spi_dev->data_buf[(tx_idx / 4)].reg_buf = *(uint32_t *)(trans_desc->tx_buffer + tx_idx);
            tx_idx += 4;
        }

        /* Apply the configuration */
        lp_spi_dev->spi_cmd.reg_update = 1;
        while (lp_spi_dev->spi_cmd.reg_update) {
            ;
        }

        /* Start the transaction */
        lp_spi_dev->spi_cmd.reg_usr = 1;

        /* Wait for the transaction to complete */
        ret = lp_core_spi_wait_for_interrupt(ticks_to_wait);
        if (ret != ESP_OK) {
            return ret;
        }

        /* Clear the transaction done interrupt */
        lp_spi_dev->spi_dma_int_clr.reg_trans_done_int_clr = 1;
    }

    return ret;
}

#endif /* SOC_LP_SPI_SUPPORTED */
