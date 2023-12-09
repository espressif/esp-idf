/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The ll is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "soc/soc_caps.h"
#include "soc/pcr_struct.h"
#include "soc/lp_io_struct.h"
#include "soc/lp_aon_struct.h"
#include "soc/pmu_struct.h"
#include "hal/misc.h"
#include "hal/assert.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RTCIO_LL_PIN_FUNC       0

typedef enum {
    RTCIO_LL_FUNC_RTC = 0x0,         /*!< The pin controlled by RTC module. */
    RTCIO_LL_FUNC_DIGITAL = 0x1,     /*!< The pin controlled by DIGITAL module. */
} rtcio_ll_func_t;

typedef enum {
    RTCIO_LL_WAKEUP_DISABLE    = 0,    /*!< Disable GPIO interrupt                             */
    RTCIO_LL_WAKEUP_LOW_LEVEL  = 0x4,  /*!< GPIO interrupt type : input low level trigger      */
    RTCIO_LL_WAKEUP_HIGH_LEVEL = 0x5,  /*!< GPIO interrupt type : input high level trigger     */
} rtcio_ll_wake_type_t;

typedef enum {
    RTCIO_LL_OUTPUT_NORMAL = 0,    /*!< RTCIO output mode is normal. */
    RTCIO_LL_OUTPUT_OD = 0x1,      /*!< RTCIO output mode is open-drain. */
} rtcio_ll_out_mode_t;

/**
 * @brief Select a RTC IOMUX function for the RTC IO
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @param func Function to assign to the pin
 */
static inline void rtcio_ll_iomux_func_sel(int rtcio_num, int func)
{
    LP_IO.gpio[rtcio_num].mcu_sel = func;
}

/**
 * @brief Select the rtcio function.
 *
 * @note The RTC function must be selected before the pad analog function is enabled.
 * @note The clock gating 'PCR.iomux_conf.iomux_clk_en' is the gate of both 'lp_io' and 'etm_gpio'
 *       And it's default to be turned on, so we don't need to operate this clock gate here additionally
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @param func Select pin function.
 */
static inline void rtcio_ll_function_select(int rtcio_num, rtcio_ll_func_t func)
{
    if (func == RTCIO_LL_FUNC_RTC) {
        // 0: GPIO connected to digital GPIO module. 1: GPIO connected to analog RTC module.
        uint32_t sel_mask = HAL_FORCE_READ_U32_REG_FIELD(LP_AON.gpio_mux, gpio_mux_sel);
        sel_mask |= BIT(rtcio_num);
        HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.gpio_mux, gpio_mux_sel, sel_mask);
        //0:RTC FUNCTION 1,2,3:Reserved
        rtcio_ll_iomux_func_sel(rtcio_num, RTCIO_LL_PIN_FUNC);
    } else if (func == RTCIO_LL_FUNC_DIGITAL) {
        // Clear the bit to use digital GPIO module
        uint32_t sel_mask = HAL_FORCE_READ_U32_REG_FIELD(LP_AON.gpio_mux, gpio_mux_sel);
        sel_mask &= ~BIT(rtcio_num);
        HAL_FORCE_MODIFY_U32_REG_FIELD(LP_AON.gpio_mux, gpio_mux_sel, sel_mask);
    }
}

/**
 * Enable rtcio output.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_output_enable(int rtcio_num)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_IO.out_enable_w1ts, enable_w1ts, BIT(rtcio_num));
}

/**
 * Disable rtcio output.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_output_disable(int rtcio_num)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_IO.out_enable_w1tc, enable_w1tc, BIT(rtcio_num));
}

/**
 * Set RTCIO output level.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @param level 0: output low; ~0: output high.
 */
static inline void rtcio_ll_set_level(int rtcio_num, uint32_t level)
{
    if (level) {
        HAL_FORCE_MODIFY_U32_REG_FIELD(LP_IO.out_data_w1ts, out_data_w1ts, BIT(rtcio_num));
    } else {
        HAL_FORCE_MODIFY_U32_REG_FIELD(LP_IO.out_data_w1tc, out_data_w1tc, BIT(rtcio_num));
    }
}

/**
 * Enable rtcio input.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_input_enable(int rtcio_num)
{
    LP_IO.gpio[rtcio_num].fun_ie = 1;
}

/**
 * Disable rtcio input.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_input_disable(int rtcio_num)
{
    LP_IO.gpio[rtcio_num].fun_ie = 0;
}

/**
 * Get RTCIO input level.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @return 0: input low; ~0: input high.
 */
static inline uint32_t rtcio_ll_get_level(int rtcio_num)
{
    return (uint32_t)(HAL_FORCE_READ_U32_REG_FIELD(LP_IO.in, in_data_next) >> rtcio_num) & 0x1;
}

/**
 * @brief Set RTC GPIO pad drive capability
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @param strength Drive capability of the pad. Range: 0 ~ 3.
 */
static inline void rtcio_ll_set_drive_capability(int rtcio_num, uint32_t strength)
{
    LP_IO.gpio[rtcio_num].fun_drv = strength;
}

/**
 * @brief Get RTC GPIO pad drive capability.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @return Drive capability of the pad. Range: 0 ~ 3.
 */
static inline uint32_t rtcio_ll_get_drive_capability(int rtcio_num)
{
    return LP_IO.gpio[rtcio_num].fun_drv;
}

/**
 * @brief Set RTC GPIO pad output mode.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @return mode Output mode.
 */
static inline void rtcio_ll_output_mode_set(int rtcio_num, rtcio_ll_out_mode_t mode)
{
    LP_IO.pin[rtcio_num].pad_driver = mode;
}

/**
 * RTC GPIO pullup enable.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_pullup_enable(int rtcio_num)
{
    /* Enable internal weak pull-up */
    LP_IO.gpio[rtcio_num].fun_wpu = 1;
}

/**
 * RTC GPIO pullup disable.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_pullup_disable(int rtcio_num)
{
    /* Disable internal weak pull-up */
    LP_IO.gpio[rtcio_num].fun_wpu = 0;
}

/**
 * RTC GPIO pulldown enable.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_pulldown_enable(int rtcio_num)
{
    /* Enable internal weak pull-down */
    LP_IO.gpio[rtcio_num].fun_wpd = 1;
}

/**
 * RTC GPIO pulldown disable.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_pulldown_disable(int rtcio_num)
{
    /* Enable internal weak pull-down */
    LP_IO.gpio[rtcio_num].fun_wpd = 0;
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
    LP_AON.gpio_hold0.gpio_hold0 |= BIT(rtcio_num);
}

/**
 * Disable hold function on an RTC IO pad
 *
 * @note If disable the pad hold, the status of pad maybe changed in sleep mode.
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_force_hold_disable(int rtcio_num)
{
    LP_AON.gpio_hold0.gpio_hold0 &= ~BIT(rtcio_num);
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

/**
 * Enable wakeup function and set wakeup type from light sleep or deep sleep for rtcio.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @param type  Wakeup on high level or low level.
 */
static inline void rtcio_ll_wakeup_enable(int rtcio_num, rtcio_ll_wake_type_t type)
{
    LP_IO.pin[rtcio_num].wakeup_enable = 0x1;
    LP_IO.pin[rtcio_num].int_type = type;
}

/**
 * Disable wakeup function from light sleep or deep sleep for rtcio.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_wakeup_disable(int rtcio_num)
{
    LP_IO.pin[rtcio_num].wakeup_enable = 0;
    LP_IO.pin[rtcio_num].int_type = RTCIO_LL_WAKEUP_DISABLE;
}

/**
 * Enable rtc io output in deep sleep.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_enable_output_in_sleep(int rtcio_num)
{
    LP_IO.gpio[rtcio_num].mcu_oe = 1;
}

/**
 * Disable rtc io output in deep sleep.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_disable_output_in_sleep(int rtcio_num)
{
    LP_IO.gpio[rtcio_num].mcu_oe = 0;
}

/**
 * Enable rtc io input in deep sleep.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_enable_input_in_sleep(int rtcio_num)
{
    LP_IO.gpio[rtcio_num].mcu_ie = 1;
}

/**
 * Disable rtc io input in deep sleep.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_disable_input_in_sleep(int rtcio_num)
{
    LP_IO.gpio[rtcio_num].mcu_ie = 0;
}

/**
 * Enable rtc io keep another setting in deep sleep.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_enable_sleep_setting(int rtcio_num)
{
    LP_IO.gpio[rtcio_num].slp_sel = 1;
}

/**
 * Disable rtc io keep another setting in deep sleep. (Default)
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_disable_sleep_setting(int rtcio_num)
{
    LP_IO.gpio[rtcio_num].slp_sel = 0;
}

/**
 * @brief Get the status of whether an IO is used for sleep wake-up.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @return True if the pin is enabled to wake up from deep-sleep
 */
static inline bool rtcio_ll_wakeup_is_enabled(int rtcio_num)
{
    HAL_ASSERT(rtcio_num >= 0 && rtcio_num < SOC_RTCIO_PIN_COUNT && "io does not support deep sleep wake-up function");
    return LP_IO.pin[rtcio_num].wakeup_enable;
}

/**
 * @brief Get the rtc io interrupt status
 *
 * @return  bit 0~7 corresponding to 0 ~ SOC_RTCIO_PIN_COUNT.
 */
static inline  uint32_t rtcio_ll_get_interrupt_status(void)
{
    return (uint32_t)HAL_FORCE_READ_U32_REG_FIELD(LP_IO.status, status_interrupt);
}

/**
 * @brief Clear all LP IO pads status
 */
static inline  void rtcio_ll_clear_interrupt_status(void)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_IO.status_w1tc, status_w1tc, 0xff);
}

#ifdef __cplusplus
}
#endif
