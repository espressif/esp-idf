/*
 * SPDX-FileCopyrightText: 2010-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_types.h"
#include "soc/soc_caps.h"
#include "freertos/FreeRTOS.h"

#include "hal/spi_types.h"
#include "driver/spi_common.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if !SOC_SPI_SUPPORT_SLAVE_HD_VER2  && !CI_HEADER_CHECK
#error The SPI peripheral does not support this feature
#endif

#define SPI_SLAVE_HD_TRANS_DMA_BUFFER_ALIGN_AUTO   (1<<0)    ///< Automatically re-malloc dma buffer if user buffer doesn't meet hardware alignment or dma_capable, this process may lose some memory and performance

/// Descriptor of data to send/receive
typedef struct {
    uint8_t* data;                              ///< Buffer to send, must be DMA capable
    size_t   len;                               ///< Len of data to send/receive. For receiving the buffer length should be multiples of 4 bytes, otherwise the extra part will be truncated.
    size_t   trans_len;                         ///< For RX direction, it indicates the data actually received. For TX direction, it is meaningless.
    uint32_t flags;                             ///< Bitwise OR of SPI_SLAVE_HD_TRANS_* flags
    void*    arg;                               ///< Extra argument indicating this data
} spi_slave_hd_data_t;

/// Information of SPI Slave HD event
typedef struct {
    spi_event_t          event;                 ///< Event type
    spi_slave_hd_data_t* trans;                 ///< Corresponding transaction for SPI_EV_SEND and SPI_EV_RECV events
} spi_slave_hd_event_t;

/// Callback for SPI Slave HD
typedef bool (*slave_cb_t)(void* arg, spi_slave_hd_event_t* event, BaseType_t* awoken);

/// Channel of SPI Slave HD to do data transaction
typedef enum {
    SPI_SLAVE_CHAN_TX = 0,                      ///< The output channel (RDDMA)
    SPI_SLAVE_CHAN_RX = 1,                      ///< The input channel (WRDMA)
} spi_slave_chan_t;

/// Callback configuration structure for SPI Slave HD
typedef struct {
    slave_cb_t cb_buffer_tx;                    ///< Callback when master reads from shared buffer
    slave_cb_t cb_buffer_rx;                    ///< Callback when master writes to shared buffer
    slave_cb_t cb_send_dma_ready;               ///< Callback when TX data buffer is loaded to the hardware (DMA)
    slave_cb_t cb_sent;                         ///< Callback when data are sent
    slave_cb_t cb_recv_dma_ready;               ///< Callback when RX data buffer is loaded to the hardware (DMA)
    slave_cb_t cb_recv;                         ///< Callback when data are received
    slave_cb_t cb_cmd9;                         ///< Callback when CMD9 received
    slave_cb_t cb_cmdA;                         ///< Callback when CMDA received
    void* arg;                                  ///< Argument indicating this SPI Slave HD peripheral instance
} spi_slave_hd_callback_config_t;

//flags for ``spi_slave_hd_slot_config_t`` to use
#define SPI_SLAVE_HD_TXBIT_LSBFIRST     (1<<0)  ///< Transmit command/address/data LSB first instead of the default MSB first
#define SPI_SLAVE_HD_RXBIT_LSBFIRST     (1<<1)  ///< Receive data LSB first instead of the default MSB first
#define SPI_SLAVE_HD_BIT_LSBFIRST       (SPI_SLAVE_HD_TXBIT_LSBFIRST|SPI_SLAVE_HD_RXBIT_LSBFIRST) ///< Transmit and receive LSB first
#define SPI_SLAVE_HD_APPEND_MODE        (1<<2)  ///< Adopt DMA append mode for transactions. In this mode, users can load(append) DMA descriptors without stopping the DMA

/// Configuration structure for the SPI Slave HD driver
typedef struct {
    uint8_t mode;                               /**< SPI mode, representing a pair of (CPOL, CPHA) configuration:
                                                     - 0: (0, 0)
                                                     - 1: (0, 1)
                                                     - 2: (1, 0)
                                                     - 3: (1, 1)
                                                 */
    uint32_t spics_io_num;                      ///< CS GPIO pin for this device
    uint32_t flags;                             ///< Bitwise OR of SPI_SLAVE_HD_* flags
    uint32_t command_bits;                      ///< command field bits, multiples of 8 and at least 8.
    uint32_t address_bits;                      ///< address field bits, multiples of 8 and at least 8.
    uint32_t dummy_bits;                        ///< dummy field bits, multiples of 8 and at least 8.
    uint32_t queue_size;                        ///< Transaction queue size. This sets how many transactions can be 'in the air' (queued using spi_slave_hd_queue_trans but not yet finished using spi_slave_hd_get_trans_result) at the same time
    spi_dma_chan_t dma_chan;                    ///< DMA channel to used.
    spi_slave_hd_callback_config_t cb_config;   ///< Callback configuration
} spi_slave_hd_slot_config_t;

/**
 * @brief Initialize the SPI Slave HD driver.
 *
 * @param host_id       The host to use
 * @param bus_config    Bus configuration for the bus used
 * @param config        Configuration for the SPI Slave HD driver
 * @return
 *  - ESP_OK:                on success
 *  - ESP_ERR_INVALID_ARG:   invalid argument given
 *  - ESP_ERR_INVALID_STATE: function called in invalid state, may be some resources are already in use
 *  - ESP_ERR_NOT_FOUND      if there is no available DMA channel
 *  - ESP_ERR_NO_MEM:        memory allocation failed
 *  - or other return value from `esp_intr_alloc`
 */
esp_err_t spi_slave_hd_init(spi_host_device_t host_id, const spi_bus_config_t *bus_config,
                            const spi_slave_hd_slot_config_t *config);

/**
 * @brief Deinitialize the SPI Slave HD driver
 *
 * @param host_id The host to deinitialize the driver
 * @return
 *  - ESP_OK: on success
 *  - ESP_ERR_INVALID_ARG: if the host_id is not correct
 */
esp_err_t spi_slave_hd_deinit(spi_host_device_t host_id);

/**
 * @brief Queue transactions (segment mode)
 *
 * @param host_id   Host to queue the transaction
 * @param chan      SPI_SLAVE_CHAN_TX or SPI_SLAVE_CHAN_RX
 * @param trans     Transaction descriptors
 * @param timeout   Timeout before the data is queued
 * @return
 *  - ESP_OK: on success
 *  - ESP_ERR_INVALID_ARG: The input argument is invalid. Can be the following reason:
 *      - The buffer given is not DMA capable
 *      - The length of data is invalid (not larger than 0, or exceed the max transfer length)
 *      - The transaction direction is invalid
 *  - ESP_ERR_TIMEOUT: Cannot queue the data before timeout. Master is still processing previous transaction.
 *  - ESP_ERR_INVALID_STATE: Function called in invalid state. This API should be called under segment mode.
 */
esp_err_t spi_slave_hd_queue_trans(spi_host_device_t host_id, spi_slave_chan_t chan, spi_slave_hd_data_t* trans, TickType_t timeout);

/**
 * @brief Get the result of a data transaction (segment mode)
 *
 * @note This API should be called successfully the same times as the ``spi_slave_hd_queue_trans``.
 *
 * @param host_id   Host to queue the transaction
 * @param chan      Channel to get the result, SPI_SLAVE_CHAN_TX or SPI_SLAVE_CHAN_RX
 * @param[out] out_trans Pointer to the transaction descriptor (``spi_slave_hd_data_t``) passed to the driver before. Hardware has finished this transaction. Member ``trans_len`` indicates the actual number of bytes of received data, it's meaningless for TX.
 * @param timeout   Timeout before the result is got
 * @return
 *  - ESP_OK: on success
 *  - ESP_ERR_INVALID_ARG: Function is not valid
 *  - ESP_ERR_TIMEOUT: There's no transaction done before timeout
 *  - ESP_ERR_INVALID_STATE: Function called in invalid state. This API should be called under segment mode.
 */
esp_err_t spi_slave_hd_get_trans_res(spi_host_device_t host_id, spi_slave_chan_t chan, spi_slave_hd_data_t **out_trans, TickType_t timeout);

/**
 * @brief Read the shared registers
 *
 * @param host_id   Host to read the shared registers
 * @param addr      Address of register to read, 0 to ``SOC_SPI_MAXIMUM_BUFFER_SIZE-1``
 * @param[out] out_data Output buffer to store the read data
 * @param len       Length to read, not larger than ``SOC_SPI_MAXIMUM_BUFFER_SIZE-addr``
 */
void spi_slave_hd_read_buffer(spi_host_device_t host_id, int addr, uint8_t *out_data, size_t len);

/**
 * @brief Write the shared registers
 *
 * @param host_id   Host to write the shared registers
 * @param addr      Address of register to write, 0 to ``SOC_SPI_MAXIMUM_BUFFER_SIZE-1``
 * @param data      Buffer holding the data to write
 * @param len       Length to write, ``SOC_SPI_MAXIMUM_BUFFER_SIZE-addr``
 */
void spi_slave_hd_write_buffer(spi_host_device_t host_id, int addr, uint8_t *data, size_t len);

/**
 * @brief Load transactions (append mode)
 *
 * @note In this mode, user transaction descriptors will be appended to the DMA and the DMA will keep processing the data without stopping
 *
 * @param host_id   Host to load transactions
 * @param chan      SPI_SLAVE_CHAN_TX or SPI_SLAVE_CHAN_RX
 * @param trans     Transaction descriptor
 * @param timeout   Timeout before the transaction is loaded
 * @return
 *  - ESP_OK: on success
 *  - ESP_ERR_INVALID_ARG: The input argument is invalid. Can be the following reason:
 *      - The buffer given is not DMA capable
 *      - The length of data is invalid (not larger than 0, or exceed the max transfer length)
 *      - The transaction direction is invalid
 *  - ESP_ERR_TIMEOUT: Master is still processing previous transaction. There is no available transaction for slave to load
 *  - ESP_ERR_INVALID_STATE: Function called in invalid state. This API should be called under append mode.
 */
esp_err_t spi_slave_hd_append_trans(spi_host_device_t host_id, spi_slave_chan_t chan, spi_slave_hd_data_t *trans, TickType_t timeout);

/**
 * @brief Get the result of a data transaction (append mode)
 *
 * @note This API should be called the same times as the ``spi_slave_hd_append_trans``
 *
 * @param host_id   Host to load the transaction
 * @param chan      SPI_SLAVE_CHAN_TX or SPI_SLAVE_CHAN_RX
 * @param[out] out_trans Pointer to the transaction descriptor (``spi_slave_hd_data_t``) passed to the driver before. Hardware has finished this transaction. Member ``trans_len`` indicates the actual number of bytes of received data, it's meaningless for TX.
 * @param timeout   Timeout before the result is got
 * @return
 *  - ESP_OK: on success
 *  - ESP_ERR_INVALID_ARG: Function is not valid
 *  - ESP_ERR_TIMEOUT: There's no transaction done before timeout
 *  - ESP_ERR_INVALID_STATE: Function called in invalid state. This API should be called under append mode.
 */
esp_err_t spi_slave_hd_get_append_trans_res(spi_host_device_t host_id, spi_slave_chan_t chan, spi_slave_hd_data_t **out_trans, TickType_t timeout);

#ifdef __cplusplus
}
#endif
