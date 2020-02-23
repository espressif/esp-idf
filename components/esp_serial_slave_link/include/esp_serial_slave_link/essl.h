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

#pragma once

#include "sdmmc_cmd.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"


#define ESP_ERR_NOT_FINISHED    0x201   ///< There is still remaining data.

struct essl_dev_t;
/// Handle of an ESSL device
typedef struct essl_dev_t* essl_handle_t;

/**
 * @brief Initialize the slave.
 *
 * @param handle Handle of a ``essl`` device.
 * @param wait_ms Millisecond to wait before timeout, will not wait at all if set to 0-9.
 * @return ESP_OK if success, or other value returned from lower layer `init`.
 */
esp_err_t essl_init(essl_handle_t handle, uint32_t wait_ms);

/** Wait for interrupt of a ESP32 slave device.
 *
 * @param handle Handle of a ``essl`` device.
 * @param wait_ms Millisecond to wait before timeout, will not wait at all if set to 0-9.
 *
 * @return
 *      - ESP_OK if success
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t essl_wait_for_ready(essl_handle_t handle, uint32_t wait_ms);

/** Get buffer num for the host to send data to the slave. The buffers are size of ``buffer_size``.
 *
 * @param handle Handle of a ``essl`` device.
 * @param out_tx_num Output of buffer num that host can send data to ESP32 slave.
 * @param wait_ms Millisecond to wait before timeout, will not wait at all if set to 0-9.
 *
 * @return
 *      - ESP_OK Success
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t essl_get_tx_buffer_num(essl_handle_t handle, uint32_t *out_tx_num, uint32_t wait_ms);

/** Get amount of data the ESP32 slave preparing to send to host.
 *
 * @param handle Handle of a ``essl`` device.
 * @param out_rx_size Output of data size to read from slave.
 * @param wait_ms Millisecond to wait before timeout, will not wait at all if set to 0-9.
 *
 * @return
 *      - ESP_OK Success
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t essl_get_rx_data_size(essl_handle_t handle, uint32_t *out_rx_size, uint32_t wait_ms);


/** Reset the counters of this component. Usually you don't need to do this unless you know the slave is reset.
 *
 * @param handle Handle of a ``essl`` device.
 */
esp_err_t essl_reset_cnt(essl_handle_t handle);

/** Send a packet to the ESP32 slave. The slave receive the packet into buffers whose size is ``buffer_size`` (configured during initialization).
 *
 * @param handle Handle of a ``essl`` device.
 * @param start Start address of the packet to send
 * @param length Length of data to send, if the packet is over-size, the it will be divided into blocks and hold into different buffers automatically.
 * @param wait_ms Millisecond to wait before timeout, will not wait at all if set to 0-9.
 *
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_TIMEOUT No buffer to use, or error ftrom SDMMC host controller
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t essl_send_packet(essl_handle_t handle, const void *start, size_t length, uint32_t wait_ms);

/** Get a packet from ESP32 slave.
 *
 * @param handle Handle of a ``essl`` device.
 * @param[out] out_data Data output address
 * @param size The size of the output buffer, if the buffer is smaller than the size of data to receive from slave, the driver returns ``ESP_ERR_NOT_FINISHED``
 * @param[out] out_length Output of length the data actually received from slave.
 * @param wait_ms Millisecond to wait before timeout, will not wait at all if set to 0-9.
 *
 * @return
 *     - ESP_OK Success, all the data are read from the slave.
 *     - ESP_ERR_NOT_FINISHED Read success, while there're data remaining.
 *     - One of the error codes from SDMMC host controller
 */
esp_err_t essl_get_packet(essl_handle_t handle, void *out_data, size_t size, size_t *out_length, uint32_t wait_ms);

/** Write general purpose R/W registers (8-bit) of ESP32 slave.
 *
 * @param handle Handle of a ``essl`` device.
 * @param addr Address of register to write. Valid address: 0-59.
 * @param value Value to write to the register.
 * @param value_o Output of the returned written value.
 * @param wait_ms Millisecond to wait before timeout, will not wait at all if set to 0-9.
 *
 * @note sdio 28-31 are reserved, the lower API helps to skip.
 *
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_ARG Address not valid.
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t essl_write_reg(essl_handle_t handle, uint8_t addr, uint8_t value, uint8_t *value_o, uint32_t wait_ms);

/** Read general purpose R/W registers (8-bit) of ESP32 slave.
 *
 * @param handle Handle of a ``essl`` device.
 * @param add Address of register to read. Valid address: 0-27, 32-63 (28-31 reserved, return interrupt bits on read).
 * @param value_o Output value read from the register.
 * @param wait_ms Millisecond to wait before timeout, will not wait at all if set to 0-9.
 *
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_ARG Address not valid.
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t essl_read_reg(essl_handle_t handle, uint8_t add, uint8_t *value_o, uint32_t wait_ms);

/** wait for an interrupt of the slave
 *
 * @param handle Handle of a ``essl`` device.
 * @param wait_ms Millisecond to wait before timeout, will not wait at all if set to 0-9.
 *
 * @return
 *      - ESP_ERR_NOT_SUPPORTED Currently our driver doesnot support SDIO with SPI interface.
 *      - ESP_OK If interrupt triggered.
 *      - ESP_ERR_TIMEOUT No interrupts before timeout.
 */
esp_err_t essl_wait_int(essl_handle_t handle, uint32_t wait_ms);

/** Clear interrupt bits of ESP32 slave. All the bits set in the mask will be cleared, while other bits will stay the same.
 *
 * @param handle Handle of a ``essl`` device.
 * @param intr_mask Mask of interrupt bits to clear.
 * @param wait_ms Millisecond to wait before timeout, will not wait at all if set to 0-9.
 *
 * @return
 *      - ESP_OK Success
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t essl_clear_intr(essl_handle_t handle, uint32_t intr_mask, uint32_t wait_ms);

/** Get interrupt bits of ESP32 slave.
 *
 * @param handle Handle of a ``essl`` device.
 * @param intr_raw Output of the raw interrupt bits. Set to NULL if only masked bits are read.
 * @param intr_st Output of the masked interrupt bits. set to NULL if only raw bits are read.
 * @param wait_ms Millisecond to wait before timeout, will not wait at all if set to 0-9.
 *
 * @return
 *      - ESP_OK Success
 *      - ESP_INVALID_ARG   if both ``intr_raw`` and ``intr_st`` are NULL.
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t essl_get_intr(essl_handle_t handle, uint32_t *intr_raw, uint32_t *intr_st, uint32_t wait_ms);

/** Set interrupt enable bits of ESP32 slave. The slave only sends interrupt on the line when there is a bit both the raw status and the enable are set.
 *
 * @param handle Handle of a ``essl`` device.
 * @param ena_mask Mask of the interrupt bits to enable.
 * @param wait_ms Millisecond to wait before timeout, will not wait at all if set to 0-9.
 *
 * @return
 *      - ESP_OK Success
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t essl_set_intr_ena(essl_handle_t handle, uint32_t ena_mask, uint32_t wait_ms);

/** Get interrupt enable bits of ESP32 slave.
 *
 * @param handle Handle of a ``essl`` device.
 * @param ena_mask_o Output of interrupt bit enable mask.
 * @param wait_ms Millisecond to wait before timeout, will not wait at all if set to 0-9.
 *
 * @return
 *      - ESP_OK Success
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t essl_get_intr_ena(essl_handle_t handle, uint32_t *ena_mask_o, uint32_t wait_ms);

/** Send interrupts to slave. Each bit of the interrupt will be triggered.
 *
 * @param handle Handle of a ``essl`` device.
 * @param intr_mask Mask of interrupt bits to send to slave.
 * @param wait_ms Millisecond to wait before timeout, will not wait at all if set to 0-9.
 *
 * @return
 *      - ESP_OK Success
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t essl_send_slave_intr(essl_handle_t handle, uint32_t intr_mask, uint32_t wait_ms);


