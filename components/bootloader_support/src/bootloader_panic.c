/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_cpu.h"
#include "esp_log.h"
#include "bootloader_common.h"
#include "esp_rom_sys.h"


void __assert_func(const char *file, int line, const char *func, const char *expr)
{

#if !CONFIG_OPTIMIZATION_ASSERTIONS_SILENT
    esp_rom_printf("Assert failed in %s, %s:%d (%s)\r\n", func, file, line, expr);
#endif

    while (1) {
    }
}

void abort(void)
{
#if !CONFIG_ESP_SYSTEM_PANIC_SILENT_REBOOT
    esp_rom_printf("abort() was called at PC 0x%08x\r\n", (intptr_t)__builtin_return_address(0) - 3);
#endif
#if CONFIG_ESP_DEBUG_OCDAWARE
    if (esp_cpu_dbgr_is_attached()) {
        esp_cpu_dbgr_break();
    }
#endif
    while (1) {
    }
}
