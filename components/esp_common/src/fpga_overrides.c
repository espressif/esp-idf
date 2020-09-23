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
#include "sdkconfig.h"
#include "soc/soc.h"
#include "soc/system_reg.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_rom_sys.h"
#include "esp_rom_uart.h"

extern void ets_update_cpu_frequency(uint32_t ticks_per_us);

void bootloader_clock_configure(void)
{
    esp_rom_uart_tx_wait_idle(0);

    uint32_t clock = 40000000;
    ets_update_cpu_frequency(clock / 1000000);
    REG_WRITE(RTC_CNTL_STORE5_REG, (clock >> 12) | ((clock >> 12) << 16));
}

void bootloader_fill_random(void *buffer, size_t length)
{
    uint8_t *buffer_bytes = (uint8_t *)buffer;
    for (int i = 0; i < length; i++) {
        buffer_bytes[i] = 0x5A;
    }
}

void esp_clk_init(void)
{

}

void esp_perip_clk_init(void)
{

}

/**
 * @brief No-op function, used to force linking this file
 *
 */
void esp_common_include_fpga_overrides(void)
{
}