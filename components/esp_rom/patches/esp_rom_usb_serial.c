/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_rom_usb_serial.h"
#include <stdint.h>
#include "rom/ets_sys.h"
#include "rom/efuse.h"

extern ETS_STATUS usb_serial_device_tx_one_char(uint8_t TxChar);
extern void usb_serial_device_tx_flush(void);

void esp_rom_usb_serial_putc(char c)
{
    uint32_t print_enabled = !ets_efuse_usb_print_is_disabled();
    if (print_enabled) {
        usb_serial_device_tx_one_char(c);
        if (c == '\n') {
            usb_serial_device_tx_flush();
        }
    }
}
