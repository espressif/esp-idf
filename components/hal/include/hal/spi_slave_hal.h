// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The HAL layer for SPI slave (common part)

// SPI slave HAL usages:
// 1. initialize the bus
// 2. initialize the DMA descriptors if DMA used
// 3. call setup_device to update parameters for the device
// 4. prepare data to send, and prepare the receiving buffer
// 5. trigger user defined SPI transaction to start
// 6. wait until the user transaction is done
// 7. store the received data and get the length
// 8. check and reset the DMA (if needed) before the next transaction

#pragma once

#include <esp_types.h>
#include "soc/lldesc.h"
#include "soc/spi_struct.h"
#include "soc/soc_caps.h"
#include "hal/spi_ll.h"

/**
 * Context that should be maintained by both the driver and the HAL.
 */
typedef struct {
    /* configured by driver at initialization, don't touch */
    spi_dev_t     *hw;              ///< Beginning address of the peripheral registers.
    spi_dma_dev_t *dma_in;          ///< Address of the DMA peripheral registers which stores the data received from a peripheral into RAM.
    spi_dma_dev_t *dma_out;         ///< Address of the DMA peripheral registers which transmits the data from RAM to a peripheral.
    /* should be configured by driver at initialization */
    lldesc_t      *dmadesc_rx;      /**< Array of DMA descriptor used by the TX DMA.
                                     *   The amount should be larger than dmadesc_n. The driver should ensure that
                                     *   the data to be sent is shorter than the descriptors can hold.
                                     */
    lldesc_t      *dmadesc_tx;      /**< Array of DMA descriptor used by the RX DMA.
                                     *   The amount should be larger than dmadesc_n. The driver should ensure that
                                     *   the data to be sent is shorter than the descriptors can hold.
                                     */
    int dmadesc_n;                  ///< The amount of descriptors of both ``dmadesc_tx`` and ``dmadesc_rx`` that the HAL can use.

    /*
     * configurations to be filled after ``spi_slave_hal_init``. Updated to
     * peripheral registers when ``spi_slave_hal_setup_device`` is called.
     */
    struct {
        uint32_t rx_lsbfirst : 1;
        uint32_t tx_lsbfirst : 1;
        uint32_t use_dma     : 1;
    };
    int mode;

    /*
     * Transaction specific (data), all these parameters will be updated to the
     * peripheral every transaction.
     */
    uint32_t bitlen;                ///< Expected maximum length of the transaction, in bits.
    const void *tx_buffer;          ///< Data to be sent
    void *rx_buffer;                ///< Buffer to hold the received data.

    /*  Other transaction result after one transaction */
    uint32_t rcv_bitlen;            ///< Length of the last transaction, in bits.
} spi_slave_hal_context_t;

typedef struct {
    uint32_t host_id;               ///< SPI controller ID
    spi_dma_dev_t *dma_in;          ///< Input  DMA(DMA -> RAM) peripheral register address
    spi_dma_dev_t *dma_out;         ///< Output DMA(RAM -> DMA) peripheral register address
} spi_slave_hal_config_t;

/**
 * Init the peripheral and the context.
 *
 * @param hal     Context of the HAL layer.
 * @param host_id Index of the SPI peripheral. 0 for SPI1, 1 for HSPI (SPI2) and 2 for VSPI (SPI3).
 */
void spi_slave_hal_init(spi_slave_hal_context_t *hal, const spi_slave_hal_config_t *hal_config);

/**
 * Deinit the peripheral (and the context if needed).
 *
 * @param hal Context of the HAL layer.
 */
void spi_slave_hal_deinit(spi_slave_hal_context_t *hal);

/**
 * Setup device-related configurations according to the settings in the context.
 *
 * @param hal Context of the HAL layer.
 */
void spi_slave_hal_setup_device(const spi_slave_hal_context_t *hal);

/**
 * Prepare the data for the current transaction.
 *
 * @param hal Context of the HAL layer.
 */
void spi_slave_hal_prepare_data(const spi_slave_hal_context_t *hal);

/**
 * Trigger start a user-defined transaction.
 *
 * @param hal Context of the HAL layer.
 */
void spi_slave_hal_user_start(const spi_slave_hal_context_t *hal);

/**
 * Check whether the transaction is done (trans_done is set).
 *
 * @param hal Context of the HAL layer.
 */
bool spi_slave_hal_usr_is_done(spi_slave_hal_context_t* hal);

/**
 * Post transaction operations, fetch data from the buffer and recored the length.
 *
 * @param hal Context of the HAL layer.
 */
void spi_slave_hal_store_result(spi_slave_hal_context_t *hal);

/**
 * Get the length of last transaction, in bits. Should be called after ``spi_slave_hal_store_result``.
 *
 * Note that if last transaction is longer than configured before, the return
 * value will be truncated to the configured length.
 *
 * @param hal Context of the HAL layer.
 *
 * @return Length of the last transaction, in bits.
 */
uint32_t spi_slave_hal_get_rcv_bitlen(spi_slave_hal_context_t *hal);

/**
 * Check whether we need to reset the DMA according to the status of last transactions.
 *
 * In ESP32, sometimes we may need to reset the DMA for the slave before the
 * next transaction. Call this to check it.
 *
 * @param hal Context of the HAL layer.
 *
 * @return true if reset is needed, else false.
 */
bool spi_slave_hal_dma_need_reset(const spi_slave_hal_context_t *hal);
