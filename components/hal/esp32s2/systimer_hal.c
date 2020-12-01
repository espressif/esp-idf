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

#include <sys/param.h>
#include <assert.h>
#include "soc/soc_caps.h"
#include "hal/systimer_hal.h"
#include "hal/systimer_ll.h"
#include "hal/systimer_types.h"
#include "soc/rtc.h"

#define SYSTIMER_TICKS_PER_US (80) // Number of timer ticks per microsecond

uint64_t systimer_hal_get_counter_value(systimer_counter_id_t counter_id)
{
    uint32_t lo, lo_start, hi;
    /* Set the "update" bit and wait for acknowledgment */
    systimer_ll_counter_snapshot();
    while (!systimer_ll_is_counter_value_valid());
    /* Read LO, HI, then LO again, check that LO returns the same value.
     * This accounts for the case when an interrupt may happen between reading
     * HI and LO values, and this function may get called from the ISR.
     * In this case, the repeated read will return consistent values.
     */
    lo_start = systimer_ll_get_counter_value_low();
    do {
        lo = lo_start;
        hi = systimer_ll_get_counter_value_high();
        lo_start = systimer_ll_get_counter_value_low();
    } while (lo_start != lo);

    systimer_counter_value_t result = {
        .lo = lo,
        .hi = hi
    };

    return result.val;
}

uint64_t systimer_hal_get_time(systimer_counter_id_t counter_id)
{
    return systimer_hal_get_counter_value(counter_id) / SYSTIMER_TICKS_PER_US;
}

void systimer_hal_set_alarm_target(systimer_alarm_id_t alarm_id, uint64_t timestamp)
{
    int64_t offset = SYSTIMER_TICKS_PER_US * 2;
    uint64_t now_time = systimer_hal_get_counter_value(SYSTIMER_COUNTER_0);
    systimer_counter_value_t alarm = { .val = MAX(timestamp * SYSTIMER_TICKS_PER_US, now_time + offset) };
    do {
        systimer_ll_disable_alarm(alarm_id);
        systimer_ll_set_alarm_value(alarm_id, alarm.val);
        systimer_ll_enable_alarm(alarm_id);
        now_time = systimer_hal_get_counter_value(SYSTIMER_COUNTER_0);
        int64_t delta = (int64_t)alarm.val - (int64_t)now_time;
        if (delta <= 0 && !systimer_ll_is_alarm_int_fired(alarm_id)) {
            // new alarm is less than the counter and the interrupt flag is not set
            offset += -1 * delta + SYSTIMER_TICKS_PER_US * 2;
            alarm.val = now_time + offset;
        } else {
            // finish if either (alarm > counter) or the interrupt flag is already set.
            break;
        }
    } while (1);
}

uint64_t systimer_hal_get_alarm_value(systimer_alarm_id_t alarm_id)
{
    return systimer_ll_get_alarm_value(alarm_id);
}

void systimer_hal_enable_alarm_int(systimer_alarm_id_t alarm_id)
{
    systimer_ll_enable_alarm_int(alarm_id);
}

void systimer_hal_on_apb_freq_update(uint32_t apb_ticks_per_us)
{
    /* If this function was called when switching APB clock to PLL, don't need
     * do anything: the SYSTIMER_TIMER_PLL_STEP is already correct.
     * If this was called when switching APB clock to XTAL, need to adjust
     * XTAL_STEP value accordingly.
     */
    if (apb_ticks_per_us != SYSTIMER_TICKS_PER_US) {
        assert((SYSTIMER_TICKS_PER_US % apb_ticks_per_us) == 0 && "TICK_PER_US should be divisible by APB frequency (in MHz)");
        systimer_ll_set_step_for_xtal(SYSTIMER_TICKS_PER_US / apb_ticks_per_us);
    }
}

void systimer_hal_counter_value_advance(systimer_counter_id_t counter_id, int64_t time_us)
{
    systimer_counter_value_t new_count = { .val = systimer_hal_get_counter_value(counter_id) + time_us * SYSTIMER_TICKS_PER_US };
    systimer_ll_load_counter_value(new_count.val);
    systimer_ll_apply_counter_value();
}

void systimer_hal_enable_counter(systimer_counter_id_t counter_id)
{
    (void)counter_id;
}

void systimer_hal_init(void)
{
    assert(rtc_clk_xtal_freq_get() == 40 && "update the step for xtal to support other XTAL:APB frequency ratios");
    systimer_ll_enable_clock();
    /* Configure the counter:
     * - increment by 1 when running from PLL (80 ticks per microsecond),
     * - increment by 2 when running from XTAL (40 ticks per microsecond).
     * Note that if the APB frequency is derived from XTAL with divider != 1,
     * XTAL_STEP needs to be adjusted accordingly. For example, if
     * the APB frequency is XTAL/4 = 10 MHz, then XTAL_STEP should be set to 8.
     * This is handled in systimer_hal_on_apb_freq_update function.
     */
    systimer_ll_set_step_for_pll(1);
    systimer_ll_set_step_for_xtal(2);
}

void systimer_hal_select_alarm_mode(systimer_alarm_id_t alarm_id, systimer_alarm_mode_t mode)
{
    switch (mode) {
    case SYSTIMER_ALARM_MODE_ONESHOT:
        systimer_ll_enable_alarm_oneshot(alarm_id);
        break;
    case SYSTIMER_ALARM_MODE_PERIOD:
        systimer_ll_enable_alarm_period(alarm_id);
        break;
    default:
        break;
    }
}

void systimer_hal_connect_alarm_counter(systimer_alarm_id_t alarm_id, systimer_counter_id_t counter_id)
{
    // esp32s2 only has one counter, so there's no need to connect alarm unit to counter
    (void)alarm_id;
    (void)counter_id;
}
