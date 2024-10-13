/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "ulp_lp_core_lp_timer_shared.h"
#include "soc/soc_caps.h"

#if SOC_LP_TIMER_SUPPORTED
#include "hal/lp_timer_ll.h"
#include "hal/clk_tree_ll.h"
#include "soc/rtc.h"

#define TIMER_ID 1

static struct {
    lp_timer_dev_t *dev;
} lp_timer_context = { .dev = &LP_TIMER };

static void lp_timer_hal_set_alarm_target(uint64_t value)
{
    lp_timer_ll_clear_lp_alarm_intr_status(lp_timer_context.dev);
    lp_timer_ll_set_alarm_target(lp_timer_context.dev, TIMER_ID, value);
    lp_timer_ll_set_target_enable(lp_timer_context.dev, TIMER_ID, true);
}

uint64_t ulp_lp_core_lp_timer_get_cycle_count(void)
{
    lp_timer_ll_counter_snapshot(lp_timer_context.dev);

    uint32_t lo = lp_timer_ll_get_counter_value_low(lp_timer_context.dev, 0);
    uint32_t hi = lp_timer_ll_get_counter_value_high(lp_timer_context.dev, 0);

    lp_timer_counter_value_t result = {
        .lo = lo,
        .hi = hi
    };

    return result.val;
}

void ulp_lp_core_lp_timer_set_wakeup_time(uint64_t sleep_duration_us)
{
    uint64_t cycle_cnt = ulp_lp_core_lp_timer_get_cycle_count();
    uint64_t alarm_target = cycle_cnt + ulp_lp_core_lp_timer_calculate_sleep_ticks(sleep_duration_us);

    lp_timer_hal_set_alarm_target(alarm_target);
}

void ulp_lp_core_lp_timer_set_wakeup_ticks(uint64_t sleep_duration_ticks)
{
    uint64_t cycle_cnt = ulp_lp_core_lp_timer_get_cycle_count();
    uint64_t alarm_target = cycle_cnt + sleep_duration_ticks;

    lp_timer_hal_set_alarm_target(alarm_target);
}

void ulp_lp_core_lp_timer_disable(void)
{
    lp_timer_ll_set_target_enable(lp_timer_context.dev, TIMER_ID, false);
    lp_timer_ll_clear_lp_alarm_intr_status(lp_timer_context.dev);
}

uint64_t ulp_lp_core_lp_timer_calculate_sleep_ticks(uint64_t sleep_duration_us)
{
    return (sleep_duration_us * (1 << RTC_CLK_CAL_FRACT) / clk_ll_rtc_slow_load_cal());
}

#endif //SOC_LP_TIMER_SUPPORTED
