// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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
#include <sys/types.h>
#include <sys/reent.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/lock.h>
#include <rom/rtc.h>
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/frc_timer_reg.h"
#include "rom/ets_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/xtensa_api.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#if defined( CONFIG_ESP32_TIME_SYSCALL_USE_RTC ) || defined( CONFIG_ESP32_TIME_SYSCALL_USE_RTC_FRC1 )
#define WITH_RTC 1
#endif

#if defined( CONFIG_ESP32_TIME_SYSCALL_USE_FRC1 ) || defined( CONFIG_ESP32_TIME_SYSCALL_USE_RTC_FRC1 )
#define WITH_FRC1 1
#endif

#ifdef WITH_RTC
static uint64_t get_rtc_time_us()
{
    SET_PERI_REG_MASK(RTC_CNTL_TIME_UPDATE_REG, RTC_CNTL_TIME_UPDATE_M);
    while (GET_PERI_REG_MASK(RTC_CNTL_TIME_UPDATE_REG, RTC_CNTL_TIME_VALID_M) == 0) {
        ;
    }
    CLEAR_PERI_REG_MASK(RTC_CNTL_TIME_UPDATE_REG, RTC_CNTL_TIME_UPDATE_M);
    uint64_t low = READ_PERI_REG(RTC_CNTL_TIME0_REG);
    uint64_t high = READ_PERI_REG(RTC_CNTL_TIME1_REG);
    uint64_t ticks = (high << 32) | low;
    return ticks * 100 / (RTC_CTNL_SLOWCLK_FREQ / 10000);    // scale RTC_CTNL_SLOWCLK_FREQ to avoid overflow
}
#endif // WITH_RTC


// s_boot_time: time from Epoch to the first boot time
#ifdef WITH_RTC
// when RTC is used to persist time, two RTC_STORE registers are used to store boot time
#elif defined(WITH_FRC1)
static uint64_t s_boot_time;
#endif

#if defined(WITH_RTC) || defined(WITH_FRC1)
static _lock_t s_boot_time_lock;
#endif

#ifdef WITH_FRC1
#define FRC1_PRESCALER 16
#define FRC1_PRESCALER_CTL 2
#define FRC1_TICK_FREQ (APB_CLK_FREQ / FRC1_PRESCALER)
#define FRC1_TICKS_PER_US (FRC1_TICK_FREQ / 1000000)
#define FRC1_ISR_PERIOD_US (FRC_TIMER_LOAD_VALUE(0) / FRC1_TICKS_PER_US)
// Counter frequency will be APB_CLK_FREQ / 16 = 5 MHz
// 1 tick = 0.2 us
// Timer has 23 bit counter, so interrupt will fire each 1677721.6 microseconds.
// This is not a whole number, so timer will drift by 0.3 ppm due to rounding error.

static volatile uint64_t s_microseconds = 0;

static void IRAM_ATTR frc_timer_isr()
{
    // Write to FRC_TIMER_INT_REG may not take effect in some cases (root cause TBD)
    // This extra write works around this issue.
    // FRC_TIMER_LOAD_REG(0) is used here, but any other DPORT register address can also be used.
    WRITE_PERI_REG(FRC_TIMER_LOAD_REG(0), FRC_TIMER_LOAD_VALUE(0));
    WRITE_PERI_REG(FRC_TIMER_INT_REG(0), FRC_TIMER_INT_CLR);
    s_microseconds += FRC1_ISR_PERIOD_US;
}

#endif // WITH_FRC1

#if defined(WITH_RTC) || defined(WITH_FRC1)
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

static uint64_t get_boot_time()
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
#endif //defined(WITH_RTC) || defined(WITH_FRC1)

void esp_setup_time_syscalls()
{
#if defined( WITH_FRC1 )
#if defined( WITH_RTC )
    // initialize time from RTC clock
    s_microseconds = get_rtc_time_us();
#endif //WITH_RTC

    // set up timer
    WRITE_PERI_REG(FRC_TIMER_CTRL_REG(0), \
            FRC_TIMER_AUTOLOAD | \
            (FRC1_PRESCALER_CTL << FRC_TIMER_PRESCALER_S) | \
            FRC_TIMER_EDGE_INT);

    WRITE_PERI_REG(FRC_TIMER_LOAD_REG(0), FRC_TIMER_LOAD_VALUE(0));
    SET_PERI_REG_MASK(FRC_TIMER_CTRL_REG(0),
            FRC_TIMER_ENABLE | \
            FRC_TIMER_INT_ENABLE);
    esp_intr_alloc(ETS_TIMER1_INTR_SOURCE, 0, &frc_timer_isr, NULL, NULL);
#endif // WITH_FRC1
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

#if defined( WITH_FRC1 ) || defined( WITH_RTC )
static uint64_t get_time_since_boot()
{
    uint64_t microseconds = 0;
#ifdef WITH_FRC1
    uint32_t timer_ticks_before = READ_PERI_REG(FRC_TIMER_COUNT_REG(0));
    microseconds = s_microseconds;
    uint32_t timer_ticks_after = READ_PERI_REG(FRC_TIMER_COUNT_REG(0));
    if (timer_ticks_after > timer_ticks_before) {
        // overflow happened at some point between getting
        // timer_ticks_before and timer_ticks_after
        // microseconds value is ambiguous, get a new one
        microseconds = s_microseconds;
    }
    microseconds += (FRC_TIMER_LOAD_VALUE(0) - timer_ticks_after) / FRC1_TICKS_PER_US;
#elif defined(WITH_RTC)
    microseconds = get_rtc_time_us();
#endif
    return microseconds;
}
#endif // defined( WITH_FRC1 ) || defined( WITH_RTC )

int IRAM_ATTR _gettimeofday_r(struct _reent *r, struct timeval *tv, void *tz)
{
    (void) tz;
#if defined( WITH_FRC1 ) || defined( WITH_RTC )
    if (tv) {
        uint64_t microseconds = get_boot_time() + get_time_since_boot();
        tv->tv_sec = microseconds / 1000000;
        tv->tv_usec = microseconds % 1000000;
    }
    return 0;
#else
    __errno_r(r) = ENOSYS;
    return -1;
#endif // defined( WITH_FRC1 ) || defined( WITH_RTC )
}

int settimeofday(const struct timeval *tv, const struct timezone *tz)
{
    (void) tz;
#if defined( WITH_FRC1 ) || defined( WITH_RTC )
    if (tv) {
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

uint32_t system_get_time(void)
{
#if defined( WITH_FRC1 ) || defined( WITH_RTC )
    return get_time_since_boot();
#else
    return 0;
#endif
}

uint32_t system_get_current_time(void) __attribute__((alias("system_get_time")));

uint32_t system_relative_time(uint32_t current_time)
{
    return system_get_time() - current_time;
}

uint64_t system_get_rtc_time(void)
{
#ifdef WITH_RTC
    return get_rtc_time_us();
#else
    return 0;
#endif
}

