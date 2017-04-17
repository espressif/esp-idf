// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
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
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "rom/ets_sys.h"
#include "rom/uart.h"
#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"

/* Number of cycles to wait from the 32k XTAL oscillator to consider it running.
 * Larger values increase startup delay. Smaller values may cause false positive
 * detection (i.e. oscillator runs for a few cycles and then stops).
 */
#define XTAL_32K_DETECT_CYCLES  32
#define SLOW_CLK_CAL_CYCLES     CONFIG_ESP32_RTC_CLK_CAL_CYCLES

static void select_rtc_slow_clk(rtc_slow_freq_t slow_clk);

static const char* TAG = "clk";
/*
 * This function is not exposed as an API at this point,
 * because FreeRTOS doesn't yet support dynamic changing of
 * CPU frequency. Also we need to implement hooks for
 * components which want to be notified of CPU frequency
 * changes.
 */
void esp_clk_init(void)
{
    rtc_config_t cfg = RTC_CONFIG_DEFAULT();
    rtc_init(cfg);
    rtc_clk_fast_freq_set(RTC_FAST_FREQ_8M);

#ifdef CONFIG_ESP32_RTC_CLOCK_SOURCE_EXTERNAL_CRYSTAL
    select_rtc_slow_clk(RTC_SLOW_FREQ_32K_XTAL);
#else
    select_rtc_slow_clk(RTC_SLOW_FREQ_RTC);
#endif

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
    rtc_clk_cpu_freq_set(freq);
}

void IRAM_ATTR ets_update_cpu_frequency(uint32_t ticks_per_us)
{
    extern uint32_t g_ticks_per_us_pro;  // g_ticks_us defined in ROM for PRO CPU
    extern uint32_t g_ticks_per_us_app;  // same defined for APP CPU
    g_ticks_per_us_pro = ticks_per_us;
    g_ticks_per_us_app = ticks_per_us;
}

/* This is a cached value of RTC slow clock period; it is updated by
 * the select_rtc_slow_clk function at start up. This cached value is used in
 * other places, like time syscalls and deep sleep.
 */
static uint32_t s_rtc_slow_clk_cal = 0;

static void select_rtc_slow_clk(rtc_slow_freq_t slow_clk)
{
    if (slow_clk == RTC_SLOW_FREQ_32K_XTAL) {
        /* 32k XTAL oscillator needs to be enabled and running before it can
         * be used. Hardware doesn't have a direct way of checking if the
         * oscillator is running. Here we use rtc_clk_cal function to count
         * the number of main XTAL cycles in the given number of 32k XTAL
         * oscillator cycles. If the 32k XTAL has not started up, calibration
         * will time out, returning 0.
         */
        rtc_clk_32k_enable(true);
        uint32_t cal_val = 0;
        uint32_t wait = 0;
        // increment of 'wait' counter equivalent to 3 seconds
        const uint32_t warning_timeout = 3 /* sec */ * 32768 /* Hz */ / (2 * XTAL_32K_DETECT_CYCLES);
        ESP_EARLY_LOGD(TAG, "waiting for 32k oscillator to start up")
        do {
            ++wait;
            cal_val = rtc_clk_cal(RTC_CAL_32K_XTAL, XTAL_32K_DETECT_CYCLES);
            if (wait % warning_timeout == 0) {
                ESP_EARLY_LOGW(TAG, "still waiting for 32k oscillator to start up");
            }
        } while (cal_val == 0);
        ESP_EARLY_LOGD(TAG, "32k oscillator ready, wait=%d", wait);
    }
    rtc_clk_slow_freq_set(slow_clk);
    if (SLOW_CLK_CAL_CYCLES > 0) {
        /* TODO: 32k XTAL oscillator has some frequency drift at startup.
         * Improve calibration routine to wait until the frequency is stable.
         */
        s_rtc_slow_clk_cal = rtc_clk_cal(RTC_CAL_RTC_MUX, SLOW_CLK_CAL_CYCLES);
    } else {
        const uint64_t cal_dividend = (1ULL << RTC_CLK_CAL_FRACT) * 1000000ULL;
        s_rtc_slow_clk_cal = (uint32_t) (cal_dividend / rtc_clk_slow_freq_get_hz());
    }
    ESP_EARLY_LOGD(TAG, "RTC_SLOW_CLK calibration value: %d", s_rtc_slow_clk_cal);
}

uint32_t esp_clk_slowclk_cal_get()
{
    return s_rtc_slow_clk_cal;
}
