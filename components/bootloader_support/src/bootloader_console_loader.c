/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * This file contains console-related functions which should be located in iram_loader_seg,
 * to be available in the "loader" phase, when iram_seg may be overwritten.
 */
#include <stdint.h>
#include <stddef.h>
#include "sdkconfig.h"
#include "bootloader_console.h"
#include "esp_rom_uart.h"
#include "esp_rom_sys.h"
#if CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/usb/chip_usb_dw_wrapper.h"
#include "esp32s2/rom/usb/usb_dc.h"
#include "esp32s2/rom/usb/cdc_acm.h"
#include "esp32s2/rom/usb/usb_persist.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/usb/chip_usb_dw_wrapper.h"
#include "esp32s3/rom/usb/usb_dc.h"
#include "esp32s3/rom/usb/cdc_acm.h"
#include "esp32s3/rom/usb/usb_persist.h"
#endif

#ifdef CONFIG_ESP_CONSOLE_USB_CDC
/* The following functions replace esp_rom_uart_putc, esp_rom_uart_tx_one_char,
 * and uart_tx_one_char_uart ROM functions. The main difference is that
 * uart_tx_one_char_uart calls cdc_acm_fifo_fill for each byte passed to it,
 * which results in very slow console output. The version here uses a TX buffer.
 * It also doesn't handle UART output, only works with USB.
 */
static char cdc_txbuf[ACM_BYTES_PER_TX];
static size_t cdc_txpos;

static void bootloader_console_flush_usb(void)
{
    cdc_acm_fifo_fill(uart_acm_dev, (const uint8_t *) cdc_txbuf, cdc_txpos);
    /* return value ignored — if bootloader fails to log something, proceed anyway */
    cdc_txpos = 0;
}

static void bootloader_console_write_one_char_usb(char ch)
{
    cdc_txbuf[cdc_txpos++] = ch;
    if (ch == '\n' || cdc_txpos == sizeof(cdc_txbuf)) {
        bootloader_console_flush_usb();
    }
}

void bootloader_console_write_char_usb(char c)
{
    if (c == '\n') {
        bootloader_console_write_one_char_usb('\r');
        bootloader_console_write_one_char_usb('\n');
    } else if (c == '\r') {
    } else {
        bootloader_console_write_one_char_usb(c);
    }
}
#endif //CONFIG_ESP_CONSOLE_USB_CDC

void bootloader_console_deinit(void)
{
#ifdef CONFIG_ESP_CONSOLE_UART
    /* Ensure any buffered log output is displayed */
    esp_rom_output_flush_tx(CONFIG_ESP_CONSOLE_ROM_SERIAL_PORT_NUM);
#endif // CONFIG_ESP_CONSOLE_UART

#ifdef CONFIG_ESP_CONSOLE_USB_CDC
    bootloader_console_flush_usb();
    usb_dc_prepare_persist();
    chip_usb_set_persist_flags(USBDC_PERSIST_ENA);
    esp_rom_delay_us(100);
    for (int i = 0; i < 10; i++) {
        usb_dc_check_poll_for_interrupts();
    }
    esp_rom_install_channel_putc(1, NULL);
#endif
}
