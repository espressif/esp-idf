/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

/*
 * The HAL layer for SPI Slave HD mode.
 *
 * Usage (segment mode):
 * - Firstly, initialize the slave with `spi_slave_hd_hal_init`
 *
 * - Event handling:
 *     - (Optional) Call ``spi_slave_hd_hal_enable_event_intr`` to enable the used interrupts
 *     - (Basic) Call ``spi_slave_hd_hal_check_clear_event`` to check whether an event happen, and also
 *       clear its interrupt. For events: SPI_EV_BUF_TX, SPI_EV_BUF_RX, SPI_EV_BUF_RX, SPI_EV_CMD9,
 *       SPI_EV_CMDA.
 *     - (Advanced) Call ``spi_slave_hd_hal_check_disable_event`` to disable the interrupt of an event,
 *       so that the task can call ``spi_slave_hd_hal_invoke_event_intr`` later to manually invoke the
 *       ISR. For SPI_EV_SEND, SPI_EV_RECV.
 *
 * - TXDMA:
 *     - To send data through DMA, call `spi_slave_hd_hal_txdma`
 *     - When the operation is done, SPI_EV_SEND will be triggered.
 *
 * - RXDMA:
 *     - To receive data through DMA, call `spi_slave_hd_hal_rxdma`
 *     - When the operation is done, SPI_EV_RECV will be triggered.
 *     - Call ``spi_slave_hd_hal_rxdma_seg_get_len`` to get the received length
 *
 *  - Shared buffer:
 *      - Call ``spi_slave_hd_hal_write_buffer`` to write the shared register buffer. When the buffer is
 *        read by the master (regardless of the read address), SPI_EV_BUF_TX will be triggered
 *      - Call ``spi_slave_hd_hal_read_buffer`` to read the shared register buffer. When the buffer is
 *        written by the master (regardless of the written address), SPI_EV_BUF_RX will be triggered.
 */

#pragma once

#include "esp_types.h"
#include "esp_err.h"
#include "soc/soc_caps.h"
#include "hal/spi_types.h"
#if SOC_GPSPI_SUPPORTED
#include "hal/spi_ll.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_GPSPI_SUPPORTED

/// Configuration of the HAL
typedef struct {
    uint32_t      host_id;                          ///< Host ID of the spi peripheral
    bool          dma_enabled;                      ///< DMA enabled or not
    bool          append_mode;                      ///< True for DMA append mode, false for segment mode
    bool          three_wire_mode;                  ///< True for 3-wire mode, false for 4-wire mode
    uint32_t      spics_io_num;                     ///< CS GPIO pin for this device
    uint8_t       mode;                             ///< SPI mode (0-3)
    uint32_t      command_bits;                     ///< command field bits, multiples of 8 and at least 8.
    uint32_t      address_bits;                     ///< address field bits, multiples of 8 and at least 8.
    uint32_t      dummy_bits;                       ///< dummy field bits, multiples of 8 and at least 8.

    struct {
        uint32_t  tx_lsbfirst : 1;                  ///< Whether TX data should be sent with LSB first.
        uint32_t  rx_lsbfirst : 1;                  ///< Whether RX data should be read with LSB first.
    };
} spi_slave_hd_hal_config_t;

/// Context of the HAL, initialized by :cpp:func:`spi_slave_hd_hal_init`.
typedef struct {
    /* address of the hardware */
    spi_dev_t                       *dev;                   ///< Beginning address of the peripheral registers.

    /* Internal status used by the HAL implementation, initialized as 0. */
    uint32_t                        intr_not_triggered;
} spi_slave_hd_hal_context_t;

/**
 * @brief Initialize the hardware and part of the context
 *
 * @param hal        Context of the HAL layer
 * @param hal_config Configuration of the HAL
 */
void spi_slave_hd_hal_init(spi_slave_hd_hal_context_t *hal, const spi_slave_hd_hal_config_t *hal_config);

/**
 * @brief Check and clear signal of one event
 *
 * @param hal       Context of the HAL layer
 * @param ev        Event to check
 * @return          True if event triggered, otherwise false
 */
bool spi_slave_hd_hal_check_clear_event(spi_slave_hd_hal_context_t* hal, spi_event_t ev);

/**
 * @brief Check and clear the interrupt by mask
 *
 * @param hal       Context of the HAL layer
 * @param mask      Mask of the interrupt bits to check
 * @return          True if the masked interrupts are set, false otherwise
 */
bool spi_slave_hd_hal_check_clear_intr(spi_slave_hd_hal_context_t *hal, uint32_t mask);

/**
 * @brief Check and clear the interrupt of one event.
 *
 * @note The event source will be kept, so that the interrupt can be invoked by
 *       :cpp:func:`spi_slave_hd_hal_invoke_event_intr`. If event not triggered, its interrupt source
 *       will not be disabled either.
 *
 * @param hal       Context of the HAL layer
 * @param ev        Event to check and disable
 * @return          True if event triggered, otherwise false
 */
bool spi_slave_hd_hal_check_disable_event(spi_slave_hd_hal_context_t* hal, spi_event_t ev);

/**
 * @brief Enable to involve the ISR of corresponding event.
 *
 * @note The function, compared with :cpp:func:`spi_slave_hd_hal_enable_event_intr`, contains a
 *       workaround to force trigger the interrupt, even if the interrupt source cannot be initialized
 *       correctly.
 *
 * @param hal       Context of the HAL layer
 * @param ev        Event (reason) to invoke the ISR
 */
void spi_slave_hd_hal_invoke_event_intr(spi_slave_hd_hal_context_t* hal, spi_event_t ev);

/**
 * @brief Enable the interrupt source of corresponding event.
 *
 * @param hal       Context of the HAL layer
 * @param ev        Event whose corresponding interrupt source should be enabled.
 */
void spi_slave_hd_hal_enable_event_intr(spi_slave_hd_hal_context_t* hal, spi_event_t ev);

////////////////////////////////////////////////////////////////////////////////
// RX DMA
////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Start the RX DMA operation to the specified buffer.
 *
 * @param hal       Context of the HAL layer
 * @param[out] out_buf  Buffer to receive the data
 * @param len       Maximul length to receive
 */
void spi_slave_hd_hal_rxdma(spi_slave_hd_hal_context_t *hal);

/**
 * @brief Prepare hardware for a new dma rx trans
 *
 * @param hal       Context of the HAL layer
 */
void spi_slave_hd_hal_hw_prepare_rx(spi_slave_hd_hal_context_t *hal);

////////////////////////////////////////////////////////////////////////////////
// TX DMA
////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Start the TX DMA operation with the specified buffer
 *
 * @param hal       Context of the HAL layer
 * @param data      Buffer of data to send
 * @param len       Size of the buffer, also the maximum length to send
 */
void spi_slave_hd_hal_txdma(spi_slave_hd_hal_context_t *hal);

/**
 * @brief Prepare hardware for a new dma tx trans
 *
 * @param hal       Context of the HAL layer
 */
void spi_slave_hd_hal_hw_prepare_tx(spi_slave_hd_hal_context_t *hal);

////////////////////////////////////////////////////////////////////////////////
// Shared buffer
////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Read from the shared register buffer
 *
 * @param hal       Context of the HAL layer
 * @param addr      Address of the shared register to read
 * @param out_data  Buffer to store the read data
 * @param len       Length to read from the shared buffer
 */
void spi_slave_hd_hal_read_buffer(spi_slave_hd_hal_context_t *hal, int addr, uint8_t *out_data, size_t len);

/**
 * @brief Write the shared register buffer
 *
 * @param hal       Context of the HAL layer
 * @param addr      Address of the shared register to write
 * @param data      Buffer of the data to write
 * @param len       Length to write into the shared buffer
 */
void spi_slave_hd_hal_write_buffer(spi_slave_hd_hal_context_t *hal, int addr, uint8_t *data, size_t len);

/**
 * @brief Get the length of previous transaction.
 *
 * @param hal       Context of the HAL layer
 * @return          The length of previous transaction
 */
int spi_slave_hd_hal_get_rxlen(spi_slave_hd_hal_context_t *hal);

/**
 * @brief Get the address of last transaction
 *
 * @param hal       Context of the HAL layer
 * @return          The address of last transaction
 */
int spi_slave_hd_hal_get_last_addr(spi_slave_hd_hal_context_t *hal);

#endif  //#if SOC_GPSPI_SUPPORTED

#ifdef __cplusplus
}
#endif
