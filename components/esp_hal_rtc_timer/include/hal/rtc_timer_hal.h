/*
 * SPDX-FileCopyrightText: 2020-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#include "hal/rtc_timer_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_RTC_TIMER_SUPPORTED
/**
 * @brief Set wakeup timer value
 *
 * @param timer_id Timer ID
 * @param ticks Timer value in RTC slow clock ticks
 */
#define rtc_timer_hal_set_wakeup_time(timer_id, ticks)         rtc_timer_ll_set_wakeup_time(timer_id, ticks)

/**
 * @brief Clear/disarm the wakeup timer alarm
 *
 * Disables the comparator left armed by rtc_timer_hal_set_wakeup_time() and
 * clears any pending alarm interrupt status.
 *
 * @param timer_id Timer ID
 */
#define rtc_timer_hal_clear_wakeup_time(timer_id)              rtc_timer_ll_clear_wakeup_time(timer_id)

/**
 * @brief Get current RTC timer cycle count
 *
 * @param timer_id Timer ID
 * @return Current RTC time in RTC slow clock ticks
 */
#define rtc_timer_hal_get_cycle_count(timer_id)                 rtc_timer_ll_get_cycle_count(timer_id)
#endif

#ifdef __cplusplus
}
#endif
