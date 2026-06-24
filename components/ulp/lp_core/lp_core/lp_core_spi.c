/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc_caps.h"

#if SOC_LP_SPI_SUPPORTED

#include <stdint.h>
#include <string.h>
#include "esp_err.h"
#include "ulp_lp_core_spi.h"
#include "ulp_lp_core_utils.h"
#include "hal/lp_spi_ll.h"

static lp_spi_ll_dev_t *lp_spi_dev = LP_SPI_LL_GET_HW();

static inline esp_err_t lp_core_spi_wait_for_interrupt(int32_t cycles_to_wait)
{
    uint32_t timeout_start = ulp_lp_core_get_cpu_cycles();
    while (!lp_spi_ll_get_int_trans_done(lp_spi_dev)) {
        if (ulp_lp_core_is_timeout_elapsed(timeout_start, cycles_to_wait)) {
            lp_spi_ll_clear_int_trans_done(lp_spi_dev);
            return ESP_ERR_TIMEOUT;
        }
    }

    return ESP_OK;
}

//////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Public APIs ///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

esp_err_t lp_core_lp_spi_master_transfer(lp_spi_transaction_t *trans_desc, int32_t cycles_to_wait)
{
    esp_err_t ret = ESP_OK;

    /* Argument sanity check
     * Note: The Tx buffer is mandatory for this API.
     */
    if (trans_desc == NULL || trans_desc->tx_buffer == NULL || trans_desc->tx_length == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Reset the Tx and Rx FIFOs */
    lp_spi_ll_reset_fifos(lp_spi_dev);

    /* Clear any previous interrupts.
     * Note: LP SPI does not have any DMA access but the interrupt bit lives in the DMA interrupt register.
     */
    lp_spi_ll_clear_int_trans_done(lp_spi_dev);

    /* Make sure that we do not have any ongoing transactions */
    if (lp_spi_ll_is_busy(lp_spi_dev)) {
        return ESP_ERR_INVALID_STATE;
    }

    /* Configure dummy bits */
    lp_spi_ll_set_dummy_en(lp_spi_dev, trans_desc->dummy_bits != 0);
    if (trans_desc->dummy_bits) {
        lp_spi_ll_set_dummy_cyclelen(lp_spi_dev, trans_desc->dummy_bits);
    }

    /* Configure the command and command bit length */
    lp_spi_ll_set_command_en(lp_spi_dev, trans_desc->command_bits != 0);
    if (trans_desc->command_bits) {
        lp_spi_ll_set_command_bitlen(lp_spi_dev, trans_desc->command_bits);
        lp_spi_ll_set_command_value(lp_spi_dev,
                                    lp_spi_ll_get_wr_bit_order(lp_spi_dev)
                                    ? trans_desc->command
                                    : __builtin_bswap32(trans_desc->command << (32 - trans_desc->command_bits)));
    }

    /* Configure the address and address bit length */
    lp_spi_ll_set_address_en(lp_spi_dev, trans_desc->address_bits != 0);
    if (trans_desc->address_bits) {
        lp_spi_ll_set_address_bitlen(lp_spi_dev, trans_desc->address_bits - 1);
        lp_spi_ll_set_address_value(lp_spi_dev,
                                    lp_spi_ll_get_wr_bit_order(lp_spi_dev)
                                    ? __builtin_bswap32(trans_desc->address)
                                    : trans_desc->address << (32 - trans_desc->address_bits));
    }

    /* Set data lines */
    lp_spi_ll_set_mosi_en(lp_spi_dev, true);
    lp_spi_ll_set_miso_en(lp_spi_dev, trans_desc->rx_buffer != NULL);

    /* Configure the transaction bit length */
    int tx_bitlen = trans_desc->tx_length * 8;
    lp_spi_ll_set_data_bitlen(lp_spi_dev, tx_bitlen - 1);

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
    uint8_t max_data_reg_num = (LP_SPI_LL_MAX_BUFFER_SIZE / 4) - 1; // 15
    uint8_t max_data_chunk_size = max_data_reg_num * 4; // 60
    while (tx_idx < trans_desc->tx_length) {
        /* Store 4 bytes of data in the data buffer registers serially. */
        lp_spi_ll_write_buffer_word(lp_spi_dev, (tx_idx / 4) & max_data_reg_num, *(uint32_t *)(trans_desc->tx_buffer + tx_idx));
        tx_idx += 4;

        /* Begin transmission of the data if we have pushed all the data or if we have reached the maximum data chunk size */
        if ((tx_idx >= trans_desc->tx_length) || (tx_idx % max_data_chunk_size) == 0) {
            /* Apply the configuration */
            lp_spi_ll_apply_config(lp_spi_dev);

            /* Start the transaction */
            lp_spi_ll_start_user_transaction(lp_spi_dev);

            /* Wait for the transaction to complete */
            ret = lp_core_spi_wait_for_interrupt(cycles_to_wait);
            if (ret != ESP_OK) {
                return ret;
            }

            /* Clear the transaction done interrupt */
            lp_spi_ll_clear_int_trans_done(lp_spi_dev);

            /* Fetch the received data if an Rx buffer is provided */
            if (trans_desc->rx_buffer != NULL) {
                while (rx_idx < tx_idx) {
                    *(uint32_t *)(trans_desc->rx_buffer + rx_idx) = lp_spi_ll_read_buffer_word(lp_spi_dev, (rx_idx / 4) & max_data_reg_num);
                    rx_idx += 4;
                    // This loop would exit even if we haven't received all the data.
                }
            }
        }
    }

    return ret;
}

esp_err_t lp_core_lp_spi_slave_transfer(lp_spi_transaction_t *trans_desc, int32_t cycles_to_wait)
{
    esp_err_t ret = ESP_OK;

    /* Argument sanity check
     * Note: The Rx buffer is mandatory for this API.
     */
    if (trans_desc == NULL || trans_desc->rx_buffer == NULL || trans_desc->rx_length == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Reset the Tx and Rx FIFOs */
    lp_spi_ll_reset_fifos(lp_spi_dev);

    /* Clear any previous interrupts.
     * Note: LP SPI does not have any DMA access but the interrupt bit lives in the DMA interrupt register.
     */
    lp_spi_ll_clear_int_trans_done(lp_spi_dev);

    /* Set data lines */
    lp_spi_ll_set_mosi_en(lp_spi_dev, true);
    lp_spi_ll_set_miso_en(lp_spi_dev, true);

    /* Configure the transaction bit length */
    int rx_bitlen = trans_desc->rx_length * 8;
    lp_spi_ll_set_data_bitlen(lp_spi_dev, rx_bitlen - 1);

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
    if (trans_desc->rx_length > LP_SPI_LL_MAX_BUFFER_SIZE) {
        /* Truncate the length to the maximum buffer size */
        length_in_bytes = LP_SPI_LL_MAX_BUFFER_SIZE;
    }

    while (rx_idx < length_in_bytes) {
        /* Wait for the transmission to complete */
        ret = lp_core_spi_wait_for_interrupt(cycles_to_wait);
        if (ret != ESP_OK) {
            return ret;
        }

        /* Fetch the received bit length */
        uint32_t slave_bitlen = lp_spi_ll_get_slave_rcv_bitlen(lp_spi_dev);
        rcvd_bitlen = slave_bitlen > (trans_desc->rx_length * 8) ? (trans_desc->rx_length * 8) : slave_bitlen;
        rcvd_length_in_bytes = (rcvd_bitlen + 7) / 8;

        /* Read the received data */
        while (rx_idx < rcvd_length_in_bytes) {
            *(uint32_t *)(trans_desc->rx_buffer + rx_idx) = lp_spi_ll_read_buffer_word(lp_spi_dev, rx_idx / 4);
            rx_idx += 4;
        }

        /* Clear the transaction done interrupt */
        lp_spi_ll_clear_int_trans_done(lp_spi_dev);
    }

    /* Prepare data for transmission if a Tx buffer is provided */
    if (trans_desc->tx_buffer != NULL) {
        uint32_t tx_idx = 0;
        uint32_t length_in_bytes = trans_desc->tx_length;
        if (length_in_bytes > LP_SPI_LL_MAX_BUFFER_SIZE) {
            /* Truncate the length to the maximum buffer size */
            length_in_bytes = LP_SPI_LL_MAX_BUFFER_SIZE;
        }

        while (tx_idx < length_in_bytes) {
            /* Store 4 bytes of data in the data buffer registers serially. */
            lp_spi_ll_write_buffer_word(lp_spi_dev, tx_idx / 4, *(uint32_t *)(trans_desc->tx_buffer + tx_idx));
            tx_idx += 4;
        }

        /* Apply the configuration */
        lp_spi_ll_apply_config(lp_spi_dev);

        /* Start the transaction */
        lp_spi_ll_start_user_transaction(lp_spi_dev);

        /* Wait for the transaction to complete */
        ret = lp_core_spi_wait_for_interrupt(cycles_to_wait);
        if (ret != ESP_OK) {
            return ret;
        }

        /* Clear the transaction done interrupt */
        lp_spi_ll_clear_int_trans_done(lp_spi_dev);
    }

    return ret;
}

#endif /* SOC_LP_SPI_SUPPORTED */
