/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Configuration structure for the usb-serial-jtag-driver. Can be expanded in the future
 *
 * @note  tx_buffer_size and rx_buffer_size must be > 0
 */
typedef struct {
    uint32_t tx_buffer_size;                    /* Size of the buffer (in bytes) for the TX direction */
    uint32_t rx_buffer_size;                    /* Size of the buffer (in bytes) for the RX direction */
} usb_serial_jtag_driver_config_t;

#define USB_SERIAL_JTAG_DRIVER_CONFIG_DEFAULT() (usb_serial_jtag_driver_config_t) {\
    .tx_buffer_size = 256,\
    .rx_buffer_size = 256,\
}

/**
 * @brief Install USB-SERIAL-JTAG driver and set the USB-SERIAL-JTAG to the default configuration.
 *
 * USB-SERIAL-JTAG driver's ISR will be attached to the same CPU core that calls this function. Thus, users
 * should ensure that the same core is used when calling `usb_serial_jtag_driver_uninstall()`.
 *
 * @note  Blocking mode will result in usb_serial_jtag_write_bytes() blocking for a
 * short period if the TX FIFO if full. It will not block again until the buffer
 * has some space available again.
 *
 * @param usb_serial_jtag_driver_config_t Configuration for usb_serial_jtag driver.
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Failed for some reason.
 */
esp_err_t usb_serial_jtag_driver_install(usb_serial_jtag_driver_config_t *usb_serial_jtag_config);

/**
 * @brief USB_SERIAL_JTAG read bytes from USB_SERIAL_JTAG buffer
 *
 * @param buf     pointer to the buffer.
 * @param length  data length
 * @param ticks_to_wait Timeout in RTOS ticks
 *
 * @return
 *     - The number of bytes read from USB_SERIAL FIFO
 */
int usb_serial_jtag_read_bytes(void* buf, uint32_t length, TickType_t ticks_to_wait);

/**
 * @brief Send data to the USB-UART port from a given buffer and length,
 *
 * Please ensure the `tx_buffer_size is larger than 0`, if the 'tx_buffer_size' > 0, this function will return after copying all the data to tx ring buffer,
 * USB_SERIAL_JTAG ISR will then move data from the ring buffer to TX FIFO gradually.
 *
 * @param src   data buffer address
 * @param size  data length to send
 * @param ticks_to_wait Maximum timeout in RTOS ticks
 *
 * @return
 *     - The number of bytes pushed to the TX FIFO
 */
int usb_serial_jtag_write_bytes(const void* src, size_t size, TickType_t ticks_to_wait);

/**
 * @brief Uninstall USB-SERIAL-JTAG driver.
 *
 * @return
 *     - ESP_OK   Success
 */
esp_err_t usb_serial_jtag_driver_uninstall(void);

#ifdef __cplusplus
}
#endif
