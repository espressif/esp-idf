/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "ulp_lp_core_utils.h"
#include "hal/lp_core_ll.h"
#if SOC_RTC_TIMER_SUPPORTED
#include "ulp_lp_core_lp_timer_shared.h"
#endif

/* Wake HP, then stop the LP core without returning from main.
 * Returning from main rearms the LP timer and would run main again, wiping
 * the results HP still has to read. */
static inline void __attribute__((noreturn)) lp_sleep_finish(void)
{
    ulp_lp_core_wakeup_main_processor();
#if SOC_RTC_TIMER_SUPPORTED
    ulp_lp_core_lp_timer_disable();
#endif
    lp_core_ll_set_wakeup_source(0);
    ulp_lp_core_halt();
}
