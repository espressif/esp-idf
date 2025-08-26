/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_system_console.h"
#include "sdkconfig.h"
#include "esp_rom_uart.h"
#include "esp_rom_usb_serial.h"
#include "esp_rom_caps.h"

void esp_system_console_put_char(char c)
{
    esp_rom_output_tx_one_char(c);
#if CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG && !ESP_ROM_CONSOLE_OUTPUT_SECONDARY
    esp_rom_usb_serial_putc(c);
#endif // CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG && !ESP_ROM_CONSOLE_OUTPUT_SECONDARY
}
