// Copyright 2010-2020 Espressif Systems (Shanghai) PTE LTD
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

#pragma once

#include "esp_err.h"
#include "esp_serial_slave_link/essl.h"


#ifdef __cplusplus
extern "C"
{
#endif

/// Configuration of ESSL SPI device
typedef struct {
    spi_device_handle_t *spi;           ///< Pointer to SPI device handle.
    uint32_t            tx_buf_size;    ///< The pre-negotiated Master TX buffer size used by both the host and the slave.
    uint8_t             tx_sync_reg;    ///< The pre-negotiated register ID for Master-TX-SLAVE-RX synchronization. 1 word (4 Bytes) will be reserved for the synchronization.
    uint8_t             rx_sync_reg;    ///< The pre-negotiated register ID for Master-RX-Slave-TX synchronization. 1 word (4 Bytes) will be reserved for the synchronization.
} essl_spi_config_t;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// APIs for DMA Append Mode
// This mode has a better performance for continuous Half Duplex SPI transactions.
//
// * You can use the ``essl_spi_init_dev`` and ``essl_spi_deinit_dev`` together with APIs in ``essl.h`` to communicate
//   with ESP SPI Slaves in Half Duplex DMA Append Mode. See example for SPI SLAVE HALFDUPLEX APPEND MODE.
// * You can also use the following APIs to create your own logic.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Initialize the ESSL SPI device function list and get its handle
 *
 * @param[out] out_handle    Output of the handle
 * @param      init_config   Configuration for the ESSL SPI device
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_NO_MEM:        Memory exhausted
 *        - ESP_ERR_INVALID_STATE: SPI driver is not initialized
 *        - ESP_ERR_INVALID_ARG:   Wrong register ID
 */
esp_err_t essl_spi_init_dev(essl_handle_t *out_handle, const essl_spi_config_t *init_config);

/**
 * @brief Deinitialize the ESSL SPI device and free the memory used by the device
 *
 * @param handle    Handle of the ESSL SPI device
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_INVALID_STATE: ESSL SPI is not in use
 */
esp_err_t essl_spi_deinit_dev(essl_handle_t handle);

/**
 * @brief Read from the shared registers
 *
 * @note The registers for Master/Slave synchronization are reserved. Do not use them. (see `rx_sync_reg` in `essl_spi_config_t`)
 *
 * @param      arg        Context of the component. (Member ``arg`` from ``essl_handle_t``)
 * @param      addr       Address of the shared registers. (Valid: 0 ~ SOC_SPI_MAXIMUM_BUFFER_SIZE, registers for M/S sync are reserved, see note1).
 * @param[out] out_value  Read buffer for the shared registers.
 * @param      wait_ms    Time to wait before timeout (reserved for future use, user should set this to 0).
 * @return
 *        - ESP_OK:                success
 *        - ESP_ERR_INVALID_STATE: ESSL SPI has not been initialized.
 *        - ESP_ERR_INVALID_ARG:   The address argument is not valid. See note 1.
 *        - or other return value from :cpp:func:`spi_device_transmit`.
 */
esp_err_t essl_spi_read_reg(void *arg, uint8_t addr, uint8_t *out_value, uint32_t wait_ms);

/**
 * @brief Get a packet from Slave
 *
 * @param      arg         Context of the component. (Member ``arg`` from ``essl_handle_t``)
 * @param[out] out_data    Output data address
 * @param      size        The size of the output data.
 * @param      wait_ms     Time to wait before timeout (reserved for future use, user should set this to 0).
 * @return
 *        - ESP_OK:                On Success
 *        - ESP_ERR_INVALID_STATE: ESSL SPI has not been initialized.
 *        - ESP_ERR_INVALID_ARG:   The output data address is neither DMA capable nor 4 byte-aligned
 *        - ESP_ERR_INVALID_SIZE:  Master requires ``size`` bytes of data but Slave did not load enough bytes.
 */
esp_err_t essl_spi_get_packet(void *arg, void *out_data, size_t size, uint32_t wait_ms);

/**
 * @brief Write to the shared registers
 *
 * @note The registers for Master/Slave synchronization are reserved. Do not use them. (see `tx_sync_reg` in `essl_spi_config_t`)
 * @note Feature of checking the actual written value (``out_value``) is not supported.
 *
 * @param      arg        Context of the component. (Member ``arg`` from ``essl_handle_t``)
 * @param      addr       Address of the shared registers. (Valid: 0 ~ SOC_SPI_MAXIMUM_BUFFER_SIZE, registers for M/S sync are reserved, see note1)
 * @param      value      Buffer for data to send, should be align to 4.
 * @param[out] out_value  Not supported, should be set to NULL.
 * @param      wait_ms    Time to wait before timeout (reserved for future use, user should set this to 0).
 * @return
 *        - ESP_OK:                success
 *        - ESP_ERR_INVALID_STATE: ESSL SPI has not been initialized.
 *        - ESP_ERR_INVALID_ARG:   The address argument is not valid. See note 1.
 *        - ESP_ERR_NOT_SUPPORTED: Should set ``out_value`` to NULL. See note 2.
 *        - or other return value from :cpp:func:`spi_device_transmit`.
 *
 */
esp_err_t essl_spi_write_reg(void *arg, uint8_t addr, uint8_t value, uint8_t *out_value, uint32_t wait_ms);

/**
 * @brief Send a packet to Slave
 *
 * @param arg        Context of the component. (Member ``arg`` from ``essl_handle_t``)
 * @param data       Address of the data to send
 * @param size       Size of the data to send.
 * @param wait_ms    Time to wait before timeout (reserved for future use, user should set this to 0).
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_INVALID_STATE: ESSL SPI has not been initialized.
 *        - ESP_ERR_INVALID_ARG:   The data address is not DMA capable
 *        - ESP_ERR_INVALID_SIZE:  Master will send ``size`` bytes of data but Slave did not load enough RX buffer
 */
esp_err_t essl_spi_send_packet(void *arg, const void *data, size_t size, uint32_t wait_ms);

/**
 * @brief Reset the counter in Master context
 *
 * @note Shall only be called if the slave has reset its counter. Else, Slave and Master would be desynchronized
 *
 * @param arg    Context of the component. (Member ``arg`` from ``essl_handle_t``)
 */
void essl_spi_reset_cnt(void *arg);

////////////////////////////////////////////////////////////////////////////////
// Basic commands to communicate with the SPI Slave HD on ESP32-S2
////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Read the shared buffer from the slave in ISR way
 *
 * @note The slave's HW doesn't guarantee the data in one SPI transaction is consistent. It sends data in unit of byte.
 * In other words, if the slave SW attempts to update the shared register when a rdbuf SPI transaction is in-flight,
 * the data got by the master will be the combination of bytes of different writes of slave SW.
 *
 * @note ``out_data`` should be prepared in words and in the DRAM. The buffer may be written in words
 *       by the DMA. When a byte is written, the remaining bytes in the same word will also be
 *       overwritten, even the ``len`` is shorter than a word.
 *
 * @param      spi       SPI device handle representing the slave
 * @param[out] out_data  Buffer for read data, strongly suggested to be in the DRAM and aligned to 4
 * @param      addr      Address of the slave shared buffer
 * @param      len       Length to read
 * @param      flags     `SPI_TRANS_*` flags to control the transaction mode of the transaction to send.
 * @return
 *      - ESP_OK: on success
 *      - or other return value from :cpp:func:`spi_device_transmit`.
 */
esp_err_t essl_spi_rdbuf(spi_device_handle_t spi, uint8_t *out_data, int addr, int len, uint32_t flags);

/**
 * @brief Read the shared buffer from the slave in polling way
 *
 * @note ``out_data`` should be prepared in words and in the DRAM. The buffer may be written in words
 *       by the DMA. When a byte is written, the remaining bytes in the same word will also be
 *       overwritten, even the ``len`` is shorter than a word.
 *
 * @param      spi       SPI device handle representing the slave
 * @param[out] out_data  Buffer for read data, strongly suggested to be in the DRAM and aligned to 4
 * @param      addr      Address of the slave shared buffer
 * @param      len       Length to read
 * @param      flags     `SPI_TRANS_*` flags to control the transaction mode of the transaction to send.
 * @return
 *      - ESP_OK: on success
 *      - or other return value from :cpp:func:`spi_device_transmit`.
 */
esp_err_t essl_spi_rdbuf_polling(spi_device_handle_t spi, uint8_t *out_data, int addr, int len, uint32_t flags);

/**
 * @brief Write the shared buffer of the slave in ISR way
 *
 * @note ``out_data`` should be prepared in words and in the DRAM. The buffer may be written in words
 *       by the DMA. When a byte is written, the remaining bytes in the same word will also be
 *       overwritten, even the ``len`` is shorter than a word.
 *
 * @param spi       SPI device handle representing the slave
 * @param data      Buffer for data to send, strongly suggested to be in the DRAM
 * @param addr      Address of the slave shared buffer,
 * @param len       Length to write
 * @param flags     `SPI_TRANS_*` flags to control the transaction mode of the transaction to send.
 * @return
 *      - ESP_OK: success
 *      - or other return value from :cpp:func:`spi_device_transmit`.
 */
esp_err_t essl_spi_wrbuf(spi_device_handle_t spi, const uint8_t *data, int addr, int len, uint32_t flags);

/**
 * @brief Write the shared buffer of the slave in polling way
 *
 * @note ``out_data`` should be prepared in words and in the DRAM. The buffer may be written in words
 *       by the DMA. When a byte is written, the remaining bytes in the same word will also be
 *       overwritten, even the ``len`` is shorter than a word.
 *
 * @param spi       SPI device handle representing the slave
 * @param data      Buffer for data to send, strongly suggested to be in the DRAM
 * @param addr      Address of the slave shared buffer,
 * @param len       Length to write
 * @param flags     `SPI_TRANS_*` flags to control the transaction mode of the transaction to send.
 * @return
 *      - ESP_OK: success
 *      - or other return value from :cpp:func:`spi_device_polling_transmit`.
 */
esp_err_t essl_spi_wrbuf_polling(spi_device_handle_t spi, const uint8_t *data, int addr, int len, uint32_t flags);

/**
 * @brief Receive long buffer in segments from the slave through its DMA.
 *
 * @note This function combines several :cpp:func:`essl_spi_rddma_seg` and one
 *       :cpp:func:`essl_spi_rddma_done` at the end. Used when the slave is working in segment mode.
 *
 * @param      spi       SPI device handle representing the slave
 * @param[out] out_data  Buffer to hold the received data, strongly suggested to be in the DRAM and aligned to 4
 * @param      len       Total length of data to receive.
 * @param      seg_len Length of each segment, which is not larger than the maximum transaction length
 *                allowed for the spi device. Suggested to be multiples of 4. When set < 0, means send
 *                all data in one segment (the ``rddma_done`` will still be sent.)
 * @param flags     `SPI_TRANS_*` flags to control the transaction mode of the transaction to send.
 * @return
 *      - ESP_OK: success
 *      - or other return value from :cpp:func:`spi_device_transmit`.
 */
esp_err_t essl_spi_rddma(spi_device_handle_t spi, uint8_t *out_data, int len, int seg_len, uint32_t flags);

/**
 * @brief Read one data segment from the slave through its DMA.
 *
 * @note To read long buffer, call :cpp:func:`essl_spi_rddma` instead.
 *
 * @param      spi       SPI device handle representing the slave
 * @param[out] out_data  Buffer to hold the received data. strongly suggested to be in the DRAM and aligned to 4
 * @param      seg_len   Length of this segment
 * @param      flags     `SPI_TRANS_*` flags to control the transaction mode of the transaction to send.
 * @return
 *      - ESP_OK: success
 *      - or other return value from :cpp:func:`spi_device_transmit`.
 */
esp_err_t essl_spi_rddma_seg(spi_device_handle_t spi, uint8_t *out_data, int seg_len, uint32_t flags);

/**
 * @brief Send the ``rddma_done`` command to the slave. Upon receiving this command, the slave will
 *        stop sending the current buffer even there are data unsent, and maybe prepare the next buffer to
 *        send.
 *
 * @note This is required only when the slave is working in segment mode.
 *
 * @param spi       SPI device handle representing the slave
 * @param flags     `SPI_TRANS_*` flags to control the transaction mode of the transaction to send.
 * @return
 *      - ESP_OK: success
 *      - or other return value from :cpp:func:`spi_device_transmit`.
 */
esp_err_t essl_spi_rddma_done(spi_device_handle_t spi, uint32_t flags);

/**
 * @brief Send long buffer in segments to the slave through its DMA.
 *
 * @note This function combines several :cpp:func:`essl_spi_wrdma_seg` and one
 *       :cpp:func:`essl_spi_wrdma_done` at the end. Used when the slave is working in segment mode.
 *
 * @param spi       SPI device handle representing the slave
 * @param data      Buffer for data to send, strongly suggested to be in the DRAM
 * @param len       Total length of data to send.
 * @param seg_len Length of each segment, which is not larger than the maximum transaction length
 *                allowed for the spi device. Suggested to be multiples of 4. When set < 0, means send
 *                all data in one segment (the ``wrdma_done`` will still be sent.)
 * @param flags     `SPI_TRANS_*` flags to control the transaction mode of the transaction to send.
 * @return
 *      - ESP_OK: success
 *      - or other return value from :cpp:func:`spi_device_transmit`.
 */
esp_err_t essl_spi_wrdma(spi_device_handle_t spi, const uint8_t *data, int len, int seg_len, uint32_t flags);

/**
 * @brief Send one data segment to the slave through its DMA.
 *
 * @note To send long buffer, call :cpp:func:`essl_spi_wrdma` instead.
 *
 * @param spi       SPI device handle representing the slave
 * @param data      Buffer for data to send, strongly suggested to be in the DRAM
 * @param seg_len   Length of this segment
 * @param flags     `SPI_TRANS_*` flags to control the transaction mode of the transaction to send.
 * @return
 *      - ESP_OK: success
 *      - or other return value from :cpp:func:`spi_device_transmit`.
 */
esp_err_t essl_spi_wrdma_seg(spi_device_handle_t spi, const uint8_t *data, int seg_len, uint32_t flags);

/**
 * @brief Send the ``wrdma_done`` command to the slave. Upon receiving this command, the slave will
 *        stop receiving, process the received data, and maybe prepare the next buffer to receive.
 *
 * @note This is required only when the slave is working in segment mode.
 *
 * @param spi       SPI device handle representing the slave
 * @param flags     `SPI_TRANS_*` flags to control the transaction mode of the transaction to send.
 * @return
 *      - ESP_OK: success
 *      - or other return value from :cpp:func:`spi_device_transmit`.
 */
esp_err_t essl_spi_wrdma_done(spi_device_handle_t spi, uint32_t flags);

#ifdef __cplusplus
}
#endif
