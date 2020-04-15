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
#include "esp_attr.h"

#include "sdkconfig.h"

#ifdef CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/rtc.h"
#endif

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

void esp_rom_disable_logging(void)
{
#if CONFIG_IDF_TARGET_ESP32 // [refactor-todo]: ESP32S2 seem to also reference disabling logging in its ROM code
    /* To disable logging in the ROM, only the least significant bit of the register is used,
     * but since this register is also used to store the frequency of the main crystal (RTC_XTAL_FREQ_REG),
     * you need to write to this register in the same format.
     * Namely, the upper 16 bits and lower should be the same.
     */
    REG_SET_BIT(RTC_CNTL_STORE4_REG, RTC_DISABLE_ROM_LOG);
#endif
}
