// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
