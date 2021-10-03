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
#ifndef CONFIG_IDF_TARGET_ESP32
#include "soc/system_reg.h"
#endif // not CONFIG_IDF_TARGET_ESP32
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "esp_rom_uart.h"
#include "esp_attr.h"

static const char *TAG = "fpga";

#ifdef CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/rtc.h"
#endif
#ifdef CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/rtc.h"
#endif

extern void ets_update_cpu_frequency(uint32_t ticks_per_us);

static void s_warn(void)
{
    ESP_EARLY_LOGW(TAG, "Project configuration is for internal FPGA use, not all functions will work");
}

void bootloader_clock_configure(void)
{
    s_warn();
    esp_rom_uart_tx_wait_idle(0);

    uint32_t xtal_freq_mhz = 40;
#ifdef CONFIG_IDF_TARGET_ESP32S2
    uint32_t apb_freq_hz = 20000000;
#elif CONFIG_IDF_TARGET_ESP32S2H2
    uint32_t apb_freq_hz = 32000000;
#else
    uint32_t apb_freq_hz = 40000000;
#endif // CONFIG_IDF_TARGET_ESP32S2
    ets_update_cpu_frequency(apb_freq_hz / 1000000);
#ifdef RTC_APB_FREQ_REG
    REG_WRITE(RTC_APB_FREQ_REG, (apb_freq_hz >> 12) | ((apb_freq_hz >> 12) << 16));
#endif
    REG_WRITE(RTC_CNTL_STORE4_REG, (xtal_freq_mhz) | ((xtal_freq_mhz) << 16));
}

/* Placed in IRAM since test_apps expects it to be */
void IRAM_ATTR bootloader_fill_random(void *buffer, size_t length)
{
    uint8_t *buffer_bytes = (uint8_t *)buffer;
    for (int i = 0; i < length; i++) {
        buffer_bytes[i] = 0x5A;
    }
}

void esp_clk_init(void)
{
    s_warn();
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
