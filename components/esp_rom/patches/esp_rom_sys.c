/*
 * SPDX-FileCopyrightText: 2010-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "soc/soc_caps.h"
#include "esp_rom_caps.h"
#include "esp_rom_serial_output.h"
#include "rom/ets_sys.h"
#include "esp_rom_sys.h"
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

#if SOC_RECOVERY_BOOTLOADER_SUPPORTED
uint32_t esp_rom_get_bootloader_offset(void)
{
    static uint32_t offset_of_active_bootloader = UINT32_MAX;
    if (offset_of_active_bootloader == UINT32_MAX) {
        offset_of_active_bootloader = ets_get_bootloader_offset();
    }
    return offset_of_active_bootloader;
}
#endif // SOC_RECOVERY_BOOTLOADER_SUPPORTED

#if ESP_ROM_DELAY_US_PATCH && CONFIG_SECURE_ENABLE_TEE && !NON_OS_BUILD
#if (CONFIG_IDF_TARGET_ESP32C5 && CONFIG_ESP32C5_REV_MIN_FULL <= 100) || \
    (CONFIG_IDF_TARGET_ESP32C61 && CONFIG_ESP32C61_REV_MIN_FULL <= 100)

#include "riscv/rv_utils.h"

extern const ets_ops *ets_ops_table_ptr;

struct ets_ops ets_ops_patch_table_ptr;

/*
 * NOTE: Workaround for ROM delay API in ESP32-C5 (<=ECO2) and ESP32-C61 (<=ECO3):
 *
 * The ROM implementation of `ets_delay_us` uses the `mcycle` CSR to get CPU cycle count.
 * This CSR is accessible only in M-mode and when the ROM API is called from U-mode,
 * accessing `mcycle` causes an illegal instruction fault.
 *
 * This issue has been fixed in later ECO revisions of both SoCs.
 */
void ets_delay_us(uint32_t us)
{
    uint32_t start = rv_utils_get_cycle_count();
    uint32_t end = us * esp_rom_get_cpu_ticks_per_us();

    while ((rv_utils_get_cycle_count() - start) < end) {
        /* busy-wait loop for delay */
    }
}

void __attribute__((constructor)) ets_ops_set_rom_patches(void)
{
    /* Copy ROM default function table into our patch table */
    memcpy(&ets_ops_patch_table_ptr, ets_ops_table_ptr, sizeof(struct ets_ops));

    /* Replace the ROM's delay function with the patched version */
    ets_ops_patch_table_ptr.ets_delay_us = ets_delay_us;

    /* Redirect ROM calls to use the patched function table */
    ets_ops_table_ptr = &ets_ops_patch_table_ptr;
}

#endif // CONFIG_ESP32C5_REV_MIN_FULL <= 100 || CONFIG_ESP32C61_REV_MIN_FULL <= 100
#endif // ESP_ROM_DELAY_US_PATCH && CONFIG_SECURE_ENABLE_TEE && !NON_OS_BUILD
