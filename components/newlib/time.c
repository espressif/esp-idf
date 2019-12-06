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

#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <reent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/reent.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/lock.h>
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "esp_timer.h"
#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/frc_timer_reg.h"
#include "freertos/FreeRTOS.h"
#include "freertos/xtensa_api.h"
#include "freertos/task.h"
#include "limits.h"
#include "sdkconfig.h"
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/ets_sys.h"
#include "esp32/clk.h"
#include "esp32/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S2BETA
#include "esp32s2beta/clk.h"
#include "esp32s2beta/rom/rtc.h"
#include "esp32s2beta/rom/ets_sys.h"
#endif

#if defined( CONFIG_ESP32_TIME_SYSCALL_USE_RTC ) || defined( CONFIG_ESP32_TIME_SYSCALL_USE_RTC_FRC1 ) || defined( CONFIG_ESP32S2_TIME_SYSCALL_USE_RTC ) || defined( CONFIG_ESP32S2_TIME_SYSCALL_USE_RTC_FRC1 )
#define WITH_RTC 1
#endif

#if defined( CONFIG_ESP32_TIME_SYSCALL_USE_FRC1 ) || defined( CONFIG_ESP32_TIME_SYSCALL_USE_RTC_FRC1 ) || defined( CONFIG_ESP32S2_TIME_SYSCALL_USE_FRC1 ) || defined( CONFIG_ESP32S2_TIME_SYSCALL_USE_RTC_FRC1 )
#define WITH_FRC 1
#endif

#ifdef WITH_RTC
static uint64_t get_rtc_time_us(void)
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
#endif // WITH_RTC


// s_boot_time: time from Epoch to the first boot time
#ifdef WITH_RTC
// when RTC is used to persist time, two RTC_STORE registers are used to store boot time
#elif defined(WITH_FRC)
static uint64_t s_boot_time;
#endif // WITH_RTC

#if defined(WITH_RTC) || defined(WITH_FRC)
static _lock_t s_boot_time_lock;
static _lock_t s_adjust_time_lock;
// stores the start time of the slew
static uint64_t adjtime_start = 0;
// is how many microseconds total to slew
static int64_t adjtime_total_correction = 0;
#define ADJTIME_CORRECTION_FACTOR 6
static uint64_t get_time_since_boot(void);
#endif
// Offset between FRC timer and the RTC.
// Initialized after reset or light sleep.
#if defined(WITH_RTC) && defined(WITH_FRC)
uint64_t s_microseconds_offset;
#endif

#if defined(WITH_RTC) || defined(WITH_FRC)
static void set_boot_time(uint64_t time_us)
{
    _lock_acquire(&s_boot_time_lock);
#ifdef WITH_RTC
    REG_WRITE(RTC_BOOT_TIME_LOW_REG, (uint32_t) (time_us & 0xffffffff));
    REG_WRITE(RTC_BOOT_TIME_HIGH_REG, (uint32_t) (time_us >> 32));
#else
    s_boot_time = time_us;
#endif
    _lock_release(&s_boot_time_lock);
}

static uint64_t get_boot_time(void)
{
    uint64_t result;
    _lock_acquire(&s_boot_time_lock);
#ifdef WITH_RTC
    result = ((uint64_t) REG_READ(RTC_BOOT_TIME_LOW_REG)) + (((uint64_t) REG_READ(RTC_BOOT_TIME_HIGH_REG)) << 32);
#else
    result = s_boot_time;
#endif
    _lock_release(&s_boot_time_lock);
    return result;
}

// This function gradually changes boot_time to the correction value and immediately updates it.
static uint64_t adjust_boot_time(void)
{
    uint64_t boot_time = get_boot_time();
    if ((boot_time == 0) || (get_time_since_boot() < adjtime_start)) {
        adjtime_start = 0;
    }
    if (adjtime_start > 0) {
        uint64_t since_boot = get_time_since_boot();
        // If to call this function once per second, then (since_boot - adjtime_start) will be 1_000_000 (1 second),
        // and the correction will be equal to (1_000_000us >> 6) = 15_625 us.
        // The minimum possible correction step can be (64us >> 6) = 1us.
        // Example: if the time error is 1 second, then it will be compensate for 1 sec / 0,015625 = 64 seconds.
        int64_t correction = (since_boot >> ADJTIME_CORRECTION_FACTOR) - (adjtime_start >> ADJTIME_CORRECTION_FACTOR);
        if (correction > 0) {
            adjtime_start = since_boot;
            if (adjtime_total_correction < 0) {
                if ((adjtime_total_correction + correction) >= 0) {
                    boot_time = boot_time + adjtime_total_correction;
                    adjtime_start = 0;
                } else {
                    adjtime_total_correction += correction;
                    boot_time -= correction;
                }
            } else {
                if ((adjtime_total_correction - correction) <= 0) {
                    boot_time = boot_time + adjtime_total_correction;
                    adjtime_start = 0;
                } else {
                    adjtime_total_correction -= correction;
                    boot_time += correction;
                }
            }
            set_boot_time(boot_time);
        }
    }
    return boot_time;
}

// Get the adjusted boot time.
static uint64_t get_adjusted_boot_time (void)
{
    _lock_acquire(&s_adjust_time_lock);
    uint64_t adjust_time = adjust_boot_time();
    _lock_release(&s_adjust_time_lock);
    return adjust_time;
}

// Applying the accumulated correction to boot_time and stopping the smooth time adjustment.
static void adjtime_corr_stop (void)
{
    _lock_acquire(&s_adjust_time_lock);
    if (adjtime_start != 0){
        adjust_boot_time();
        adjtime_start = 0;
    }
    _lock_release(&s_adjust_time_lock);
}
#endif //defined(WITH_RTC) || defined(WITH_FRC)

int adjtime(const struct timeval *delta, struct timeval *outdelta)
{
#if defined( WITH_FRC ) || defined( WITH_RTC )
    if(delta != NULL){
        int64_t sec  = delta->tv_sec;
        int64_t usec = delta->tv_usec;
        if(llabs(sec) > ((INT_MAX / 1000000L) - 1L)) {
            return -1;
        }
        /*
        * If adjusting the system clock by adjtime () is already done during the second call adjtime (),
        * and the delta of the second call is not NULL, the earlier tuning is stopped,
        * but the already completed part of the adjustment is not canceled.
        */
        _lock_acquire(&s_adjust_time_lock);
        // If correction is already in progress (adjtime_start != 0), then apply accumulated corrections.
        adjust_boot_time();
        adjtime_start = get_time_since_boot();
        adjtime_total_correction = sec * 1000000L + usec;
        _lock_release(&s_adjust_time_lock);
    }
    if(outdelta != NULL){
        _lock_acquire(&s_adjust_time_lock);
        adjust_boot_time();
        if (adjtime_start != 0) {
            outdelta->tv_sec    = adjtime_total_correction / 1000000L;
            outdelta->tv_usec   = adjtime_total_correction % 1000000L;
        } else {
            outdelta->tv_sec    = 0;
            outdelta->tv_usec   = 0;
        }
        _lock_release(&s_adjust_time_lock);
    }
  return 0;
#else
  return -1;
#endif

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
    uint64_t boot_time_adj = get_boot_time() + boot_time_diff;
    set_boot_time(boot_time_adj);
#endif // WITH_RTC
    REG_WRITE(RTC_SLOW_CLK_CAL_REG, new_cal);
}

uint32_t esp_clk_slowclk_cal_get(void)
{
    return REG_READ(RTC_SLOW_CLK_CAL_REG);
}

void esp_set_time_from_rtc(void)
{
#if defined( WITH_FRC ) && defined( WITH_RTC )
    // initialize time from RTC clock
    s_microseconds_offset = get_rtc_time_us() - esp_timer_get_time();
#endif // WITH_FRC && WITH_RTC
}

uint64_t esp_clk_rtc_time(void)
{
#ifdef WITH_RTC
    return get_rtc_time_us();
#else
    return 0;
#endif
}

clock_t IRAM_ATTR _times_r(struct _reent *r, struct tms *ptms)
{
    clock_t t = xTaskGetTickCount() * (portTICK_PERIOD_MS * CLK_TCK / 1000);
    ptms->tms_cstime = 0;
    ptms->tms_cutime = 0;
    ptms->tms_stime = t;
    ptms->tms_utime = 0;
    struct timeval tv = {0, 0};
    _gettimeofday_r(r, &tv, NULL);
    return (clock_t) tv.tv_sec;
}

#if defined( WITH_FRC ) || defined( WITH_RTC )
static uint64_t get_time_since_boot(void)
{
    uint64_t microseconds = 0;
#ifdef WITH_FRC
#ifdef WITH_RTC
    microseconds = s_microseconds_offset + esp_timer_get_time();
#else
    microseconds = esp_timer_get_time();
#endif // WITH_RTC
#elif defined(WITH_RTC)
    microseconds = get_rtc_time_us();
#endif // WITH_FRC
    return microseconds;
}
#endif // defined( WITH_FRC ) || defined( WITH_RTC )

int IRAM_ATTR _gettimeofday_r(struct _reent *r, struct timeval *tv, void *tz)
{
    (void) tz;
#if defined( WITH_FRC ) || defined( WITH_RTC )
    if (tv) {
        uint64_t microseconds = get_adjusted_boot_time() + get_time_since_boot();
        tv->tv_sec = microseconds / 1000000;
        tv->tv_usec = microseconds % 1000000;
    }
    return 0;
#else
    __errno_r(r) = ENOSYS;
    return -1;
#endif // defined( WITH_FRC ) || defined( WITH_RTC )
}

int settimeofday(const struct timeval *tv, const struct timezone *tz)
{
    (void) tz;
#if defined( WITH_FRC ) || defined( WITH_RTC )
    if (tv) {
        adjtime_corr_stop();
        uint64_t now = ((uint64_t) tv->tv_sec) * 1000000LL + tv->tv_usec;
        uint64_t since_boot = get_time_since_boot();
        set_boot_time(now - since_boot);
    }
    return 0;
#else
    errno = ENOSYS;
    return -1;
#endif
}

int usleep(useconds_t us)
{
    const int us_per_tick = portTICK_PERIOD_MS * 1000;
    if (us < us_per_tick) {
        ets_delay_us((uint32_t) us);
    } else {
        /* since vTaskDelay(1) blocks for anywhere between 0 and portTICK_PERIOD_MS,
         * round up to compensate.
         */
        vTaskDelay((us + us_per_tick - 1) / us_per_tick);
    }
    return 0;
}

unsigned int sleep(unsigned int seconds)
{
    usleep(seconds*1000000UL);
    return 0;
}

uint32_t system_get_time(void)
{
#if defined( WITH_FRC ) || defined( WITH_RTC )
    return get_time_since_boot();
#else
    return 0;
#endif
}

uint32_t system_get_current_time(void) __attribute__((alias("system_get_time")));

uint32_t system_relative_time(uint32_t current_time)
{
#if defined( WITH_FRC ) || defined( WITH_RTC )
    return get_time_since_boot() - current_time;
#else
    return 0;
#endif
}

uint64_t system_get_rtc_time(void)
{
#ifdef WITH_RTC
    return get_rtc_time_us();
#else
    return 0;
#endif
}

void esp_sync_counters_rtc_and_frc(void)
{
#if defined( WITH_FRC ) && defined( WITH_RTC )
    adjtime_corr_stop();
    int64_t s_microseconds_offset_cur = get_rtc_time_us() - esp_timer_get_time();
    set_boot_time(get_adjusted_boot_time() + ((int64_t)s_microseconds_offset - s_microseconds_offset_cur));
#endif
}


int clock_settime (clockid_t clock_id, const struct timespec *tp)
{
#if defined( WITH_FRC ) || defined( WITH_RTC )
    if (tp == NULL) {
        errno = EINVAL;
        return -1;
    }
    struct timeval tv;
    switch (clock_id) {
        case CLOCK_REALTIME:
            tv.tv_sec = tp->tv_sec;
            tv.tv_usec = tp->tv_nsec / 1000L;
            settimeofday(&tv, NULL);
            break;
        default:
            errno = EINVAL;
            return -1;
    }
    return 0;
#else
    errno = ENOSYS;
    return -1;
#endif
}

int clock_gettime (clockid_t clock_id, struct timespec *tp)
{
#if defined( WITH_FRC ) || defined( WITH_RTC )
    if (tp == NULL) {
        errno = EINVAL;
        return -1;
    }
    struct timeval tv;
    uint64_t monotonic_time_us = 0;
    switch (clock_id) {
        case CLOCK_REALTIME:
            _gettimeofday_r(NULL, &tv, NULL);
            tp->tv_sec = tv.tv_sec;
            tp->tv_nsec = tv.tv_usec * 1000L;
            break;
        case CLOCK_MONOTONIC:
#if defined( WITH_FRC )
            monotonic_time_us = (uint64_t) esp_timer_get_time();
#elif defined( WITH_RTC )
            monotonic_time_us = get_rtc_time_us();
#endif // WITH_FRC
            tp->tv_sec = monotonic_time_us / 1000000LL;
            tp->tv_nsec = (monotonic_time_us % 1000000LL) * 1000L;
            break;
        default:
            errno = EINVAL;
            return -1;
    }
    return 0;
#else
    errno = ENOSYS;
    return -1;
#endif
}

int clock_getres (clockid_t clock_id, struct timespec *res)
{
#if defined( WITH_FRC ) || defined( WITH_RTC )
    if (res == NULL) {
        errno = EINVAL;
        return -1;
    }
#if defined( WITH_FRC )
    res->tv_sec = 0;
    res->tv_nsec = 1000L;
#elif defined( WITH_RTC )
    res->tv_sec = 0;
    uint32_t rtc_freq = rtc_clk_slow_freq_get_hz();
    assert(rtc_freq != 0);
    res->tv_nsec = 1000000000L / rtc_freq;
#endif // WITH_FRC
    return 0;
#else
    errno = ENOSYS;
    return -1;
#endif
}
