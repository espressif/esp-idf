/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "ulp_lp_core_lp_timer_shared.h"
#include "soc/soc_caps.h"

#if SOC_RTC_TIMER_V2_SUPPORTED
#include "hal/rtc_timer_ll.h"
#include "hal/clk_tree_ll.h"
#include "soc/rtc.h"

#define TIMER_ID 1

uint64_t ulp_lp_core_lp_timer_get_cycle_count(void)
{
    return rtc_timer_ll_get_cycle_count(0);
}

void ulp_lp_core_lp_timer_set_wakeup_time(uint64_t sleep_duration_us)
{
    uint64_t cycle_cnt = ulp_lp_core_lp_timer_get_cycle_count();
    uint64_t alarm_target = cycle_cnt + ulp_lp_core_lp_timer_calculate_sleep_ticks(sleep_duration_us);

    rtc_timer_ll_set_wakeup_time(TIMER_ID, alarm_target);
}

void ulp_lp_core_lp_timer_set_wakeup_ticks(uint64_t sleep_duration_ticks)
{
    uint64_t cycle_cnt = ulp_lp_core_lp_timer_get_cycle_count();
    uint64_t alarm_target = cycle_cnt + sleep_duration_ticks;

    rtc_timer_ll_set_wakeup_time(TIMER_ID, alarm_target);
}

void ulp_lp_core_lp_timer_disable(void)
{
    rtc_timer_ll_set_target_enable(&LP_TIMER, TIMER_ID, false);
    rtc_timer_ll_clear_alarm_intr_status(&LP_TIMER, TIMER_ID);
}

uint64_t ulp_lp_core_lp_timer_calculate_sleep_ticks(uint64_t sleep_duration_us)
{
    return (sleep_duration_us * (1 << RTC_CLK_CAL_FRACT) / clk_ll_rtc_slow_load_cal());
}

#endif // SOC_RTC_TIMER_V2_SUPPORTED
