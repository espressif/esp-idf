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
#include "rom/ets_sys.h"
#include "rom/uart.h"
#include "sdkconfig.h"

typedef enum{
    XTAL_40M = 40,
    XTAL_26M = 26,
    XTAL_24M = 24,
    XTAL_AUTO = 0
} xtal_freq_t;

typedef enum{
    CPU_80M = 1,
    CPU_160M = 2,
    CPU_240M = 3,
} cpu_freq_t;

extern void phy_get_romfunc_addr();

// TODO: these functions need to be moved from librtc to ESP-IDF
extern void rtc_init_lite();
extern void rtc_set_cpu_freq(xtal_freq_t xtal_freq, cpu_freq_t cpu_freq);

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
    phy_get_romfunc_addr();

    // freq will be changed to 40MHz in rtc_init_lite,
    // wait uart tx finish, otherwise some uart output will be lost
    uart_tx_wait_idle(0);

    rtc_init_lite();
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
    uart_tx_wait_idle(0);

    rtc_set_cpu_freq(XTAL_AUTO, freq);
    ets_update_cpu_frequency(freq_mhz);
}

