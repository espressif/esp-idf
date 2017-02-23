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
#include "rtc.h"
#include "soc/soc.h"
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

    // freq will be changed to 40MHz in rtc_init_lite,
    // wait uart tx finish, otherwise some uart output will be lost
    uart_tx_wait_idle(CONFIG_CONSOLE_UART_NUM);

    rtc_init_lite(XTAL_AUTO);
    // work around a bug that RTC fast memory may be isolated
    // from the system after rtc_init_lite
    SET_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_FASTMEM_FORCE_NOISO_M);

    cpu_freq_t freq = CPU_80M;
    switch(freq_mhz) {
        case 240:
            freq = CPU_240M;
            break;
        case 160:
            freq = CPU_160M;
            break;
        default:
            freq_mhz = 80;
            /* no break */
        case 80:
            freq = CPU_80M;
            break;
    }

    // freq will be changed to freq in rtc_set_cpu_freq,
    // wait uart tx finish, otherwise some uart output will be lost
    uart_tx_wait_idle(CONFIG_CONSOLE_UART_NUM);

    rtc_set_cpu_freq(freq);
    ets_update_cpu_frequency(freq_mhz);
}

void IRAM_ATTR ets_update_cpu_frequency(uint32_t ticks_per_us)
{
    extern uint32_t g_ticks_per_us_pro;  // g_ticks_us defined in ROM for PRO CPU
    extern uint32_t g_ticks_per_us_app;  // same defined for APP CPU
    g_ticks_per_us_pro = ticks_per_us;
    g_ticks_per_us_app = ticks_per_us;
}
