/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Wait for TX FIFO is empty and all data has been sent out.
 *
 * @param serial_num The serial number defined in ROM, including UART_x, USB_OTG, USB_SERIAL_JTAG.
 *                   Use ESP_ROM_USB_SERIAL_DEVICE_NUM or ESP_ROM_USB_OTG_NUM to identify USB_SERIAL_JTAG and USB_OTG, respectively.
 */
void esp_rom_output_tx_wait_idle(uint8_t serial_num);

/**
 * @brief Flush TX FIFO
 *
 * @param serial_num The serial number defined in ROM, including UART_x, USB_OTG, USB_SERIAL_JTAG.
 *                   Use ESP_ROM_USB_SERIAL_DEVICE_NUM or ESP_ROM_USB_OTG_NUM to identify USB_SERIAL_JTAG and USB_OTG, respectively.
 */
void esp_rom_output_flush_tx(uint8_t serial_num);

/**
 * @brief Transmit one character to the console channel.
 *
 * @param c Character to send
 * @return
 *      - 0 on success
 *      - 1 on failure
 */
int esp_rom_output_tx_one_char(uint8_t c);

/**
 * @brief Transmit one character to the console channel.
 * @note This function can help handle line ending issue by replacing '\n' with '\r\n'.
 *
 * @param c Character to send
 */
void esp_rom_output_putc(char c);

/**
 * @brief Get one character from the console channel.
 *
 * @param c Where to store the character
 * @return
 *      - 0 on success
 *      - 1 on failure or no data available
 */
int esp_rom_output_rx_one_char(uint8_t *c);

/**
 * @brief Get one line of string from console channel (line ending won't be stored in the buffer).
 *
 * @param str Where to store the string
 * @param max_len Maximum length of the buffer (including the NULL delimiter)
 * @return always return 0 when on success or wait in a loop for rx data
 */
int esp_rom_output_rx_string(uint8_t *str, uint8_t max_len);

/**
 * @brief Set the serial port used by ROM printf
 *
 * @param serial_num The serial number defined in ROM, including UART_x, USB_OTG, USB_SERIAL_JTAG.
 *                   Use ESP_ROM_USB_SERIAL_DEVICE_NUM or ESP_ROM_USB_OTG_NUM to identify USB_SERIAL_JTAG and USB_OTG, respectively.
 */
void esp_rom_output_set_as_console(uint8_t serial_num);

/**
 * @brief Switch the serial port that will use a buffer for TX and RX.
 *
 * @param serial_num The serial number defined in ROM, including UART_x, USB_OTG, USB_SERIAL_JTAG.
 *                   Use ESP_ROM_USB_SERIAL_DEVICE_NUM or ESP_ROM_USB_OTG_NUM to identify USB_SERIAL_JTAG and USB_OTG, respectively.
 */
void esp_rom_output_switch_buffer(uint8_t serial_num);


#define ESP_ROM_CDC_ACM_WORK_BUF_MIN 128

/**
 * @brief Initialize the USB ACM
 * @note The ACM working memory should be at least 128 bytes (ESP_ROM_CDC_ACM_WORK_BUF_MIN) in size.
 *
 * @param cdc_acm_work_mem Pointer to the work memory used for CDC-ACM
 * @param cdc_acm_work_mem_len Length of work memory
 */
void esp_rom_output_usb_acm_init(void *cdc_acm_work_mem, int cdc_acm_work_mem_len);

#ifdef __cplusplus
}
#endif
