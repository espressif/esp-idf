/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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

#include "esp_types.h"
#include "soc/soc_caps.h"
#include "hal/dma_types.h"
#include "hal/gdma_types.h"
#if SOC_GPSPI_SUPPORTED
#include "hal/spi_ll.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_GPSPI_SUPPORTED
#if (SOC_GDMA_TRIG_PERIPH_SPI2_BUS == SOC_GDMA_BUS_AHB)
typedef dma_descriptor_align4_t spi_dma_desc_t;
#else
typedef dma_descriptor_align8_t spi_dma_desc_t;
#endif

/**
 * Context that should be maintained by both the driver and the HAL.
 */
typedef struct {
    /* configured by driver at initialization, don't touch */
    spi_dev_t     *hw;              ///< Beginning address of the peripheral registers.
    /* should be configured by driver at initialization */
    spi_dma_desc_t *dmadesc_rx;     /**< Array of DMA descriptor used by the TX DMA.
                                     *   The amount should be larger than dmadesc_n. The driver should ensure that
                                     *   the data to be sent is shorter than the descriptors can hold.
                                     */
    spi_dma_desc_t *dmadesc_tx;     /**< Array of DMA descriptor used by the RX DMA.
                                     *   The amount should be larger than dmadesc_n. The driver should ensure that
                                     *   the data to be sent is shorter than the descriptors can hold.
                                     */
    int           dmadesc_n;        ///< The amount of descriptors of both ``dmadesc_tx`` and ``dmadesc_rx`` that the HAL can use.

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
} spi_slave_hal_config_t;

/**
 * Init the peripheral and the context.
 *
 * @param hal        Context of the HAL layer.
 * @param hal_config Configuration of the HAL
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
 * Prepare rx hardware for a new DMA trans
 *
 * @param hw Beginning address of the peripheral registers.
 */
void spi_slave_hal_hw_prepare_rx(spi_dev_t *hw);

/**
 * Prepare tx hardware for a new DMA trans
 *
 * @param hw Beginning address of the peripheral registers.
 */
void spi_slave_hal_hw_prepare_tx(spi_dev_t *hw);

/**
 * Rest peripheral registers to default value
 *
 * @param hal Context of the HAL layer.
 */
void spi_slave_hal_hw_reset(spi_slave_hal_context_t *hal);

/**
 * Rest hw fifo in peripheral, for a CPU controlled trans
 *
 * @param hal Context of the HAL layer.
 */
void spi_slave_hal_hw_fifo_reset(spi_slave_hal_context_t *hal, bool tx_rst, bool rx_rst);

/**
 * Push data needed to be transmit into hw fifo
 *
 * @param hal Context of the HAL layer.
 */
void spi_slave_hal_push_tx_buffer(spi_slave_hal_context_t *hal);

/**
 * Config transaction bit length for slave
 *
 * @param hal Context of the HAL layer.
 */
void spi_slave_hal_set_trans_bitlen(spi_slave_hal_context_t *hal);

/**
 * Enable/Disable miso/mosi signals in peripheral
 *
 * @param hal Context of the HAL layer.
 */
void spi_slave_hal_enable_data_line(spi_slave_hal_context_t *hal);

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
 * Get SPI interrupt bits status by mask
 *
 * @param hal Context of the HAL layer.
 * @param mask Mask of the interrupt bits to check.
 * @return True if the masked interrupts are set, false otherwise.
 */
bool spi_slave_hal_get_intr_status(spi_slave_hal_context_t *hal, uint32_t mask);

/**
 * Clear SPI interrupt bits by mask
 *
 * @param hal Context of the HAL layer.
 * @param mask Mask of the interrupt bits to clear.
 */
void spi_slave_hal_clear_intr_status(spi_slave_hal_context_t *hal, uint32_t mask);

/**
 * Post transaction operations, fetch data from the buffer and recorded the length.
 *
 * @param hal Context of the HAL layer.
 */
void spi_slave_hal_store_result(spi_slave_hal_context_t *hal);

/**
 * Get the length of last transaction, in bits. Should be called after ``spi_slave_hal_store_result``.
 *
 * Note that if last transaction is longer than configured before, the return
 * value still the actual length.
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

#endif  //#if SOC_GPSPI_SUPPORTED

#ifdef __cplusplus
}
#endif
