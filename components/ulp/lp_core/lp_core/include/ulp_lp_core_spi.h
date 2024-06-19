/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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
    uint32_t tx_length;             /*!< Total data length to transmit in bytes */
    uint32_t rx_length;             /*!< Total data length to receive in bytes */
    const void *tx_buffer;          /*!< Pointer to the transmit buffer. Must be set for master mode transactions. Can be NULL for slave mode transactions. */
    void *rx_buffer;                /*!< Pointer to the receive buffer. Must be set for slave mode transactions. Can be NULL for master mode transactions. */
    lp_spi_bus_t bus;               /*!< The LP SPI bus to transmit the data on */
    // The following are only used in master mode transactions
    int command;                    /*!< Command data, of which the length is set in the ``command_bits`` field of this structure. */
    uint32_t address;               /*!< Address data, of which the length is set in the ``address_bits`` field of this structure. */
    uint8_t command_bits;           /*!< Default amount of bits in command phase */
    uint8_t address_bits;           /*!< Default amount of bits in address phase */
    uint8_t dummy_bits;             /*!< Amount of dummy bits to insert between address and data phase. */
} lp_spi_transaction_t;

/**
 * @brief Initiate an LP SPI transaction in master mode to transmit device to an SPI device and optionally receive data
 * from the device.
 *
 * @param trans_desc        LP SPI transaction configuration descriptor
 * @param ticks_to_wait     Operation timeout in CPU cycles. Set to -1 to wait forever.
 *
 * @return esp_err_t        ESP_OK when successful
 *                          ESP_ERR_INVALID_ARG if the configuration is invalid
 *                          ESP_ERR_TIMEOUT when the operation times out
 */
esp_err_t lp_core_lp_spi_master_transfer(lp_spi_transaction_t *trans_desc, int32_t ticks_to_wait);

/**
 * @brief Initiate an LP SPI transaction in slave mode to receive data from an SPI master and optionally transmit data
 * back to the master.
 *
 * @param trans_desc        LP SPI transaction configuration descriptor
 * @param ticks_to_wait     Operation timeout in CPU cycles. Set to -1 to wait forever.
 *
 * @return esp_err_t        ESP_OK when successful
 *                          ESP_ERR_INVALID_ARG if the configuration is invalid
 *                          ESP_ERR_TIMEOUT when the operation times out
 */
esp_err_t lp_core_lp_spi_slave_transfer(lp_spi_transaction_t *trans_desc, int32_t ticks_to_wait);

#ifdef __cplusplus
}
#endif
