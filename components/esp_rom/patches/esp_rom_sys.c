// Copyright 2010-2020 Espressif Systems (Shanghai) PTE LTD
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

#include <stdint.h>
#include <stdbool.h>
#include "esp_attr.h"

#include "sdkconfig.h"

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

#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2
IRAM_ATTR void esp_rom_install_uart_printf(void)
{
    extern void ets_install_uart_printf(void);
    extern bool g_uart_print;
    // If ROM log is disabled permanently via eFuse or temporarily via RTC storage register,
    // this ROM symbol will be set to false, and cause ``esp_rom_printf`` can't work on esp-idf side.
    g_uart_print = true;
    ets_install_uart_printf();
}
#endif
