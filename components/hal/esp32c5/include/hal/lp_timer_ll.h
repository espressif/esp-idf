/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for ESP32-C5 LP_Timer register operations

#pragma once

#include <stdlib.h>
#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/lp_timer_struct.h"
#include "soc/lp_aon_reg.h"
#include "hal/assert.h"
#include "hal/lp_timer_types.h"
#include "esp_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Set lp_timer alarm target
 *
 * @param  dev lp_timer source
 * @param  timer_id lp_timer target num
 * @param  value next alarm value
 *
 * @return None
 */
FORCE_INLINE_ATTR void lp_timer_ll_set_alarm_target(lp_timer_dev_t *dev, uint8_t timer_id, uint64_t value)
{
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    dev->target[timer_id].hi.main_timer_tar_high = (value >> 32) & 0xFFFF;
    dev->target[timer_id].lo.main_timer_tar_low = value & 0xFFFFFFFF;
#else
    HAL_ASSERT(false && "lp_timer not supported yet");
#endif
}

/**
 * @brief  Enable lp_timer alarm
 *
 * @param  dev lp_timer source
 * @param  timer_id lp_timer target num
 * @param  en enable bit
 *
 * @return None
 */
FORCE_INLINE_ATTR void lp_timer_ll_set_target_enable(lp_timer_dev_t *dev, uint8_t timer_id, bool en)
{
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    dev->target[timer_id].hi.main_timer_tar_en = en;
#else
    HAL_ASSERT(false && "lp_timer not supported yet");
#endif
}

/**
 * @brief  Get lp_timer low bits value of counter
 *
 * @param  dev lp_timer source
 * @param  buffer_id lp_timer counter buffer num
 *
 * @return The lp_timer low bits value of counter
 */
FORCE_INLINE_ATTR uint32_t lp_timer_ll_get_counter_value_low(lp_timer_dev_t *dev, uint8_t buffer_id)
{
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    return dev->counter[buffer_id].lo.main_timer_buf_low;
#else
    HAL_ASSERT(false && "lp_timer not supported yet");
    return 0;
#endif
}

/**
 * @brief  Get lp_timer high bits value of counter
 *
 * @param  dev lp_timer source
 * @param  buffer_id lp_timer counter buffer num
 *
 * @return The lp_timer high bits value of counter
 */
FORCE_INLINE_ATTR uint32_t lp_timer_ll_get_counter_value_high(lp_timer_dev_t *dev, uint8_t buffer_id)
{
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    return dev->counter[buffer_id].hi.main_timer_buf_high;
#else
    HAL_ASSERT(false && "lp_timer not supported yet");
    return 0;
#endif
}

/**
 * @brief  Update lp_timer counter
 *
 * @param  dev lp_timer source
 *
 * @return None
 */
FORCE_INLINE_ATTR void lp_timer_ll_counter_snapshot(lp_timer_dev_t *dev)
{
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    dev->update.main_timer_update = 1;
#else
    HAL_ASSERT(false && "lp_timer not supported yet");
#endif
}

/**
 * @brief  Clear lp_timer alarm intr status
 *
 * @param  dev lp_timer source
 *
 * @return None
 */
FORCE_INLINE_ATTR void lp_timer_ll_clear_alarm_intr_status(lp_timer_dev_t *dev)
{
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    dev->int_clr.soc_wakeup_int_clr = 1;
#else
    HAL_ASSERT(false && "lp_timer not supported yet");
#endif
}

/**
 * @brief  Clear lp_timer overflow intr status
 *
 * @param  dev lp_timer source
 *
 * @return None
 */
FORCE_INLINE_ATTR void lp_timer_ll_clear_overflow_intr_status(lp_timer_dev_t *dev)
{
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    dev->int_clr.overflow_clr = 1;
#else
    HAL_ASSERT(false && "lp_timer not supported yet");
#endif
}

/**
 * @brief  Clear lp_timer lp_alarm intr status
 *
 * @param  dev lp_timer source
 *
 * @return None
 */
FORCE_INLINE_ATTR void lp_timer_ll_clear_lp_alarm_intr_status(lp_timer_dev_t *dev)
{
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    dev->lp_int_clr.main_timer_lp_int_clr = 1;
#else
    HAL_ASSERT(false && "lp_timer not supported yet");
#endif
}

/**
 * @brief  Convert lp_timer time to count
 *
 * @param  time_in_us time in us
 *
 * @return lp_timer count
 */
FORCE_INLINE_ATTR uint64_t lp_timer_ll_time_to_count(uint64_t time_in_us)
{
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    uint32_t slow_clk_value = REG_READ(LP_AON_STORE1_REG);
    return ((time_in_us * (1 << RTC_CLK_CAL_FRACT)) / slow_clk_value);
#else
    HAL_ASSERT(false && "lp_timer not supported yet");
    return 0;
#endif
}

#ifdef __cplusplus
}
#endif
