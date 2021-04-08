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
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <sys/lock.h>

#include "esp_attr.h"
#include "esp_system.h"

#include "soc/rtc.h"
#include "esp_rom_sys.h"

#include "esp_private/system_internal.h"

#include "esp_time_impl.h"

#include "sdkconfig.h"

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
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/rtc.h"
#include "esp32c3/clk.h"
#include "esp32c3/rtc.h"
#endif



// Offset between FRC timer and the RTC.
// Initialized after reset or light sleep.
#if defined(CONFIG_ESP_TIME_FUNCS_USE_RTC_TIMER) && defined(CONFIG_ESP_TIME_FUNCS_USE_ESP_TIMER)
uint64_t s_microseconds_offset;
#endif

#ifndef CONFIG_ESP_TIME_FUNCS_USE_RTC_TIMER
static uint64_t s_boot_time; // when RTC is used to persist time, two RTC_STORE registers are used to store boot time instead
#endif

static _lock_t s_boot_time_lock;

static _lock_t s_esp_rtc_time_lock;
static RTC_DATA_ATTR uint64_t s_esp_rtc_time_us = 0, s_rtc_last_ticks = 0;

#if defined( CONFIG_ESP_TIME_FUNCS_USE_ESP_TIMER ) || defined( CONFIG_ESP_TIME_FUNCS_USE_RTC_TIMER )
uint64_t esp_time_impl_get_time_since_boot(void)
{
    uint64_t microseconds = 0;

#ifdef CONFIG_ESP_TIME_FUNCS_USE_ESP_TIMER
#ifdef CONFIG_ESP_TIME_FUNCS_USE_RTC_TIMER
    microseconds = s_microseconds_offset + esp_system_get_time();
#else
    microseconds = esp_system_get_time();
#endif // CONFIG_ESP_TIME_FUNCS_USE_RTC_TIMER
#elif defined(CONFIG_ESP_TIME_FUNCS_USE_RTC_TIMER)
    microseconds = esp_rtc_get_time_us();
#endif // CONFIG_ESP_TIME_FUNCS_USE_ESP_TIMER
    return microseconds;
}

uint64_t esp_time_impl_get_time(void)
{
    uint64_t microseconds = 0;
#if defined( CONFIG_ESP_TIME_FUNCS_USE_ESP_TIMER )
    microseconds = esp_system_get_time();
#elif defined( CONFIG_ESP_TIME_FUNCS_USE_RTC_TIMER )
    microseconds = esp_rtc_get_time_us();
#endif // CONFIG_ESP_TIME_FUNCS_USE_ESP_TIMER
    return microseconds;
}

#endif // defined( CONFIG_ESP_TIME_FUNCS_USE_ESP_TIMER ) || defined( CONFIG_ESP_TIME_FUNCS_USE_RTC_TIMER )


void esp_time_impl_set_boot_time(uint64_t time_us)
{
    _lock_acquire(&s_boot_time_lock);
#ifdef CONFIG_ESP_TIME_FUNCS_USE_RTC_TIMER
    REG_WRITE(RTC_BOOT_TIME_LOW_REG, (uint32_t) (time_us & 0xffffffff));
    REG_WRITE(RTC_BOOT_TIME_HIGH_REG, (uint32_t) (time_us >> 32));
#else
    s_boot_time = time_us;
#endif
    _lock_release(&s_boot_time_lock);
}

uint64_t esp_clk_rtc_time(void)
{
#ifdef CONFIG_ESP_TIME_FUNCS_USE_RTC_TIMER
    return esp_rtc_get_time_us();
#else
    return 0;
#endif
}

uint64_t esp_time_impl_get_boot_time(void)
{
    uint64_t result;
    _lock_acquire(&s_boot_time_lock);
#ifdef CONFIG_ESP_TIME_FUNCS_USE_RTC_TIMER
    result = ((uint64_t) REG_READ(RTC_BOOT_TIME_LOW_REG)) + (((uint64_t) REG_READ(RTC_BOOT_TIME_HIGH_REG)) << 32);
#else
    result = s_boot_time;
#endif
    _lock_release(&s_boot_time_lock);
    return result;
}

uint32_t esp_clk_slowclk_cal_get(void)
{
    return REG_READ(RTC_SLOW_CLK_CAL_REG);
}

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

void esp_set_time_from_rtc(void)
{
#if defined( CONFIG_ESP_TIME_FUNCS_USE_ESP_TIMER ) && defined( CONFIG_ESP_TIME_FUNCS_USE_RTC_TIMER )
    // initialize time from RTC clock
    s_microseconds_offset = esp_rtc_get_time_us() - esp_system_get_time();
#endif // CONFIG_ESP_TIME_FUNCS_USE_ESP_TIMER && CONFIG_ESP_TIME_FUNCS_USE_RTC_TIMER
}

void esp_sync_counters_rtc_and_frc(void)
{
#if defined( CONFIG_ESP_TIME_FUNCS_USE_ESP_TIMER ) && defined( CONFIG_ESP_TIME_FUNCS_USE_RTC_TIMER )
    struct timeval tv;
    gettimeofday(&tv, NULL);
    settimeofday(&tv, NULL);
    int64_t s_microseconds_offset_cur = esp_rtc_get_time_us() - esp_system_get_time();
    esp_time_impl_set_boot_time(esp_time_impl_get_boot_time() + ((int64_t)s_microseconds_offset - s_microseconds_offset_cur));
#endif
}

void esp_time_impl_init(void)
{
    esp_set_time_from_rtc();
}
