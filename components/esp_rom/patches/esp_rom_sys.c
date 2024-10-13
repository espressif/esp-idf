/*
 * SPDX-FileCopyrightText: 2010-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "soc/soc_caps.h"
#include "esp_rom_caps.h"
#include "esp_rom_uart.h"
#include "rom/ets_sys.h"
#include "sdkconfig.h"

#if !ESP_ROM_HAS_OUTPUT_PUTC_FUNC
void esp_rom_output_putc(char c)
{
    if (c == '\n') {
        esp_rom_output_tx_one_char('\r');
        esp_rom_output_tx_one_char('\n');
    } else if (c == '\r') {
    } else {
      esp_rom_output_tx_one_char(c);
    }
}
#endif // !ESP_ROM_HAS_OUTPUT_PUTC_FUNC

#if !ESP_ROM_HAS_OUTPUT_TO_CHANNELS_FUNC
void (* _putc1)(char c) = esp_rom_output_putc;
void (* _putc2)(char c) = NULL;

void esp_rom_output_to_channels(char c)
{
    if (_putc1) {
        _putc1(c);
    }

    if (_putc2) {
        _putc2(c);
    }
}
#endif // !ESP_ROM_HAS_OUTPUT_TO_CHANNELS_FUNC

void esp_rom_install_channel_putc(int channel, void (*putc)(char c))
{
    switch (channel) {
    case 1:
#if !ESP_ROM_HAS_OUTPUT_TO_CHANNELS_FUNC
        _putc1 = putc;
#endif
#if !CONFIG_IDF_TARGET_LINUX
        ets_install_putc1(putc);
#endif
        break;
    case 2:
#if !ESP_ROM_HAS_OUTPUT_TO_CHANNELS_FUNC
        _putc2 = putc;
#endif
#if !CONFIG_IDF_TARGET_LINUX
        ets_install_putc2(putc);
#endif
        break;
    default:
        break;
    }
}

#if ESP_ROM_HAS_ETS_PRINTF_BUG
void esp_rom_install_uart_printf(void)
{
#if !ESP_ROM_HAS_OUTPUT_TO_CHANNELS_FUNC
    _putc1 = esp_rom_output_putc;
#endif
#if !CONFIG_IDF_TARGET_LINUX
    extern void ets_install_uart_printf(void);
    extern bool g_uart_print;
    extern bool g_usb_print;
    // If ROM log is disabled permanently via eFuse or temporarily via RTC storage register,
    // this ROM symbol will be set to false, and cause ``esp_rom_printf`` can't work on esp-idf side.
    g_uart_print = true;
    g_usb_print = true;
    ets_install_uart_printf();
#endif // !CONFIG_IDF_TARGET_LINUX
}
#endif

#if CONFIG_IDF_TARGET_ESP32
extern uint32_t g_ticks_per_us_pro;
#if SOC_CPU_CORES_NUM > 1
#ifndef CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
extern uint32_t g_ticks_per_us_app;
#endif
#endif
void esp_rom_set_cpu_ticks_per_us(uint32_t ticks_per_us)
{
    /* Update scale factors used by esp_rom_delay_us */
    g_ticks_per_us_pro = ticks_per_us;
#if SOC_CPU_CORES_NUM > 1
#ifndef CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    g_ticks_per_us_app = ticks_per_us;
#endif
#endif
}
#endif // CONFIG_IDF_TARGET_ESP32
