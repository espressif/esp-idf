/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The ll is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

#pragma once

#include <stdbool.h>
#include "esp_bit_defs.h"
#include "soc/soc_caps.h"
#include "soc/lp_gpio_struct.h"
#include "soc/lp_iomux_struct.h"
#include "soc/lp_gpio_sig_map.h"
#include "soc/pmu_struct.h"
#include "hal/misc.h"
#include "hal/assert.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RTCIO_LL_PIN_FUNC       1 // LP_GPIO function

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
    RTCIO_INTR_DISABLE = 0,     /*!< Disable GPIO interrupt                             */
    RTCIO_INTR_POSEDGE = 1,     /*!< GPIO interrupt type : rising edge                  */
    RTCIO_INTR_NEGEDGE = 2,     /*!< GPIO interrupt type : falling edge                 */
    RTCIO_INTR_ANYEDGE = 3,     /*!< GPIO interrupt type : both rising and falling edge */
    RTCIO_INTR_LOW_LEVEL = 4,   /*!< GPIO interrupt type : input low level trigger      */
    RTCIO_INTR_HIGH_LEVEL = 5,  /*!< GPIO interrupt type : input high level trigger     */
} rtcio_ll_intr_type_t;

typedef enum {
    RTCIO_LL_OUTPUT_NORMAL = 0,    /*!< RTCIO output mode is normal. */
    RTCIO_LL_OUTPUT_OD = 0x1,      /*!< RTCIO output mode is open-drain. */
} rtcio_ll_out_mode_t;

/**
 * @brief Select RTC GPIO input to a signal.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @param signal_idx LP peripheral signal index.
 * @param inv True to invert input signal; False then no invert.
 */
static inline void rtcio_ll_matrix_in(int rtcio_num, uint32_t signal_idx, bool inv)
{
    lp_gpio_func_in_sel_cfg_reg_t reg;
    reg.func_in_sel = rtcio_num;
    reg.in_inv_sel = inv;
    reg.sig_in_sel = 1; // Signal should not bypass LP_GPIO matrix
    LP_GPIO.func_in_sel_cfg[signal_idx].val = reg.val;
}

/**
 * @brief Select signal output to a RTC GPIO.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @param signal_idx LP peripheral signal index.
 * @param out_inv True to invert output signal; False then no invert.
 * @param oen_inv True to invert output enable signal; False then no invert.
 */
static inline void rtcio_ll_matrix_out(int rtcio_num, uint32_t signal_idx, bool out_inv, bool oen_inv)
{
    lp_gpio_func_out_sel_cfg_reg_t reg;
    reg.func_out_sel = signal_idx;
    reg.out_inv_sel = out_inv;
    reg.oe_inv_sel = oen_inv;
    LP_GPIO.func_out_sel_cfg[rtcio_num].val = reg.val;

    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_GPIO.enable_w1ts, reg_gpio_enable_data_w1ts, BIT(rtcio_num));
}

/**
 * @brief Select a RTC IOMUX function for the RTC IO
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @param func Function to assign to the pin
 */
static inline void rtcio_ll_iomux_func_sel(int rtcio_num, int func)
{
    LP_IOMUX.pad[rtcio_num].fun_sel = func;
    // When using as normal (output) LP_GPIO, it is reasonable to ensure that no peripheral signal is routed to the pin
    if (func == RTCIO_LL_PIN_FUNC) {
        LP_GPIO.func_out_sel_cfg[rtcio_num].func_out_sel = SIG_LP_GPIO_OUT_IDX;
    }
}

/**
 * @brief Enable/Disable LP_GPIO peripheral clock.
 *
 * @param enable true to enable the clock / false to disable the clock
 */
static inline void _rtcio_ll_enable_io_clock(bool enable)
{
    LP_GPIO.clk_en.reg_clk_en = enable;
    while (LP_GPIO.clk_en.reg_clk_en != enable) {
        ;
    }
}

#define rtcio_ll_enable_io_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; _rtcio_ll_enable_io_clock(__VA_ARGS__)

/**
 * @brief Select the lp_gpio/hp_gpio function to control the pad.
 *
 * @note The RTC function must be selected before the pad analog function is enabled.
 * @note The clock gating 'HP_SYS_CLKRST.soc_clk_ctrl3.reg_iomux_apb_clk_en' is the gate of 'lp_io', 'ana_cmpr', 'sdm', 'glitch_fliter', and 'etm_gpio'
 *       And it's default to be turned on, so we don't need to operate this clock gate here additionally
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @param func Select pin function.
 */
static inline void rtcio_ll_function_select(int rtcio_num, rtcio_ll_func_t func)
{
    if (func == RTCIO_LL_FUNC_RTC) {
        // 0: GPIO connected to digital GPIO module. 1: GPIO connected to analog RTC module.
        LP_IOMUX.pad[rtcio_num].mux_sel = 1;
        // LP_GPIO is FUNC 1
        rtcio_ll_iomux_func_sel(rtcio_num, RTCIO_LL_PIN_FUNC);
    } else if (func == RTCIO_LL_FUNC_DIGITAL) {
        // Clear the bit to use digital GPIO module
        LP_IOMUX.pad[rtcio_num].mux_sel = 0;
    }
}

/**
 * @brief Enable RTCIO output.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_output_enable(int rtcio_num)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_GPIO.enable_w1ts, reg_gpio_enable_data_w1ts, BIT(rtcio_num));
}

/**
 * @brief Disable RTCIO output.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_output_disable(int rtcio_num)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_GPIO.enable_w1tc, reg_gpio_enable_data_w1tc, BIT(rtcio_num));
    // Ensure no other output signal is routed via LP_GPIO matrix to this pin
    LP_GPIO.func_out_sel_cfg[rtcio_num].func_out_sel = SIG_LP_GPIO_OUT_IDX;
}

/**
 * @brief Set RTCIO output level.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @param level 0: output low; 1: output high.
 */
static inline void rtcio_ll_set_level(int rtcio_num, uint32_t level)
{
    if (level) {
        HAL_FORCE_MODIFY_U32_REG_FIELD(LP_GPIO.out_w1ts, reg_gpio_out_data_w1ts, BIT(rtcio_num));
    } else {
        HAL_FORCE_MODIFY_U32_REG_FIELD(LP_GPIO.out_w1tc, reg_gpio_out_data_w1tc, BIT(rtcio_num));
    }
}

/**
 * @brief Enable RTCIO input.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_input_enable(int rtcio_num)
{
    LP_IOMUX.pad[rtcio_num].fun_ie = 1;
}

/**
 * @brief Disable RTCIO input.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_input_disable(int rtcio_num)
{
    LP_IOMUX.pad[rtcio_num].fun_ie = 0;
}

/**
 * @brief Get RTCIO input level.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 *
 * @return 0: input low; 1: input high.
 */
static inline uint32_t rtcio_ll_get_level(int rtcio_num)
{
    return (uint32_t)(HAL_FORCE_READ_U32_REG_FIELD(LP_GPIO.in, reg_gpio_in_data_next) >> rtcio_num) & 0x1;
}

/**
 * @brief Set RTC GPIO pad drive capability
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @param strength Drive capability of the pad. Range: 0 ~ 3.
 */
static inline void rtcio_ll_set_drive_capability(int rtcio_num, uint32_t strength)
{
    LP_IOMUX.pad[rtcio_num].drv = strength;
}

/**
 * @brief Get RTC GPIO pad drive capability.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 *
 * @return Drive capability of the pad. Range: 0 ~ 3.
 */
static inline uint32_t rtcio_ll_get_drive_capability(int rtcio_num)
{
    return LP_IOMUX.pad[rtcio_num].drv;
}

/**
 * @brief Set RTC GPIO pad output mode.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 *
 * @return mode Output mode.
 */
static inline void rtcio_ll_output_mode_set(int rtcio_num, rtcio_ll_out_mode_t mode)
{
    LP_GPIO.pin[rtcio_num].pad_driver = mode;
}

/**
 * @brief RTC GPIO pullup enable.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_pullup_enable(int rtcio_num)
{
    /* Enable internal weak pull-up */
    LP_IOMUX.pad[rtcio_num].rue = 1;
}

/**
 * @brief RTC GPIO pullup disable.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_pullup_disable(int rtcio_num)
{
    /* Disable internal weak pull-up */
    LP_IOMUX.pad[rtcio_num].rue = 0;
}

/**
 * @brief Get RTC GPIO pad pullup status.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @return Whether the pullup of the pad is enabled or not.
 */
static inline bool rtcio_ll_is_pullup_enabled(int rtcio_num)
{
    return LP_IOMUX.pad[rtcio_num].rue;
}

/**
 * @brief RTC GPIO pulldown enable.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_pulldown_enable(int rtcio_num)
{
    /* Enable internal weak pull-down */
    LP_IOMUX.pad[rtcio_num].rde = 1;
}

/**
 * @brief RTC GPIO pulldown disable.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_pulldown_disable(int rtcio_num)
{
    /* Enable internal weak pull-down */
    LP_IOMUX.pad[rtcio_num].rde = 0;
}

/**
 * @brief Get RTC GPIO pad pulldown status.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @return Whether the pulldown of the pad is enabled or not.
 */
static inline bool rtcio_ll_is_pulldown_enabled(int rtcio_num)
{
    return LP_IOMUX.pad[rtcio_num].rde;
}

/**
 * @brief Enable force hold function for an RTC IO pad.
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
    uint32_t hold_mask = HAL_FORCE_READ_U32_REG_FIELD(LP_IOMUX.lp_pad_hold, reg_lp_gpio_hold);
    hold_mask |= BIT(rtcio_num);
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_IOMUX.lp_pad_hold, reg_lp_gpio_hold, hold_mask);
}

/**
 * @brief Disable hold function on an RTC IO pad
 *
 * @note If disable the pad hold, the status of pad maybe changed in sleep mode.
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_force_hold_disable(int rtcio_num)
{
    uint32_t hold_mask = HAL_FORCE_READ_U32_REG_FIELD(LP_IOMUX.lp_pad_hold, reg_lp_gpio_hold);
    hold_mask &= ~BIT(rtcio_num);
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_IOMUX.lp_pad_hold, reg_lp_gpio_hold, hold_mask);
}

/**
 * @brief Enable force hold function for all RTC IO pads
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
 * @brief Disable hold function fon all RTC IO pads
 *
 * @note If disable the pad hold, the status of pad maybe changed in sleep mode.
 */
static inline void rtcio_ll_force_unhold_all(void)
{
    PMU.imm.pad_hold_all.tie_low_lp_pad_hold_all = 1;
}

/**
 * @brief Enable wakeup function and set wakeup type from light sleep or deep sleep for RTCIO.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @param type  Wakeup on high level or low level.
 */
static inline void rtcio_ll_wakeup_enable(int rtcio_num, rtcio_ll_wake_type_t type)
{
    LP_GPIO.pin[rtcio_num].wakeup_enable = 1;
    LP_GPIO.pin[rtcio_num].int_type = type;
}

/**
 * @brief Disable wakeup function from light sleep or deep sleep for RTCIO.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_wakeup_disable(int rtcio_num)
{
    LP_GPIO.pin[rtcio_num].wakeup_enable = 0;
    LP_GPIO.pin[rtcio_num].int_type = RTCIO_LL_WAKEUP_DISABLE;
}

/**
 * Enable interrupt function and set interrupt type
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @param type  Interrupt type on high level or low level.
 */
static inline void rtcio_ll_intr_enable(int rtcio_num, rtcio_ll_intr_type_t type)
{
    LP_GPIO.pin[rtcio_num].int_type = type;
}

/**
 * @brief Enable RTCIO output in deep sleep.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_enable_output_in_sleep(int rtcio_num)
{
    LP_IOMUX.pad[rtcio_num].slp_oe = 1;
}

/**
 * @brief Disable RTCIO output in deep sleep.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_disable_output_in_sleep(int rtcio_num)
{
    LP_IOMUX.pad[rtcio_num].slp_oe = 0;
}

/**
 * @brief Enable RTCIO input in deep sleep.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_enable_input_in_sleep(int rtcio_num)
{
    LP_IOMUX.pad[rtcio_num].slp_ie = 1;
}

/**
 * @brief Disable RTCIO input in deep sleep.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_disable_input_in_sleep(int rtcio_num)
{
    LP_IOMUX.pad[rtcio_num].slp_ie = 0;
}

/**
 * @brief Enable RTCIO keep another setting in deep sleep.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_enable_sleep_setting(int rtcio_num)
{
    LP_IOMUX.pad[rtcio_num].slp_sel = 1;
}

/**
 * @brief Disable RTCIO keep another setting in deep sleep. (Default)
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_disable_sleep_setting(int rtcio_num)
{
    LP_IOMUX.pad[rtcio_num].slp_sel = 0;
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
    return LP_GPIO.pin[rtcio_num].wakeup_enable;
}

/**
 * @brief Get the LP IO interrupt status
 *
 * @return  Bit 0~15 corresponding to 0 ~ SOC_RTCIO_PIN_COUNT.
 */
static inline uint32_t rtcio_ll_get_interrupt_status(void)
{
    return (uint32_t)HAL_FORCE_READ_U32_REG_FIELD(LP_GPIO.status, reg_gpio_status_data);
}

/**
 * @brief Clear all LP IO pads status
 */
static inline  void rtcio_ll_clear_interrupt_status(void)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_GPIO.status_w1tc, reg_gpio_status_data_w1tc, 0xFFFF);
}

#ifdef __cplusplus
}
#endif
