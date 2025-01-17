/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The ll is not public api, don't use in application code.
 * See readme.md in hal/readme.md
 ******************************************************************************/

#pragma once

#include "soc/lp_aon_struct.h"
#include "soc/pmu_struct.h"
#include "hal/misc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RTCIO_LL_GPIO_NUM_OFFSET    7 // rtcio 0-7 correspond to gpio 7-14

typedef enum {
    RTCIO_FUNC_RTC = 0x0,         /*!< The pin controlled by RTC module. */
    RTCIO_FUNC_DIGITAL = 0x1,     /*!< The pin controlled by DIGITAL module. */
} rtcio_ll_func_t;

/**
 * @brief Select the rtcio function.
 *
 * @note The RTC function must be selected before the pad analog function is enabled.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @param func Select pin function.
 */
static inline void rtcio_ll_function_select(int rtcio_num, rtcio_ll_func_t func)
{
    if (func == RTCIO_FUNC_RTC) {
        // 0: GPIO connected to digital GPIO module. 1: GPIO connected to analog RTC module.
        uint32_t sel_mask = HAL_FORCE_READ_U32_REG_FIELD(LP_AON.gpio_mux, gpio_mux_sel);
        sel_mask |= BIT(rtcio_num);
        HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.gpio_mux, gpio_mux_sel, sel_mask);
    } else if (func == RTCIO_FUNC_DIGITAL) {
        // Clear the bit to use digital GPIO module
        uint32_t sel_mask = HAL_FORCE_READ_U32_REG_FIELD(LP_AON.gpio_mux, gpio_mux_sel);
        sel_mask &= ~BIT(rtcio_num);
        HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.gpio_mux, gpio_mux_sel, sel_mask);
    }
}

/**
 * Enable force hold function for an RTC IO pad.
 *
 * Enabling HOLD function will cause the pad to lock current status, such as,
 * input/output enable, input/output value, function, drive strength values.
 * This function is useful when going into light or deep sleep mode to prevent
 * the pin configuration from changing.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_force_hold_enable(int rtcio_num)
{
    LP_AON.gpio_hold0.gpio_hold0 |= BIT(rtcio_num + RTCIO_LL_GPIO_NUM_OFFSET);
}

/**
 * Disable hold function on an RTC IO pad
 *
 * @note If disable the pad hold, the status of pad maybe changed in sleep mode.
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_force_hold_disable(int rtcio_num)
{
    LP_AON.gpio_hold0.gpio_hold0 &= ~BIT(rtcio_num + RTCIO_LL_GPIO_NUM_OFFSET);
}

/**
 * Enable force hold function for all RTC IO pads
 *
 * Enabling HOLD function will cause the pad to lock current status, such as,
 * input/output enable, input/output value, function, drive strength values.
 * This function is useful when going into light or deep sleep mode to prevent
 * the pin configuration from changing.
 */
static inline void rtcio_ll_force_hold_all(void)
{
    PMU.imm.pad_hold_all.tie_high_lp_pad_hold_all = 1;
}

/**
 * Disable hold function fon all RTC IO pads
 *
 * @note If disable the pad hold, the status of pad maybe changed in sleep mode.
 */
static inline void rtcio_ll_force_unhold_all(void)
{
    PMU.imm.pad_hold_all.tie_low_lp_pad_hold_all = 1;
}

#ifdef __cplusplus
}
#endif
