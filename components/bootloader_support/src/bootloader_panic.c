/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_log.h"
#include "bootloader_common.h"
#include "hal/cpu_hal.h"
#include "esp_rom_sys.h"


void __assert_func(const char *file, int line, const char *func, const char *expr)
{
    esp_rom_printf("Assert failed in %s, %s:%d (%s)\r\n", func, file, line, expr);
    while (1) {
    }
}

void abort(void)
{
#if !CONFIG_ESP_SYSTEM_PANIC_SILENT_REBOOT
    esp_rom_printf("abort() was called at PC 0x%08x\r\n", (intptr_t)__builtin_return_address(0) - 3);
#endif
    if (cpu_hal_is_debugger_attached()) {
        cpu_hal_break();
    }
    while (1) {
    }
}
