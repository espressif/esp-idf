/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_attr.h"
#include "hal/assert.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set wakeup timer value
 *
 * @param timer_id Timer ID (ignored for V1 hardware, only one timer supported)
 * @param ticks Timer value in RTC slow clock ticks
 */
FORCE_INLINE_ATTR void rtc_timer_ll_set_wakeup_time(uint8_t timer_id, uint64_t t)
{
    (void)timer_id; // V1 hardware only supports one timer
    WRITE_PERI_REG(RTC_CNTL_SLP_TIMER0_REG, t & UINT32_MAX);
    WRITE_PERI_REG(RTC_CNTL_SLP_TIMER1_REG, t >> 32);

    SET_PERI_REG_MASK(RTC_CNTL_INT_CLR_REG, RTC_CNTL_MAIN_TIMER_INT_CLR_M);
    SET_PERI_REG_MASK(RTC_CNTL_SLP_TIMER1_REG, RTC_CNTL_MAIN_TIMER_ALARM_EN_M);
}

/**
 * @brief Get current RTC timer cycle count
 *
 * @param timer_id Timer ID (ignored for V1 hardware, only one timer supported)
 * @return Current RTC time in RTC slow clock ticks
 */
FORCE_INLINE_ATTR uint64_t rtc_timer_ll_get_cycle_count(uint8_t timer_id)
{
    (void)timer_id; // V1 hardware only supports one timer
    SET_PERI_REG_MASK(RTC_CNTL_TIME_UPDATE_REG, RTC_CNTL_TIME_UPDATE);
    uint64_t t = READ_PERI_REG(RTC_CNTL_TIME0_REG);
    t |= ((uint64_t) READ_PERI_REG(RTC_CNTL_TIME1_REG)) << 32;
    return t;
}

#ifdef __cplusplus
}
#endif
