// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
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

#include "sys/param.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "esp_system.h"
#include "esp_task.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "esp32/clk.h"
#include "esp_private/esp_timer_impl.h"
#include "soc/frc_timer_reg.h"
#include "soc/rtc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

/**
 * @file esp_timer_esp32.c
 * @brief Implementation of chip-specific part of esp_timer
 *
 * This implementation uses FRC2 (legacy) timer of the ESP32. This timer is
 * a 32-bit up-counting timer, with a programmable compare value (called 'alarm'
 * hereafter). When the timer reaches compare value, interrupt is raised.
 * The timer can be configured to produce an edge or a level interrupt.
 *
 * In this implementation the timer is used for two purposes:
 * 1. To generate interrupts at certain moments — the upper layer of esp_timer
 *    uses this to trigger callbacks of esp_timer objects.
 *
 * 2. To keep track of time relative to application start. This facility is
 *    used both by the upper layer of esp_timer and by time functions, such as
 *    gettimeofday.
 *
 * Whenever an esp_timer timer is armed (configured to fire once or
 * periodically), timer_insert function of the upper layer calls
 * esp_timer_impl_set_alarm to enable the interrupt at the required moment.
 * This implementation sets up the timer interrupt to fire at the earliest of
 * two moments:
 * a) the time requested by upper layer
 * b) the time when the timer count reaches 0xffffffff (i.e. is about to overflow)
 *
 * Whenever the interrupt fires and timer overflow is detected, interrupt hander
 * increments s_time_base_us variable, which is used for timekeeping.
 *
 * When the interrupt fires, the upper layer is notified, and it dispatches
 * the callbacks (if any timers have expired) and sets new alarm value (if any
 * timers are still active).
 *
 * At any point in time, esp_timer_impl_get_time will return the current timer
 * value (expressed in microseconds) plus s_time_base_us. To account for the
 * case when the timer counter has overflown, but the interrupt has not fired
 * yet (for example, because interupts are temporarily disabled),
 * esp_timer_impl_get_time will also check timer overflow flag, and will add
 * s_timer_us_per_overflow to the returned value.
 *
 */

/* Timer is clocked from APB. To allow for integer scaling factor between ticks
 * and microseconds, divider 1 is used. 16 or 256 would not work for APB
 * frequencies such as 40 or 26 or 2 MHz.
 */
#define TIMER_DIV           1
#define TIMER_DIV_CFG       FRC_TIMER_PRESCALER_1

/* ALARM_OVERFLOW_VAL is used as timer alarm value when there are not timers
 * enabled which need to fire within the next timer overflow period. This alarm
 * is used to perform timekeeping (i.e. to track timer overflows).
 * Due to the 0xffffffff cannot recognize the real overflow or the scenario that
 * ISR happens follow set_alarm, so change the ALARM_OVERFLOW_VAL to resolve this problem.
 * Set it to 0xefffffffUL. The remain 0x10000000UL(about 3 second) is enough to handle ISR.
 */
#define DEFAULT_ALARM_OVERFLOW_VAL 0xefffffffUL

/* Provision to set lower overflow value for unit testing. Lowering the
 * overflow value helps check for race conditions which occur near overflow
 * moment.
 */
#ifndef ESP_TIMER_DYNAMIC_OVERFLOW_VAL
#define ALARM_OVERFLOW_VAL  DEFAULT_ALARM_OVERFLOW_VAL
#else
static uint32_t s_alarm_overflow_val = DEFAULT_ALARM_OVERFLOW_VAL;
#define ALARM_OVERFLOW_VAL (s_alarm_overflow_val)
#endif

static const char* TAG = "esp_timer_impl";

// Interrupt handle returned by the interrupt allocator
static intr_handle_t s_timer_interrupt_handle;

// Function from the upper layer to be called when the interrupt happens.
// Registered in esp_timer_impl_init.
static intr_handler_t s_alarm_handler;

// Time in microseconds from startup to the moment
// when timer counter was last equal to 0. This variable is updated each time
// when timer overflows, and when APB frequency switch is performed.
static uint64_t s_time_base_us;

// Number of timer ticks per microsecond. Calculated from APB frequency.
static uint32_t s_timer_ticks_per_us;

// Period between timer overflows, in microseconds.
// Equal to 2^32 / s_timer_ticks_per_us.
static uint32_t s_timer_us_per_overflow;

// When frequency switch happens, timer counter is reset to 0, s_time_base_us
// is updated, and alarm value is re-calculated based on the new APB frequency.
// However because the frequency switch can happen before the final
// interrupt handler is invoked, interrupt handler may see a different alarm
// value than the one which caused an interrupt. This can cause interrupt handler
// to consider that the interrupt has happened due to timer overflow, incrementing
// s_time_base_us. To avoid this, frequency switch hook sets this flag if
// it needs to set timer alarm value to ALARM_OVERFLOW_VAL. Interrupt handler
// will not increment s_time_base_us if this flag is set.
static bool s_mask_overflow;

#ifdef CONFIG_PM_DFS_USE_RTC_TIMER_REF
// If DFS is enabled, upon the first frequency change this value is set to the
// difference between esp_timer value and RTC timer value. On every subsequent
// frequency change, s_time_base_us is adjusted to maintain the same difference
// between esp_timer and RTC timer. (All mentioned values are in microseconds.)
static uint64_t s_rtc_time_diff = 0;
#endif

// Spinlock used to protect access to static variables above and to the hardware
// registers.
portMUX_TYPE s_time_update_lock = portMUX_INITIALIZER_UNLOCKED;

//Use FRC_TIMER_LOAD_VALUE(1) instead of UINT32_MAX, convenience to change FRC TIMER for future
#define TIMER_IS_AFTER_OVERFLOW(a) (ALARM_OVERFLOW_VAL < (a) && (a) <= FRC_TIMER_LOAD_VALUE(1))

// Check if timer overflow has happened (but was not handled by ISR yet)
static inline bool IRAM_ATTR timer_overflow_happened()
{
    return ((REG_READ(FRC_TIMER_CTRL_REG(1)) & FRC_TIMER_INT_STATUS) != 0 &&
              ((REG_READ(FRC_TIMER_ALARM_REG(1)) == ALARM_OVERFLOW_VAL && TIMER_IS_AFTER_OVERFLOW(REG_READ(FRC_TIMER_COUNT_REG(1))) && !s_mask_overflow) ||
               (!TIMER_IS_AFTER_OVERFLOW(REG_READ(FRC_TIMER_ALARM_REG(1))) && TIMER_IS_AFTER_OVERFLOW(REG_READ(FRC_TIMER_COUNT_REG(1))))));
}

static inline void IRAM_ATTR timer_count_reload(void)
{
    //this function should be only called the real overflow happened. And the count cannot be very approach to 0xffffffff.
    assert(TIMER_IS_AFTER_OVERFLOW(REG_READ(FRC_TIMER_COUNT_REG(1))));

    /* Restart the timer count by current time count minus ALARM_OVERFLOW_VAL(0xefffffff), it may cause error, if current tick is near boundary.
     * But even if the error happen 100% per overflow(the distance of each real overflow is about 50 second),
     * the error is 0.0125us*N per 50s(the FRC time clock is 80MHz), the N is the ticks run by the line following,
     * Normally, N is less than 10, assume N is 10, so the error accumulation is only 6.48ms per month.
     * In fact, if the CPU frequency is large than 80MHz. The error accumulation will be more less than 6.48ms per month.
     * so It can be adopted.
     */
    REG_WRITE(FRC_TIMER_LOAD_REG(1), REG_READ(FRC_TIMER_COUNT_REG(1)) - ALARM_OVERFLOW_VAL);
}

void esp_timer_impl_lock()
{
    portENTER_CRITICAL(&s_time_update_lock);
}

void esp_timer_impl_unlock()
{
    portEXIT_CRITICAL(&s_time_update_lock);
}

uint64_t IRAM_ATTR esp_timer_impl_get_time()
{
    uint32_t timer_val;
    uint64_t time_base;
    uint32_t ticks_per_us;
    bool overflow;

    do {
        /* Read all values needed to calculate current time */
        timer_val = REG_READ(FRC_TIMER_COUNT_REG(1));
        time_base = s_time_base_us;
        overflow = timer_overflow_happened();
        ticks_per_us = s_timer_ticks_per_us;

        /* Read them again and compare */
        /* In this function, do not call timer_count_reload() when overflow is true.
         * Because there's remain count enough to allow FRC_TIMER_COUNT_REG grow
         */
        if (REG_READ(FRC_TIMER_COUNT_REG(1)) > timer_val &&
                time_base == *((volatile uint64_t*) &s_time_base_us) &&
                ticks_per_us == *((volatile uint32_t*) &s_timer_ticks_per_us) &&
                overflow == timer_overflow_happened()) {
            break;
        }

        /* If any value has changed (other than the counter increasing), read again */
    } while(true);

    uint64_t result = time_base
                        + timer_val / ticks_per_us;
    return result;
}

void IRAM_ATTR esp_timer_impl_set_alarm(uint64_t timestamp)
{
    portENTER_CRITICAL(&s_time_update_lock);
    // Use calculated alarm value if it is less than ALARM_OVERFLOW_VAL.
    // Note that if by the time we update ALARM_REG, COUNT_REG value is higher,
    // interrupt will not happen for another ALARM_OVERFLOW_VAL timer ticks,
    // so need to check if alarm value is too close in the future (e.g. <2 us away).
    const uint32_t offset = s_timer_ticks_per_us * 2;
    do {
        // Adjust current time if overflow has happened
        if (timer_overflow_happened()) {
            timer_count_reload();
            s_time_base_us += s_timer_us_per_overflow;
        }
        s_mask_overflow = false;
        uint64_t cur_count = REG_READ(FRC_TIMER_COUNT_REG(1));
        // Alarm time relative to the moment when counter was 0
        int64_t time_after_timebase_us = (int64_t)timestamp - s_time_base_us;
        // Calculate desired timer compare value (may exceed 2^32-1)
        int64_t compare_val = time_after_timebase_us * s_timer_ticks_per_us;

        compare_val = MAX(compare_val, cur_count + offset);
        uint32_t alarm_reg_val = ALARM_OVERFLOW_VAL;
        if (compare_val < ALARM_OVERFLOW_VAL) {
            alarm_reg_val = (uint32_t) compare_val;
        }
        REG_WRITE(FRC_TIMER_ALARM_REG(1), alarm_reg_val);
    } while (REG_READ(FRC_TIMER_ALARM_REG(1)) <= REG_READ(FRC_TIMER_COUNT_REG(1)));
    portEXIT_CRITICAL(&s_time_update_lock);
}

static void IRAM_ATTR timer_alarm_isr(void *arg)
{
    portENTER_CRITICAL_ISR(&s_time_update_lock);
    // Timekeeping: adjust s_time_base_us if counter has passed ALARM_OVERFLOW_VAL
    if (timer_overflow_happened()) {
        timer_count_reload();
        s_time_base_us += s_timer_us_per_overflow;
    }
    s_mask_overflow = false;
    // Clear interrupt status
    REG_WRITE(FRC_TIMER_INT_REG(1), FRC_TIMER_INT_CLR);
    // Set alarm to the next overflow moment. Later, upper layer function may
    // call esp_timer_impl_set_alarm to change this to an earlier value.
    REG_WRITE(FRC_TIMER_ALARM_REG(1), ALARM_OVERFLOW_VAL);
    portEXIT_CRITICAL_ISR(&s_time_update_lock);
    // Call the upper layer handler
    (*s_alarm_handler)(arg);
}

void IRAM_ATTR esp_timer_impl_update_apb_freq(uint32_t apb_ticks_per_us)
{
    portENTER_CRITICAL_ISR(&s_time_update_lock);
    /* Bail out if the timer is not initialized yet */
    if (s_timer_interrupt_handle == NULL) {
        portEXIT_CRITICAL_ISR(&s_time_update_lock);
        return;
    }

    uint32_t new_ticks_per_us = apb_ticks_per_us / TIMER_DIV;
    uint32_t alarm = REG_READ(FRC_TIMER_ALARM_REG(1));
    uint32_t count = REG_READ(FRC_TIMER_COUNT_REG(1));
    uint64_t ticks_to_alarm = alarm - count;
    uint64_t new_ticks = (ticks_to_alarm * new_ticks_per_us) / s_timer_ticks_per_us;
    uint32_t new_alarm_val;
    if (alarm > count && new_ticks <= ALARM_OVERFLOW_VAL) {
        new_alarm_val = new_ticks;
    } else {
        new_alarm_val = ALARM_OVERFLOW_VAL;
        if (alarm != ALARM_OVERFLOW_VAL) {
            s_mask_overflow = true;
        }
    }
    REG_WRITE(FRC_TIMER_ALARM_REG(1), new_alarm_val);
    REG_WRITE(FRC_TIMER_LOAD_REG(1), 0);

    s_time_base_us += count / s_timer_ticks_per_us;

#ifdef CONFIG_PM_DFS_USE_RTC_TIMER_REF
    // Due to the extra time required to read RTC time, don't attempt this
    // adjustment when switching to a higher frequency (which usually
    // happens in an interrupt).
    if (new_ticks_per_us < s_timer_ticks_per_us) {
        uint64_t rtc_time = esp_clk_rtc_time();
        uint64_t new_rtc_time_diff = s_time_base_us - rtc_time;
        if (s_rtc_time_diff != 0) {
            uint64_t correction = new_rtc_time_diff - s_rtc_time_diff;
            s_time_base_us -= correction;
        } else {
            s_rtc_time_diff = new_rtc_time_diff;
        }
    }
#endif // CONFIG_PM_DFS_USE_RTC_TIMER_REF

    s_timer_ticks_per_us = new_ticks_per_us;
    s_timer_us_per_overflow = ALARM_OVERFLOW_VAL / new_ticks_per_us;

    portEXIT_CRITICAL_ISR(&s_time_update_lock);
}

void esp_timer_impl_advance(int64_t time_us)
{
    assert(time_us > 0 && "negative adjustments not supported yet");

    portENTER_CRITICAL(&s_time_update_lock);
    uint64_t count = REG_READ(FRC_TIMER_COUNT_REG(1));
    /* Trigger an ISR to handle past alarms and set new one.
     * ISR handler will run once we exit the critical section.
     */
    REG_WRITE(FRC_TIMER_ALARM_REG(1), 0);
    REG_WRITE(FRC_TIMER_LOAD_REG(1), 0);
    s_time_base_us += count / s_timer_ticks_per_us + time_us;
    portEXIT_CRITICAL(&s_time_update_lock);
}

esp_err_t esp_timer_impl_init(intr_handler_t alarm_handler)
{
    s_alarm_handler = alarm_handler;

    esp_err_t err = esp_intr_alloc(ETS_TIMER2_INTR_SOURCE,
            ESP_INTR_FLAG_INTRDISABLED | ESP_INTR_FLAG_IRAM,
            &timer_alarm_isr, NULL, &s_timer_interrupt_handle);

    if (err != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "esp_intr_alloc failed (0x%0x)", err);
        return err;
    }

    uint32_t apb_freq = rtc_clk_apb_freq_get();
    s_timer_ticks_per_us = apb_freq / 1000000 / TIMER_DIV;
    assert(s_timer_ticks_per_us > 0
            && apb_freq % TIMER_DIV == 0
            && "APB frequency does not result in a valid ticks_per_us value");
    s_timer_us_per_overflow = ALARM_OVERFLOW_VAL / s_timer_ticks_per_us;
    s_time_base_us = 0;

    REG_WRITE(FRC_TIMER_ALARM_REG(1), ALARM_OVERFLOW_VAL);
    REG_WRITE(FRC_TIMER_LOAD_REG(1), 0);
    REG_WRITE(FRC_TIMER_CTRL_REG(1),
            TIMER_DIV_CFG | FRC_TIMER_ENABLE | FRC_TIMER_LEVEL_INT);
    REG_WRITE(FRC_TIMER_INT_REG(1), FRC_TIMER_INT_CLR);
    ESP_ERROR_CHECK( esp_intr_enable(s_timer_interrupt_handle) );

    return ESP_OK;
}

void esp_timer_impl_deinit()
{
    esp_intr_disable(s_timer_interrupt_handle);

    REG_WRITE(FRC_TIMER_CTRL_REG(1), 0);
    REG_WRITE(FRC_TIMER_ALARM_REG(1), 0);
    REG_WRITE(FRC_TIMER_LOAD_REG(1), 0);

    esp_intr_free(s_timer_interrupt_handle);
    s_timer_interrupt_handle = NULL;
}

// FIXME: This value is safe for 80MHz APB frequency.
// Should be modified to depend on clock frequency.

uint64_t IRAM_ATTR esp_timer_impl_get_min_period_us()
{
    return 50;
}

#ifdef ESP_TIMER_DYNAMIC_OVERFLOW_VAL
uint32_t esp_timer_impl_get_overflow_val()
{
    return s_alarm_overflow_val;
}

void esp_timer_impl_set_overflow_val(uint32_t overflow_val)
{
    s_alarm_overflow_val = overflow_val;
    /* update alarm value */
    esp_timer_impl_update_apb_freq(esp_clk_apb_freq() / 1000000);
}
#endif // ESP_TIMER_DYNAMIC_OVERFLOW_VAL
