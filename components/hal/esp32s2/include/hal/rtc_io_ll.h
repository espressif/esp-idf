// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*******************************************************************************
 * NOTICE
 * The ll is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

#pragma once

#include <stdlib.h>
#include "soc/rtc_io_periph.h"
#include "hal/rtc_io_types.h"
#include "hal/gpio_types.h"

#define RTCIO_LL_PIN_FUNC     0

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    RTCIO_FUNC_RTC = 0x0,         /*!< The pin controled by RTC module. */
    RTCIO_FUNC_DIGITAL = 0x1,     /*!< The pin controlled by DIGITAL module. */
} rtcio_ll_func_t;

typedef enum {
    RTCIO_WAKEUP_DISABLE    = 0,    /*!< Disable GPIO interrupt                             */
    RTCIO_WAKEUP_LOW_LEVEL  = 0x4,  /*!< GPIO interrupt type : input low level trigger      */
    RTCIO_WAKEUP_HIGH_LEVEL = 0x5,  /*!< GPIO interrupt type : input high level trigger     */
} rtcio_ll_wake_type_t;

typedef enum {
    RTCIO_OUTPUT_NORMAL = 0,    /*!< RTCIO output mode is normal. */
    RTCIO_OUTPUT_OD = 0x1,      /*!< RTCIO output mode is open-drain. */
} rtcio_ll_out_mode_t;

/**
 * @brief Select the rtcio function.
 *
 * @note The RTC function must be selected before the pad analog function is enabled.
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @param func Select pin function.
 */
static inline void rtcio_ll_function_select(int rtcio_num, rtcio_ll_func_t func)
{
    if (func == RTCIO_FUNC_RTC) {
        SENS.sar_io_mux_conf.iomux_clk_gate_en = 1;
        // 0: GPIO connected to digital GPIO module. 1: GPIO connected to analog RTC module.
        SET_PERI_REG_MASK(rtc_io_desc[rtcio_num].reg, (rtc_io_desc[rtcio_num].mux));
        //0:RTC FUNCTION 1,2,3:Reserved
        SET_PERI_REG_BITS(rtc_io_desc[rtcio_num].reg, RTC_IO_TOUCH_PAD1_FUN_SEL_V, RTCIO_LL_PIN_FUNC, rtc_io_desc[rtcio_num].func);
    } else if (func == RTCIO_FUNC_DIGITAL) {
        CLEAR_PERI_REG_MASK(rtc_io_desc[rtcio_num].reg, (rtc_io_desc[rtcio_num].mux));
        SENS.sar_io_mux_conf.iomux_clk_gate_en = 0;
    }
}

/**
 * Enable rtcio output.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_output_enable(int rtcio_num)
{
    RTCIO.enable_w1ts.w1ts = (1U << rtcio_num);
}

/**
 * Disable rtcio output.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_output_disable(int rtcio_num)
{
    RTCIO.enable_w1tc.w1tc = (1U << rtcio_num);
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
        RTCIO.out_w1ts.w1ts = (1U << rtcio_num);
    } else {
        RTCIO.out_w1tc.w1tc = (1U << rtcio_num);
    }
}

/**
 * Enable rtcio input.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_input_enable(int rtcio_num)
{
    SET_PERI_REG_MASK(rtc_io_desc[rtcio_num].reg, rtc_io_desc[rtcio_num].ie);
}

/**
 * Disable rtcio input.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_input_disable(int rtcio_num)
{
    CLEAR_PERI_REG_MASK(rtc_io_desc[rtcio_num].reg, rtc_io_desc[rtcio_num].ie);
}

/**
 * Get RTCIO input level.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @return 0: input low; ~0: input high.
 */
static inline uint32_t rtcio_ll_get_level(int rtcio_num)
{
    return (uint32_t)(RTCIO.in_val.in >> rtcio_num) & 0x1;
}

/**
 * @brief Set RTC GPIO pad drive capability
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @param strength Drive capability of the pad. Range: 0 ~ 3.
 */
static inline void rtcio_ll_set_drive_capability(int rtcio_num, uint32_t strength)
{
    if (rtc_io_desc[rtcio_num].drv_v) {
        SET_PERI_REG_BITS(rtc_io_desc[rtcio_num].reg, rtc_io_desc[rtcio_num].drv_v, strength, rtc_io_desc[rtcio_num].drv_s);
    }
}

/**
 * @brief Get RTC GPIO pad drive capability.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @return Drive capability of the pad. Range: 0 ~ 3.
 */
static inline uint32_t rtcio_ll_get_drive_capability(int rtcio_num)
{
    return GET_PERI_REG_BITS2(rtc_io_desc[rtcio_num].reg, rtc_io_desc[rtcio_num].drv_v, rtc_io_desc[rtcio_num].drv_s);
}

/**
 * @brief Set RTC GPIO pad output mode.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @return mode Output mode.
 */
static inline void rtcio_ll_output_mode_set(int rtcio_num, rtcio_ll_out_mode_t mode)
{
    RTCIO.pin[rtcio_num].pad_driver = mode;
}

/**
 * RTC GPIO pullup enable.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_pullup_enable(int rtcio_num)
{
    if (rtc_io_desc[rtcio_num].pullup) {
        SET_PERI_REG_MASK(rtc_io_desc[rtcio_num].reg, rtc_io_desc[rtcio_num].pullup);
    }
}

/**
 * RTC GPIO pullup disable.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_pullup_disable(int rtcio_num)
{
    if (rtc_io_desc[rtcio_num].pullup) {
        CLEAR_PERI_REG_MASK(rtc_io_desc[rtcio_num].reg, rtc_io_desc[rtcio_num].pullup);
    }
}

/**
 * RTC GPIO pulldown enable.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_pulldown_enable(int rtcio_num)
{
    if (rtc_io_desc[rtcio_num].pulldown) {
        SET_PERI_REG_MASK(rtc_io_desc[rtcio_num].reg, rtc_io_desc[rtcio_num].pulldown);
    }
}

/**
 * RTC GPIO pulldown disable.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_pulldown_disable(int rtcio_num)
{
    if (rtc_io_desc[rtcio_num].pulldown) {
        CLEAR_PERI_REG_MASK(rtc_io_desc[rtcio_num].reg, rtc_io_desc[rtcio_num].pulldown);
    }
}

/**
 * Enable force hold function for RTC IO pad.
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
    SET_PERI_REG_MASK(RTC_CNTL_PAD_HOLD_REG, rtc_io_desc[rtcio_num].hold_force);
}

/**
 * Disable hold function on an RTC IO pad
 *
 * @note If disable the pad hold, the status of pad maybe changed in sleep mode.
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_force_hold_disable(int rtcio_num)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_PAD_HOLD_REG, rtc_io_desc[rtcio_num].hold_force);
}

/**
 * Enable force hold function for RTC IO pad.
 *
 * Enabling HOLD function will cause the pad to lock current status, such as,
 * input/output enable, input/output value, function, drive strength values.
 * This function is useful when going into light or deep sleep mode to prevent
 * the pin configuration from changing.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_force_hold_all(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_PAD_FORCE_HOLD_M);
}

/**
 * Disable hold function on an RTC IO pad
 *
 * @note If disable the pad hold, the status of pad maybe changed in sleep mode.
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_force_unhold_all(void)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_PWC_REG, RTC_CNTL_PAD_FORCE_HOLD_M);
}

/**
 * Enable wakeup function and set wakeup type from light sleep status for rtcio.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @param type  Wakeup on high level or low level.
 */
static inline void rtcio_ll_wakeup_enable(int rtcio_num, rtcio_ll_wake_type_t type)
{
    RTCIO.pin[rtcio_num].wakeup_enable = 0x1;
    RTCIO.pin[rtcio_num].int_type = type;
}

/**
 * Disable wakeup function from light sleep status for rtcio.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_wakeup_disable(int rtcio_num)
{
    RTCIO.pin[rtcio_num].wakeup_enable = 0;
    RTCIO.pin[rtcio_num].int_type = RTCIO_WAKEUP_DISABLE;
}

/**
 * Enable rtc io output in deep sleep.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_enable_output_in_sleep(gpio_num_t gpio_num)
{
    if (rtc_io_desc[gpio_num].slpoe) {
        SET_PERI_REG_MASK(rtc_io_desc[gpio_num].reg, rtc_io_desc[gpio_num].slpoe);
    }
}

/**
 * Disable rtc io output in deep sleep.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_in_sleep_disable_output(gpio_num_t gpio_num)
{
    if (rtc_io_desc[gpio_num].slpoe) {
        CLEAR_PERI_REG_MASK(rtc_io_desc[gpio_num].reg, rtc_io_desc[gpio_num].slpoe);
    }
}

/**
 * Enable rtc io input in deep sleep.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_in_sleep_enable_input(gpio_num_t gpio_num)
{
    SET_PERI_REG_MASK(rtc_io_desc[gpio_num].reg, rtc_io_desc[gpio_num].slpie);
}

/**
 * Disable rtc io input in deep sleep.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_in_sleep_disable_input(gpio_num_t gpio_num)
{
    CLEAR_PERI_REG_MASK(rtc_io_desc[gpio_num].reg, rtc_io_desc[gpio_num].slpie);
}

/**
 * Enable rtc io keep another setting in deep sleep.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_enable_sleep_setting(gpio_num_t gpio_num)
{
    SET_PERI_REG_MASK(rtc_io_desc[gpio_num].reg, rtc_io_desc[gpio_num].slpsel);
}

/**
 * Disable rtc io keep another setting in deep sleep. (Default)
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_disable_sleep_setting(gpio_num_t gpio_num)
{
    CLEAR_PERI_REG_MASK(rtc_io_desc[gpio_num].reg, rtc_io_desc[gpio_num].slpsel);
}

static inline void rtcio_ll_ext0_set_wakeup_pin(int rtcio_num, int level)
{
    REG_SET_FIELD(RTC_IO_EXT_WAKEUP0_REG, RTC_IO_EXT_WAKEUP0_SEL, rtcio_num);
    // Set level which will trigger wakeup
    SET_PERI_REG_BITS(RTC_CNTL_EXT_WAKEUP_CONF_REG, 0x1,
            level , RTC_CNTL_EXT_WAKEUP0_LV_S);
}

#ifdef __cplusplus
}
#endif