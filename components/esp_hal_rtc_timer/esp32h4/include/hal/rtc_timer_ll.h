/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for ESP32-H4 RTC_Timer register operations

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "soc/soc.h"
#include "soc/lp_timer_struct.h"
#include "soc/lp_timer_reg.h"
#include "soc/lp_aon_reg.h"
#include "hal/misc.h"
#include "esp_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

FORCE_INLINE_ATTR void rtc_timer_ll_set_alarm_target(lp_timer_dev_t *dev, uint8_t timer_id, uint64_t value)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->target[timer_id].hi, main_timer_tar_high0, (value >> 32) & 0xFFFF);
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->target[timer_id].lo, main_timer_tar_low0, value & 0xFFFFFFFF);
}

FORCE_INLINE_ATTR void rtc_timer_ll_set_target_enable(lp_timer_dev_t *dev, uint8_t timer_id, bool en)
{
    dev->target[timer_id].hi.main_timer_tar_en0 = en;
}

FORCE_INLINE_ATTR uint32_t rtc_timer_ll_get_counter_value_low(lp_timer_dev_t *dev, uint8_t buffer_id)
{
    return HAL_FORCE_READ_U32_REG_FIELD(dev->counter[buffer_id].lo, main_timer_buf0_low);
}

FORCE_INLINE_ATTR uint32_t rtc_timer_ll_get_counter_value_high(lp_timer_dev_t *dev, uint8_t buffer_id)
{
    return HAL_FORCE_READ_U32_REG_FIELD(dev->counter[buffer_id].hi, main_timer_buf0_high);
}

FORCE_INLINE_ATTR void rtc_timer_ll_counter_snapshot(lp_timer_dev_t *dev)
{
    dev->update.main_timer_update = 1;
}

/**
 * @brief Clear alarm interrupt status
 *
 * @param dev LP timer peripheral instance
 * @param timer_id Timer ID (0 for main timer, 1 for LP timer)
 */
FORCE_INLINE_ATTR void rtc_timer_ll_clear_alarm_intr_status(lp_timer_dev_t *dev, uint8_t timer_id)
{
    if (timer_id == 0) {
        dev->int_clr.soc_wakeup_int_clr = 1;
    } else if (timer_id == 1) {
        dev->lp_int_clr.main_timer_lp_int_clr = 1;
    }
}

/**
 * @brief Get interrupt raw status
 *
 * @param dev LP timer peripheral instance
 * @param timer_id Timer ID (0 for main timer, 1 for LP timer)
 * @return Interrupt raw status value
 */
FORCE_INLINE_ATTR uint32_t rtc_timer_ll_get_intr_raw(lp_timer_dev_t *dev, uint8_t timer_id)
{
    if (timer_id == 0) {
        return dev->int_raw.val;
    } else if (timer_id == 1) {
        return dev->lp_int_raw.val;
    }
    return 0;
}

/**
 * @brief Enable/disable alarm interrupt
 *
 * @param dev LP timer peripheral instance
 * @param timer_id Timer ID (0 for main timer, 1 for LP timer)
 * @param enable Enable or disable interrupt
 */
FORCE_INLINE_ATTR void rtc_timer_ll_alarm_intr_enable(lp_timer_dev_t *dev, uint8_t timer_id, bool enable)
{
    if (timer_id == 0) {
        dev->int_ena.soc_wakeup_int_ena = enable;
    } else if (timer_id == 1) {
        dev->lp_int_ena.main_timer_lp_int_ena = enable;
    }
}

/* Record the start and finish times for one-time regdma work */
FORCE_INLINE_ATTR void rtc_timer_ll_record_regdma_work_time_enable(lp_timer_dev_t *dev, bool en)
{
    dev->update.main_timer_regdma_work = en;
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
