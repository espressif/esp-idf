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

////////////////////////////////////////////////////////////////////////////////
// Basic commands to communicate with the SPI Slave HD on ESP32-S2
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Read the shared buffer from the slave.
 *
 * @note ``out_data`` should be prepared in words and in the DRAM. The buffer may be written in words
 *       by the DMA. When a byte is written, the remaining bytes in the same word will also be
 *       overwritten, even the ``len`` is shorter than a word.
 *
 * @param spi       SPI device handle representing the slave
 * @param out_data  Buffer for read data, strongly suggested to be in the DRAM and align to 4
 * @param addr      Address of the slave shared buffer
 * @param len       Length to read
 * @param flags     `SPI_TRANS_*` flags to control the transaction mode of the transaction to send.
 * @return
 *      - ESP_OK: on success
 *      - or other return value from :cpp:func:`spi_device_transmit`.
 */
esp_err_t essl_spi_rdbuf(spi_device_handle_t spi, uint8_t *out_data, int addr, int len, uint32_t flags);

/**
 * @brief Write the shared buffer of the slave.
 *
 * @note ``out_data`` should be prepared in words and in the DRAM. The buffer may be written in words
 *       by the DMA. When a byte is written, the remaining bytes in the same word will also be
 *       overwritten, even the ``len`` is shorter than a word.
 *
 * @param spi       SPI device handle representing the slave
 * @param data      Buffer for data to send, strongly suggested to be in the DRAM and align to 4
 * @param addr      Address of the slave shared buffer,
 * @param len       Length to write
 * @param flags     `SPI_TRANS_*` flags to control the transaction mode of the transaction to send.
 * @return
 *      - ESP_OK: success
 *      - or other return value from :cpp:func:`spi_device_transmit`.
 */
esp_err_t essl_spi_wrbuf(spi_device_handle_t spi, const uint8_t *data, int addr, int len, uint32_t flags);

/**
 * @brief Receive long buffer in segments from the slave through its DMA.
 *
 * @note This function combines several :cpp:func:`essl_spi_rddma_seg` and one
 *       :cpp:func:`essl_spi_rddma_done` at the end. Used when the slave is working in segment mode.
 *
 * @param spi       SPI device handle representing the slave
 * @param out_data  Buffer to hold the received data, strongly suggested to be in the DRAM and align to 4
 * @param len       Total length of data to receive.
 * @param seg_len Length of each segment, which is not larger than the maximum transaction length
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
 * @param spi       SPI device handle representing the slave
 * @param out_data  Buffer to hold the received data, strongly suggested to be in the DRAM and align to 4
 * @param seg_len   Length of this segment
 * @param flags     `SPI_TRANS_*` flags to control the transaction mode of the transaction to send.
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
 * @param data      Buffer for data to send, strongly suggested to be in the DRAM and align to 4
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
 * @param data      Buffer for data to send, strongly suggested to be in the DRAM and align to 4
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