// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdint.h>
#include "esp_attr.h"
#include "rom/ets_sys.h"
#include "rom/uart.h"
#include "sdkconfig.h"
#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"

/*
 * This function is not exposed as an API at this point,
 * because FreeRTOS doesn't yet support dynamic changing of
 * CPU frequency. Also we need to implement hooks for
 * components which want to be notified of CPU frequency
 * changes.
 */
void esp_set_cpu_freq(void)
{
    uint32_t freq_mhz = CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ;
    rtc_cpu_freq_t freq = RTC_CPU_FREQ_80M;
    switch(freq_mhz) {
        case 240:
            freq = RTC_CPU_FREQ_240M;
            break;
        case 160:
            freq = RTC_CPU_FREQ_160M;
            break;
        default:
            freq_mhz = 80;
            /* no break */
        case 80:
            freq = RTC_CPU_FREQ_80M;
            break;
    }

    // Wait for UART TX to finish, otherwise some UART output will be lost
    // when switching APB frequency
    uart_tx_wait_idle(CONFIG_CONSOLE_UART_NUM);
    rtc_config_t cfg = RTC_CONFIG_DEFAULT();
    rtc_init(cfg);
    rtc_clk_cpu_freq_set(freq);
#if ESP32_RTC_CLOCK_SOURCE_EXTERNAL_CRYSTAL
    rtc_clk_slow_freq_set(RTC_SLOW_FREQ_32K_XTAL);
#endif
}

void IRAM_ATTR ets_update_cpu_frequency(uint32_t ticks_per_us)
{
    extern uint32_t g_ticks_per_us_pro;  // g_ticks_us defined in ROM for PRO CPU
    extern uint32_t g_ticks_per_us_app;  // same defined for APP CPU
    g_ticks_per_us_pro = ticks_per_us;
    g_ticks_per_us_app = ticks_per_us;
}
