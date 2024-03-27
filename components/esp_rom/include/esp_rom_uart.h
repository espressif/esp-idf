/*
 * SPDX-FileCopyrightText: 2010-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "hal/uart_ll.h"

#define ESP_ROM_CDC_ACM_WORK_BUF_MIN 128

typedef enum {
    ESP_ROM_UART_0,
    ESP_ROM_UART_1,
    ESP_ROM_UART_USB
} esp_rom_uart_num_t;

/**
 * @brief Wait for UART TX FIFO is empty and all data has been sent out.
 *
 * @param serial_num The serial number defined in ROM, including UART_x, USB_OTG, USB_SERIAL_JTAG..
 */
void esp_rom_output_tx_wait_idle(uint8_t serial_num);

void esp_rom_uart_tx_wait_idle(uint8_t serial_num) __attribute__((deprecated("Please use esp_rom_output_tx_wait_idle instead")));

/**
 * @brief Set clock source and baud rate for UART.
 *
 * @param serial_num UART port number
 * @param clock_hz Source clock (in Hz)
 * @param baud_rate Baud rate to set
 *
 * @note Only for HP UART
 */
#define esp_rom_uart_set_clock_baudrate(serial_num, clock_hz, baud_rate) uart_ll_set_baudrate(UART_LL_GET_HW(serial_num), baud_rate, clock_hz)

/**
 * @brief Wait until UART TX FIFO is empty (i.e. flush TX FIFO)
 *
 * @param serial_num UART port number
 */
void esp_rom_output_flush_tx(uint8_t serial_num);

void esp_rom_uart_flush_tx(uint8_t serial_num) __attribute__((deprecated("Please use esp_rom_output_flush_tx instead")));

/**
 * @brief Transmit one character to the console channel.
 *
 * @param c Character to send
 * @return
 *      - 0 on success
 *      - 1 on failure
 */
int esp_rom_output_tx_one_char(uint8_t c);

int esp_rom_uart_tx_one_char(uint8_t c) __attribute__((deprecated("Please use esp_rom_output_tx_one_char instead")));

/**
 * @brief Transmit one character to the console channel.
 * @note This function is a wrapper over esp_rom_uart_tx_one_char, it can help handle line ending issue by replacing '\n' with '\r\n'.
 *
 * @param c Character to send
 */
void esp_rom_output_putc(char c);

void esp_rom_uart_putc(char c) __attribute__((deprecated("Please use esp_rom_output_putc instead")));

/**
 * @brief Get one character from the console channel.
 *
 * @param c Where to store the character
 * @return
 *      - 0 on success
 *      - 1 on failure or no data available
 */
int esp_rom_output_rx_one_char(uint8_t *c);

int esp_rom_uart_rx_one_char(uint8_t *c) __attribute__((deprecated("Please use esp_rom_output_rx_one_char instead")));

/**
 * @brief Get one line of string from console channel (line ending won't be stored in the buffer).
 *
 * @param str Where to store the string
 * @param max_len Maximum length of the buffer (including the NULL delimiter)
 * @return always return 0 when on success or wait in a loop for rx data
 */
int esp_rom_output_rx_string(uint8_t *str, uint8_t max_len);

int esp_rom_uart_rx_string(uint8_t *str, uint8_t max_len) __attribute__((deprecated("Please use esp_rom_output_rx_string instead")));

/**
 * @brief Set the UART port used by ets_printf.
 *
 * @note USB-CDC port is also treated as "UART" port in the ROM code.
 *       Use ESP_ROM_USB_SERIAL_DEVICE_NUM or ESP_ROM_USB_OTG_NUM to identify USB_SERIAL_JTAG and USB_OTG, respectively.
 *
 * @param serial_num UART port number
 */
void esp_rom_output_set_as_console(uint8_t serial_num);

void esp_rom_uart_set_as_console(uint8_t serial_num) __attribute__((deprecated("Please use esp_rom_output_set_as_console instead")));

/**
 * @brief Switch the UART port that will use a buffer for TX and RX.
 *
 * @note USB-CDC port is also treated as "UART" port in the ROM code.
 *       Use ESP_ROM_USB_SERIAL_DEVICE_NUM or ESP_ROM_USB_OTG_NUM to identify USB_SERIAL_JTAG and USB_OTG, respectively.
 *
 * @param serial_num UART port number
 */
void esp_rom_output_switch_buffer(uint8_t serial_num);

void esp_rom_uart_switch_buffer(uint8_t serial_num) __attribute__((deprecated("Please use esp_rom_output_switch_buffer instead")));

/**
 * @brief Initialize the USB ACM UART
 * @note The ACM working memroy should be at least 128 bytes (ESP_ROM_CDC_ACM_WORK_BUF_MIN) in size.
 *
 * @param cdc_acm_work_mem Pointer to the work memroy used for CDC-ACM
 * @param cdc_acm_work_mem_len Length of work memory
 */
void esp_rom_output_usb_acm_init(void *cdc_acm_work_mem, int cdc_acm_work_mem_len);

void esp_rom_uart_usb_acm_init(uint8_t serial_num) __attribute__((deprecated("Please use esp_rom_output_usb_acm_init instead")));

#ifdef __cplusplus
}
#endif
