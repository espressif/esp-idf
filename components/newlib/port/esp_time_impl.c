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

#include "esp_system.h"

#include "soc/spinlock.h"
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
#endif

#if defined( CONFIG_ESP32_TIME_SYSCALL_USE_RTC ) \
    || defined( CONFIG_ESP32_TIME_SYSCALL_USE_RTC_FRC1 ) \
    || defined( CONFIG_ESP32S2_TIME_SYSCALL_USE_RTC ) \
    || defined( CONFIG_ESP32S2_TIME_SYSCALL_USE_RTC_FRC1 )
#define WITH_RTC 1
#endif

#if defined( CONFIG_ESP32_TIME_SYSCALL_USE_FRC1 ) \
    || defined( CONFIG_ESP32_TIME_SYSCALL_USE_RTC_FRC1 ) \
    || defined( CONFIG_ESP32S2_TIME_SYSCALL_USE_FRC1 ) \
    || defined( CONFIG_ESP32S2_TIME_SYSCALL_USE_RTC_FRC1 )
#define WITH_FRC 1
#endif

// Offset between FRC timer and the RTC.
// Initialized after reset or light sleep.
#if defined(WITH_RTC) && defined(WITH_FRC)
uint64_t s_microseconds_offset;
#endif

#ifndef WITH_RTC
static uint64_t s_boot_time; // when RTC is used to persist time, two RTC_STORE registers are used to store boot time instead
#endif

static spinlock_t s_time_lock = SPINLOCK_INITIALIZER;

#if defined( WITH_FRC ) || defined( WITH_RTC )
uint64_t esp_time_impl_get_time_since_boot(void)
{
    uint64_t microseconds = 0;

#ifdef WITH_FRC
#ifdef WITH_RTC
    microseconds = s_microseconds_offset + esp_system_get_time();
#else
    microseconds = esp_system_get_time();
#endif // WITH_RTC
#elif defined(WITH_RTC)
    spinlock_acquire(&s_time_lock, SPINLOCK_WAIT_FOREVER);
    microseconds = esp_rtc_get_time_us();
    spinlock_release(&s_time_lock);
#endif // WITH_FRC
    return microseconds;
}

uint64_t esp_time_impl_get_time(void)
{
    uint64_t microseconds = 0;
#if defined( WITH_FRC )
    microseconds = esp_system_get_time();
#elif defined( WITH_RTC )
    spinlock_acquire(&s_time_lock, SPINLOCK_WAIT_FOREVER);
    microseconds = esp_rtc_get_time_us();
    spinlock_release(&s_time_lock);
#endif // WITH_FRC
    return microseconds;
}

#endif // defined( WITH_FRC ) || defined( WITH_RTC )


void esp_time_impl_set_boot_time(uint64_t time_us)
{
    spinlock_acquire(&s_time_lock, SPINLOCK_WAIT_FOREVER);
#ifdef WITH_RTC
    REG_WRITE(RTC_BOOT_TIME_LOW_REG, (uint32_t) (time_us & 0xffffffff));
    REG_WRITE(RTC_BOOT_TIME_HIGH_REG, (uint32_t) (time_us >> 32));
#else
    s_boot_time = time_us;
#endif
    spinlock_release(&s_time_lock);
}

uint64_t esp_clk_rtc_time(void)
{
#ifdef WITH_RTC
    return esp_rtc_get_time_us();
#else
    return 0;
#endif
}

uint64_t esp_time_impl_get_boot_time(void)
{
    uint64_t result;
    spinlock_acquire(&s_time_lock, SPINLOCK_WAIT_FOREVER);
#ifdef WITH_RTC
    result = ((uint64_t) REG_READ(RTC_BOOT_TIME_LOW_REG)) + (((uint64_t) REG_READ(RTC_BOOT_TIME_HIGH_REG)) << 32);
#else
    result = s_boot_time;
#endif
    spinlock_release(&s_time_lock);
    return result;
}

uint32_t esp_clk_slowclk_cal_get(void)
{
    return REG_READ(RTC_SLOW_CLK_CAL_REG);
}

uint64_t esp_rtc_get_time_us(void)
{
    const uint64_t ticks = rtc_time_get();
    const uint32_t cal = esp_clk_slowclk_cal_get();
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
    return ((ticks_low * cal) >> RTC_CLK_CAL_FRACT) +
           ((ticks_high * cal) << (32 - RTC_CLK_CAL_FRACT));
}

void esp_clk_slowclk_cal_set(uint32_t new_cal)
{
#if defined(WITH_RTC)
    /* To force monotonic time values even when clock calibration value changes,
     * we adjust boot time, given current time and the new calibration value:
     *      T = boot_time_old + cur_cal * ticks / 2^19
     *      T = boot_time_adj + new_cal * ticks / 2^19
     * which results in:
     *      boot_time_adj = boot_time_old + ticks * (cur_cal - new_cal) / 2^19
     */
    const int64_t ticks = (int64_t) rtc_time_get();
    const uint32_t cur_cal = REG_READ(RTC_SLOW_CLK_CAL_REG);
    int32_t cal_diff = (int32_t) (cur_cal - new_cal);
    int64_t boot_time_diff = ticks * cal_diff / (1LL << RTC_CLK_CAL_FRACT);
    uint64_t boot_time_adj = esp_time_impl_get_boot_time() + boot_time_diff;
    esp_time_impl_set_boot_time(boot_time_adj);
#endif // WITH_RTC
    REG_WRITE(RTC_SLOW_CLK_CAL_REG, new_cal);
}

void esp_set_time_from_rtc(void)
{
#if defined( WITH_FRC ) && defined( WITH_RTC )
    // initialize time from RTC clock
    s_microseconds_offset = esp_rtc_get_time_us() - esp_system_get_time();
#endif // WITH_FRC && WITH_RTC
}

void esp_sync_counters_rtc_and_frc(void)
{
#if defined( WITH_FRC ) && defined( WITH_RTC )
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