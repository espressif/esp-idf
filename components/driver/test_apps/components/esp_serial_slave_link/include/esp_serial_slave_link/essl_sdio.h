/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// ESP SDIO slave link used by the ESP host to communicate with ESP SDIO slave.

#pragma once

#include "esp_err.h"
#include "driver/sdmmc_types.h"
#include "driver/sdmmc_host.h"

#include "esp_serial_slave_link/essl.h"
#include "essl_sdio_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Configuration for the ESSL SDIO device
typedef struct {
    sdmmc_card_t *card;     ///< The initialized sdmmc card pointer of the slave.
    int recv_buffer_size;   ///< The pre-negotiated recv buffer size used by both the host and the slave.
} essl_sdio_config_t;

/**
 * @brief Initialize the ESSL SDIO device and get its handle.
 *
 * @param out_handle Output of the handle.
 * @param config    Configuration for the ESSL SDIO device.
 * @return
 *  - ESP_OK: on success
 *  - ESP_ERR_NO_MEM: memory exhausted.
 */
esp_err_t essl_sdio_init_dev(essl_handle_t *out_handle, const essl_sdio_config_t *config);

/**
 * @brief Deinitialize and free the space used by the ESSL SDIO device.
 *
 * @param handle Handle of the ESSL SDIO device to deinit.
 * @return
 *  - ESP_OK: on success
 *  - ESP_ERR_INVALID_ARG: wrong handle passed
 */
esp_err_t essl_sdio_deinit_dev(essl_handle_t handle);

//Please call `essl_` functions witout `sdio` instead of calling these functions directly.
/** @cond */
/**
 * SDIO Initialize process of an ESSL SDIO slave device.
 *
 * @param arg Context of the ``essl`` component. Send to other functions later.
 * @param wait_ms Time to wait before operation is done, in ms.
 *
 * @return
 *      - ESP_OK if success
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t essl_sdio_init(void *arg, uint32_t wait_ms);

/**
 * Wait for interrupt of an ESSL SDIO slave device.
 *
 * @param arg Context of the ``essl`` component.
 * @param wait_ms Time to wait before operation is done, in ms.
 *
 * @return
 *      - ESP_OK if success
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t essl_sdio_wait_for_ready(void *arg, uint32_t wait_ms);

/**
 * Get buffer num for the host to send data to the slave. The buffers are size of ``buffer_size``.
 *
 * @param arg Context of the component.
 *
 * @return
 *      - ESP_OK Success
 *      - One of the error codes from SDMMC host controller
 */
uint32_t essl_sdio_get_tx_buffer_num(void *arg);

/** Get amount of data the ESSL SDIO slave preparing to send to host.
 *
 * @param arg Context of the component.
 *
 * @return
 *      - ESP_OK Success
 *      - One of the error codes from SDMMC host controller
 */
uint32_t essl_sdio_get_rx_data_size(void *arg);

/**
 * Send a packet to the ESSL SDIO slave. The slave receive the packet into buffers whose size is ``buffer_size`` in the arg.
 *
 * @param arg Context of the component.
 * @param start Start address of the packet to send
 * @param length Length of data to send, if the packet is over-size, the it will be divided into blocks and hold into different buffers automatically.
 * @param wait_ms Time to wait before timeout, in ms.
 *
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_TIMEOUT No buffer to use, or error ftrom SDMMC host controller
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t essl_sdio_send_packet(void *arg, const void *start, size_t length, uint32_t wait_ms);

/**
 * Get a packet from an ESSL SDIO slave.
 *
 * @param arg Context of the component.
 * @param[out] out_data Data output address
 * @param size The size of the output buffer, if the buffer is smaller than the size of data to receive from slave, the driver returns ``ESP_ERR_NOT_FINISHED``
 * @param wait_ms Time to wait before timeout, in ms.
 *
 * @return
 *     - ESP_OK Success, all the data are read from the slave.
 *     - ESP_ERR_NOT_FINISHED Read success, while there're data remaining.
 *     - One of the error codes from SDMMC host controller
 */
esp_err_t essl_sdio_get_packet(void *arg, void *out_data, size_t size, uint32_t wait_ms);

/**
 * Wait for the interrupt from the SDIO slave.
 *
 * @param arg Context of the component.
 * @param wait_ms Time to wait before timeout, in ms.
 * @return
 *  - ESP_ERR_NOT_SUPPORTED: if the interrupt line is not initialized properly.
 *  - ESP_OK: if interrupt happened
 *  - ESP_ERR_TIMEOUT: if timeout before interrupt happened.
 *  - or other values returned from the `io_int_wait` member of the `card->host` structure.
 */
esp_err_t essl_sdio_wait_int(void *arg, uint32_t wait_ms);

/**
 * Clear interrupt bits of an ESSL SDIO slave. All the bits set in the mask will be cleared, while other bits will stay the same.
 *
 * @param arg Context of the component.
 * @param intr_mask Mask of interrupt bits to clear.
 * @param wait_ms Time to wait before timeout, in ms.
 *
 * @return
 *      - ESP_OK Success
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t essl_sdio_clear_intr(void *arg, uint32_t intr_mask, uint32_t wait_ms);

/**
 * Get interrupt bits of an ESSL SDIO slave.
 *
 * @param arg Context of the component.
 * @param intr_raw Output of the raw interrupt bits. Set to NULL if only masked bits are read.
 * @param intr_st Output of the masked interrupt bits. set to NULL if only raw bits are read.
 * @param wait_ms Time to wait before timeout, in ms.
 *
 * @return
 *      - ESP_OK Success
 *      - ESP_INVALID_ARG   if both ``intr_raw`` and ``intr_st`` are NULL.
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t essl_sdio_get_intr(void *arg, uint32_t *intr_raw, uint32_t *intr_st, uint32_t wait_ms);

/**
 * Set interrupt enable bits of an ESSL SDIO slave. The slave only sends interrupt on the line when there is a bit both the raw status and the enable are set.
 *
 * @param arg Context of the component.
 * @param ena_mask Mask of the interrupt bits to enable.
 * @param wait_ms Time to wait before timeout, in ms.
 *
 * @return
 *      - ESP_OK Success
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t essl_sdio_set_intr_ena(void *arg, uint32_t ena_mask, uint32_t wait_ms);

/**
 * Get interrupt enable bits of an ESSL SDIO slave.
 *
 * @param arg Context of the component.
 * @param ena_mask_o Output of interrupt bit enable mask.
 * @param wait_ms Time to wait before timeout, in ms.
 *
 * @return
 *      - ESP_OK Success
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t essl_sdio_get_intr_ena(void *arg, uint32_t *ena_mask_o, uint32_t wait_ms);

/**
 * Write general purpose R/W registers (8-bit) of an ESSL SDIO slave.
 *
 * @param arg Context of the component.
 * @param addr Address of register to write. Valid address: 0-27, 32-63 (28-31 reserved).
 * @param value Value to write to the register.
 * @param wait_ms Time to wait before timeout, in ms.
 *
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_ARG Address not valid.
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t essl_sdio_write_reg(void *arg, uint8_t addr, uint8_t value, uint8_t *value_o, uint32_t wait_ms);

/**
 * Read general purpose R/W registers (8-bit) of an ESSL SDIO slave.
 *
 * @param arg Context of the component.
 * @param add Address of register to read. Valid address: 0-27, 32-63 (28-31 reserved, return interrupt bits on read).
 * @param value Output value read from the register.
 * @param wait_ms Time to wait before timeout, in ms.
 *
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_ARG Address not valid.
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t essl_sdio_read_reg(void *arg, uint8_t add, uint8_t *value_o, uint32_t wait_ms);

/**
 * Send interrupts to slave. Each bit of the interrupt will be triggered.
 *
 * @param arg Context of the component.
 * @param intr_mask Mask of interrupt bits to send to slave.
 * @param wait_ms Time to wait before timeout, in ms.
 *
 * @return
 *      - ESP_OK Success
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t essl_sdio_send_slave_intr(void *arg, uint32_t intr_mask, uint32_t wait_ms);

/**
 * @brief Reset the counter on the host side.
 *
 * @note Only call when you know the slave has reset its counter, or there will be inconsistent between the master and the slave.
 *
 * @param arg  Context of the component.
 */
void essl_sdio_reset_cnt(void *arg);

/** @endcond */

#ifdef __cplusplus
}
#endif
