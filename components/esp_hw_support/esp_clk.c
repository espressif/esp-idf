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
#include <sys/param.h>

#include "esp_attr.h"
#include "soc/rtc.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/rtc.h"
#include "esp32/clk.h"
#include "esp32/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/rtc.h"
#include "esp32s2/clk.h"
#include "esp32s2/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/rtc.h"
#include "esp32s3/clk.h"
#include "esp32s3/rtc.h"
#include "esp32s3/rom/ets_sys.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/rtc.h"
#include "esp32c3/clk.h"
#include "esp32c3/rtc.h"
#endif

#define MHZ (1000000)

// g_ticks_us defined in ROMs for PRO and APP CPU
extern uint32_t g_ticks_per_us_pro;
#if CONFIG_IDF_TARGET_ESP32
#ifndef CONFIG_FREERTOS_UNICORE
extern uint32_t g_ticks_per_us_app;
#endif
#endif

static _lock_t s_esp_rtc_time_lock;
static RTC_DATA_ATTR uint64_t s_esp_rtc_time_us = 0, s_rtc_last_ticks = 0;

int IRAM_ATTR esp_clk_cpu_freq(void)
{
#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3
    return ets_get_cpu_frequency() * MHZ;
#else
    return g_ticks_per_us_pro * MHZ;
#endif
}

int IRAM_ATTR esp_clk_apb_freq(void)
{
#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3
    return MIN(ets_get_cpu_frequency(), 81) * MHZ;
#else
    return MIN(g_ticks_per_us_pro, 80) * MHZ;
#endif
}

int IRAM_ATTR esp_clk_xtal_freq(void)
{
    return rtc_clk_xtal_freq_get() * MHZ;
}

#ifndef CONFIG_IDF_TARGET_ESP32C3
void IRAM_ATTR ets_update_cpu_frequency(uint32_t ticks_per_us)
{
    /* Update scale factors used by esp_rom_delay_us */
    g_ticks_per_us_pro = ticks_per_us;
#if CONFIG_IDF_TARGET_ESP32
#ifndef CONFIG_FREERTOS_UNICORE
    g_ticks_per_us_app = ticks_per_us;
#endif
#endif
}
#endif

uint64_t esp_rtc_get_time_us(void)
{
    _lock_acquire(&s_esp_rtc_time_lock);
    const uint32_t cal = esp_clk_slowclk_cal_get();
    const uint64_t rtc_this_ticks = rtc_time_get();
    const uint64_t ticks = rtc_this_ticks - s_rtc_last_ticks;
    /* RTC counter result is up to 2^48, calibration factor is up to 2^24,
     * for a 32kHz clock. We need to calculate (assuming no overflow):
     *   (ticks * cal) >> RTC_CLK_CAL_FRACT
     *
     * An overflow in the (ticks * cal) multiplication would cause time to
     * wrap around after approximately 13 days, which is probably not enough
     * for some applications.
     * Therefore multiplication is split into two terms, for the lower 32-bit
     * and the upper 16-bit parts of "ticks", i.e.:
     *   ((ticks_low + 2^32 * ticks_high) * cal) >> RTC_CLK_CAL_FRACT
     */
    const uint64_t ticks_low = ticks & UINT32_MAX;
    const uint64_t ticks_high = ticks >> 32;
    const uint64_t delta_time_us = ((ticks_low * cal) >> RTC_CLK_CAL_FRACT) +
           ((ticks_high * cal) << (32 - RTC_CLK_CAL_FRACT));
    s_esp_rtc_time_us += delta_time_us;
    s_rtc_last_ticks = rtc_this_ticks;
    _lock_release(&s_esp_rtc_time_lock);
    return s_esp_rtc_time_us;
}

void esp_clk_slowclk_cal_set(uint32_t new_cal)
{
#if defined(CONFIG_ESP_TIME_FUNCS_USE_RTC_TIMER)
    /* To force monotonic time values even when clock calibration value changes,
     * we adjust esp_rtc_time
     */
    esp_rtc_get_time_us();
#endif // CONFIG_ESP_TIME_FUNCS_USE_RTC_TIMER
    REG_WRITE(RTC_SLOW_CLK_CAL_REG, new_cal);
}

uint32_t esp_clk_slowclk_cal_get(void)
{
    return REG_READ(RTC_SLOW_CLK_CAL_REG);
}

uint64_t esp_clk_rtc_time(void)
{
#ifdef CONFIG_ESP_TIME_FUNCS_USE_RTC_TIMER
    return esp_rtc_get_time_us();
#else
    return 0;
#endif
}
