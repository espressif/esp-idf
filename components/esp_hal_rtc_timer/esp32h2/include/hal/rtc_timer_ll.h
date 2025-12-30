/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for ESP32-H2 RTC_Timer register operations

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "soc/soc.h"
#include "soc/lp_timer_struct.h"
#include "soc/lp_aon_reg.h"
#include "hal/misc.h"
#include "esp_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

FORCE_INLINE_ATTR void rtc_timer_ll_set_alarm_target(lp_timer_dev_t *dev, uint8_t timer_id, uint64_t value)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->target[timer_id].hi, target_hi, (value >> 32) & 0xFFFF);
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->target[timer_id].lo, target_lo, value & 0xFFFFFFFF);
}

FORCE_INLINE_ATTR void rtc_timer_ll_set_target_enable(lp_timer_dev_t *dev, uint8_t timer_id, bool en)
{
    dev->target[timer_id].hi.enable = en;
}

FORCE_INLINE_ATTR uint32_t rtc_timer_ll_get_counter_value_low(lp_timer_dev_t *dev, uint8_t timer_id)
{
    return HAL_FORCE_READ_U32_REG_FIELD(dev->counter[timer_id].lo, counter_lo);
}

FORCE_INLINE_ATTR uint32_t rtc_timer_ll_get_counter_value_high(lp_timer_dev_t *dev, uint8_t timer_id)
{
    return HAL_FORCE_READ_U32_REG_FIELD(dev->counter[timer_id].hi, counter_hi);
}

FORCE_INLINE_ATTR void rtc_timer_ll_counter_snapshot(lp_timer_dev_t *dev)
{
    dev->update.update = 1;
}

/**
 * @brief Clear alarm interrupt status
 *
 * @param dev LP timer peripheral instance
 * @param timer_id Timer ID (ignored for ESP32-H2, always clears main timer interrupt)
 */
FORCE_INLINE_ATTR void rtc_timer_ll_clear_alarm_intr_status(lp_timer_dev_t *dev, uint8_t timer_id)
{
    (void)timer_id; // ESP32-H2 only supports main timer
    dev->int_clr.alarm = 1;
}

/**
 * @brief Get interrupt raw status
 *
 * @param dev LP timer peripheral instance
 * @param timer_id Timer ID (ignored for ESP32-H2, always returns main timer interrupt status)
 * @return Interrupt raw status value
 */
FORCE_INLINE_ATTR uint32_t rtc_timer_ll_get_intr_raw(lp_timer_dev_t *dev, uint8_t timer_id)
{
    (void)timer_id; // ESP32-H2 only supports main timer
    return dev->int_raw.val;
}

/**
 * @brief Enable/disable alarm interrupt
 *
 * @param dev LP timer peripheral instance
 * @param timer_id Timer ID (ignored for ESP32-H2, always controls main timer interrupt)
 * @param enable Enable or disable interrupt
 */
FORCE_INLINE_ATTR void rtc_timer_ll_alarm_intr_enable(lp_timer_dev_t *dev, uint8_t timer_id, bool enable)
{
    (void)timer_id; // ESP32-H2 only supports main timer
    dev->int_en.alarm = enable;
}

/**
 * @brief Set wakeup timer value
 *
 * @param timer_id Timer ID
 * @param ticks Timer value in RTC slow clock ticks
 */
FORCE_INLINE_ATTR void rtc_timer_ll_set_wakeup_time(uint8_t timer_id, uint64_t ticks)
{
    rtc_timer_ll_clear_alarm_intr_status(&LP_TIMER, timer_id);
    rtc_timer_ll_set_alarm_target(&LP_TIMER, timer_id, ticks);
    rtc_timer_ll_set_target_enable(&LP_TIMER, timer_id, true);
}

/**
 * @brief Get current RTC timer cycle count
 * Note: For ESP32-H2, counter[timer_id] is used after snapshot
 *
 * @param timer_id Timer ID
 * @return Current RTC time in RTC slow clock ticks
 */
FORCE_INLINE_ATTR uint64_t rtc_timer_ll_get_cycle_count(uint8_t timer_id)
{
    rtc_timer_ll_counter_snapshot(&LP_TIMER);
    uint32_t lo = rtc_timer_ll_get_counter_value_low(&LP_TIMER, timer_id);
    uint32_t hi = rtc_timer_ll_get_counter_value_high(&LP_TIMER, timer_id);
    return ((uint64_t)hi << 32) | lo;
}

#ifdef __cplusplus
}
#endif
