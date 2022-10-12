/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include "esp_attr.h"

#include "esp_rom_caps.h"

IRAM_ATTR void esp_rom_install_channel_putc(int channel, void (*putc)(char c))
{
    extern void ets_install_putc1(void (*p)(char c));
    extern void ets_install_putc2(void (*p)(char c));
    switch (channel) {
    case 1:
        ets_install_putc1(putc);
        break;
    case 2:
        ets_install_putc2(putc);
        break;
    default:
        break;
    }
}

#if ESP_ROM_HAS_ETS_PRINTF_BUG
IRAM_ATTR void esp_rom_install_uart_printf(void)
{
    extern void ets_install_uart_printf(void);
    extern bool g_uart_print;
    extern bool g_usb_print;
    // If ROM log is disabled permanently via eFuse or temporarily via RTC storage register,
    // this ROM symbol will be set to false, and cause ``esp_rom_printf`` can't work on esp-idf side.
    g_uart_print = true;
    g_usb_print = true;
    ets_install_uart_printf();
}
#endif
