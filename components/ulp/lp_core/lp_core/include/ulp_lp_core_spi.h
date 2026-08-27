/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"

/**
 * The LP SPI bus identifier to initiate a transaction on.
 */
typedef uint32_t lp_spi_bus_t;

/**
 * This structure describes one SPI transaction. The descriptor should not be modified until the transaction finishes.
 */
typedef struct {
    uint32_t tx_length;             /*!< Number of bytes to transmit. Must be 0 when ``tx_buffer`` is NULL. */
    uint32_t rx_length;             /*!< Number of bytes to receive. Must be 0 when ``rx_buffer`` is NULL. */
    const void *tx_buffer;          /*!< Pointer to the transmit buffer, or NULL. */
    void *rx_buffer;                /*!< Pointer to the receive buffer, or NULL. */
    lp_spi_bus_t bus;               /*!< The LP SPI bus to transmit the data on */
    // The following are only used in master mode transactions
    int command;                    /*!< Command data, of which the length is set in the ``command_bits`` field of this structure. */
    uint32_t address;               /*!< Address data, of which the length is set in the ``address_bits`` field of this structure. */
    uint8_t command_bits;           /*!< Default amount of bits in command phase */
    uint8_t address_bits;           /*!< Default amount of bits in address phase */
    uint8_t dummy_bits;             /*!< Amount of dummy bits to insert between address and data phase. */
} lp_spi_transaction_t;

/**
 * @brief Initiate an LP SPI master transaction.
 *
 * @param trans_desc        LP SPI transaction configuration descriptor
 * @param cycles_to_wait    Operation timeout in CPU cycles. Set to -1 to wait forever.
 *
 * @return esp_err_t        ESP_OK when successful
 *                          ESP_ERR_INVALID_ARG if the configuration is invalid
 *                          ESP_ERR_INVALID_STATE if a previous transaction is still in progress
 *                          ESP_ERR_TIMEOUT when the operation times out
 */
esp_err_t lp_core_lp_spi_master_transfer(lp_spi_transaction_t *trans_desc, int32_t cycles_to_wait);

/**
 * @brief Initiate an LP SPI slave transaction.
 *
 * @param trans_desc        LP SPI transaction configuration descriptor
 * @param cycles_to_wait    Operation timeout in CPU cycles. Set to -1 to wait forever.
 *
 * @return esp_err_t        ESP_OK when successful
 *                          ESP_ERR_INVALID_ARG if the configuration is invalid
 *                          ESP_ERR_INVALID_STATE if a previous transaction is still in progress
 *                          ESP_ERR_TIMEOUT when the operation times out
 */
esp_err_t lp_core_lp_spi_slave_transfer(lp_spi_transaction_t *trans_desc, int32_t cycles_to_wait);

/**
 * @brief Preload the LP SPI slave's TX data and arm the peripheral, then return.
 *
 * Loads `trans_desc->tx_buffer` into the LP-SPI W0..W15 data buffer,
 * programs the bit length, and starts the slave user phase. The call
 * does not block on the master's SCK; the peripheral is left armed and
 * will sample/drive the bus as soon as the master starts clocking.
 *
 * Pair with `lp_core_lp_spi_slave_wait()` to block on completion and
 * drain the RX buffer. Calling `lp_core_lp_spi_slave_arm()` again while
 * a previous arm has not been waited on returns `ESP_ERR_INVALID_STATE`.
 *
 * This is the LP-SPI counterpart of `spi_slave_queue_trans()`.
 *
 * @param trans_desc        LP SPI transaction configuration descriptor.
 *
 * @return esp_err_t        ESP_OK when successful
 *                          ESP_ERR_INVALID_ARG if the configuration is invalid
 *                          ESP_ERR_INVALID_STATE if a previous transaction is still in progress
 */
esp_err_t lp_core_lp_spi_slave_arm(lp_spi_transaction_t *trans_desc);

/**
 * @brief Wait for a previously-armed LP SPI slave transaction to complete.
 *
 * Must be paired with `lp_core_lp_spi_slave_arm()` using the same `trans_desc`.
 *
 * This is the LP-SPI counterpart of `spi_slave_get_trans_result()`.
 *
 * @param trans_desc        LP SPI transaction configuration descriptor.
 * @param cycles_to_wait    Operation timeout in CPU cycles. Set to -1 to wait forever.
 *
 * @return esp_err_t        ESP_OK when successful
 *                          ESP_ERR_INVALID_ARG if the configuration is invalid
 *                          ESP_ERR_TIMEOUT when the operation times out
 */
esp_err_t lp_core_lp_spi_slave_wait(lp_spi_transaction_t *trans_desc, int32_t cycles_to_wait);

#ifdef __cplusplus
}
#endif
