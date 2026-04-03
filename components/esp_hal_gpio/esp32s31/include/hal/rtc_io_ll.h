/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
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
#include "soc/lp_gpio_struct.h"
#include "soc/lp_iomux_struct.h"
#include "soc/lp_gpio_sig_map.h"
#include "soc/lp_system_struct.h"
#include "soc/lp_peri_clkrst_struct.h"
#include "soc/pmu_struct.h"
#include "hal/gpio_types.h"
#include "hal/misc.h"
#include "hal/assert.h"

#define RTCIO_LL_PIN_FUNC           1

#define RTCIO_LL_GPIO_NUM_OFFSET    0 // rtcio 0-7 correspond to gpio 0-7

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    RTCIO_LL_FUNC_RTC = 0x0,         /*!< The pin controlled by RTC module. */
    RTCIO_LL_FUNC_DIGITAL = 0x1,     /*!< The pin controlled by DIGITAL module. */
} rtcio_ll_func_t;

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
    LP_IO_MUX.gpion[rtcio_num].gpion_mcu_sel = func;
    if (func == RTCIO_LL_PIN_FUNC) {
        LP_GPIO.funcn_out_sel_cfg[rtcio_num].funcn_out_sel = LP_SIG_GPIO_OUT_IDX;
    }
}

/**
 * @brief Enable/Disable LP_GPIO peripheral clock.
 *
 * @param enable true to enable the clock / false to disable the clock
 */
static inline void _rtcio_ll_enable_io_clock(bool enable)
{
    LP_PERI_CLKRST.iomux_ctrl.lp_iomux_clk_en = enable;
    LP_GPIO.clock_gate.clk_en = enable;
    while ((LP_PERI_CLKRST.iomux_ctrl.lp_iomux_clk_en != enable) ||
            (LP_GPIO.clock_gate.clk_en != enable)) {
        ;
    }
}

static inline void rtcio_ll_enable_io_clock(bool enable)
{
    _rtcio_ll_enable_io_clock(enable);
}

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
    if (func == RTCIO_LL_FUNC_RTC) {
        uint32_t sel = HAL_FORCE_READ_U32_REG_FIELD(LP_SYS.padctrl, pad_mux_sel);
        sel |= BIT(rtcio_num);
        HAL_FORCE_MODIFY_U32_REG_FIELD(LP_SYS.padctrl, pad_mux_sel, sel);
        rtcio_ll_iomux_func_sel(rtcio_num, RTCIO_LL_PIN_FUNC);
    } else if (func == RTCIO_LL_FUNC_DIGITAL) {
        uint32_t sel = HAL_FORCE_READ_U32_REG_FIELD(LP_SYS.padctrl, pad_mux_sel);
        sel &= ~BIT(rtcio_num);
        HAL_FORCE_MODIFY_U32_REG_FIELD(LP_SYS.padctrl, pad_mux_sel, sel);
    }
}

/**
 * @brief Enable rtcio output.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_output_enable(int rtcio_num)
{
    LP_GPIO.enable_w1ts.val = BIT(rtcio_num);
}

/**
 * @brief Disable rtcio output.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_output_disable(int rtcio_num)
{
    LP_GPIO.enable_w1tc.val = BIT(rtcio_num);
    // Ensure no other output signal is routed via LP_GPIO matrix to this pin
    LP_GPIO.funcn_out_sel_cfg[rtcio_num].funcn_out_sel = LP_SIG_GPIO_OUT_IDX;
}

/**
 * @brief Set RTCIO output level.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @param level 0: output low; ~0: output high.
 */
static inline void rtcio_ll_set_level(int rtcio_num, uint32_t level)
{
    if (level) {
        LP_GPIO.out_w1ts.val = BIT(rtcio_num);
    } else {
        LP_GPIO.out_w1tc.val = BIT(rtcio_num);
    }
}

/**
 * @brief Enable rtcio input.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_input_enable(int rtcio_num)
{
    LP_IO_MUX.gpion[rtcio_num].gpion_fun_ie = 1;
}

/**
 * @brief Disable rtcio input.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_input_disable(int rtcio_num)
{
    LP_IO_MUX.gpion[rtcio_num].gpion_fun_ie = 0;
}

/**
 * @brief Get RTCIO input level.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @return 0: input low; ~0: input high.
 */
static inline uint32_t rtcio_ll_get_level(int rtcio_num)
{
    return (HAL_FORCE_READ_U32_REG_FIELD(LP_GPIO.in, in_data_next) >> rtcio_num) & 0x1;
}

/**
 * @brief Set RTC GPIO pad drive capability
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @param strength Drive capability of the pad. Range: 0 ~ 3.
 */
static inline void rtcio_ll_set_drive_capability(int rtcio_num, uint32_t strength)
{
    LP_IO_MUX.gpion[rtcio_num].gpion_fun_drv = strength;
}

/**
 * @brief Get RTC GPIO pad drive capability.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @return Drive capability of the pad. Range: 0 ~ 3.
 */
static inline uint32_t rtcio_ll_get_drive_capability(int rtcio_num)
{
    return LP_IO_MUX.gpion[rtcio_num].gpion_fun_drv;
}

/**
 * @brief Set RTC GPIO pad output mode.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @return mode Output mode.
 */
static inline void rtcio_ll_output_mode_set(int rtcio_num, rtcio_ll_out_mode_t mode)
{
    LP_GPIO.pinn[rtcio_num].pinn_pad_driver = mode;
}

/**
 * @brief RTC GPIO pullup enable.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_pullup_enable(int rtcio_num)
{
    LP_IO_MUX.gpion[rtcio_num].gpion_fun_wpu = 1;
}

/**
 * @brief RTC GPIO pullup disable.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_pullup_disable(int rtcio_num)
{
    LP_IO_MUX.gpion[rtcio_num].gpion_fun_wpu = 0;
}

/**
 * @brief Get RTC GPIO pad pullup status.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @return Whether the pullup of the pad is enabled or not.
 */
static inline bool rtcio_ll_is_pullup_enabled(int rtcio_num)
{
    return LP_IO_MUX.gpion[rtcio_num].gpion_fun_wpu;
}

/**
 * @brief RTC GPIO pulldown enable.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_pulldown_enable(int rtcio_num)
{
    LP_IO_MUX.gpion[rtcio_num].gpion_fun_wpd = 1;
}

/**
 * @brief RTC GPIO pulldown disable.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_pulldown_disable(int rtcio_num)
{
    LP_IO_MUX.gpion[rtcio_num].gpion_fun_wpd = 0;
}

/**
 * @brief Get RTC GPIO pad pulldown status.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @return Whether the pulldown of the pad is enabled or not.
 */
static inline bool rtcio_ll_is_pulldown_enabled(int rtcio_num)
{
    return LP_IO_MUX.gpion[rtcio_num].gpion_fun_wpd;
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
    uint32_t hold = HAL_FORCE_READ_U32_REG_FIELD(LP_SYS.padctrl, pad_hold);
    hold |= BIT(rtcio_num);
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_SYS.padctrl, pad_hold, hold);
}

/**
 * @brief Disable hold function on an RTC IO pad
 *
 * @note If disable the pad hold, the status of pad maybe changed in sleep mode.
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_force_hold_disable(int rtcio_num)
{
    uint32_t hold = HAL_FORCE_READ_U32_REG_FIELD(LP_SYS.padctrl, pad_hold);
    hold &= ~BIT(rtcio_num);
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_SYS.padctrl, pad_hold, hold);
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
 * @brief Enable wakeup function and set wakeup type from light sleep or deep sleep for rtcio.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @param type  Wakeup on high level or low level.
 */
static inline void rtcio_ll_wakeup_enable(int rtcio_num, gpio_int_type_t type)
{
    LP_GPIO.pinn[rtcio_num].pinn_wakeup_enable = 1;
    LP_GPIO.pinn[rtcio_num].pinn_int_type = type;
}

/**
 * @brief Disable wakeup function from light sleep or deep sleep for rtcio.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_wakeup_disable(int rtcio_num)
{
    LP_GPIO.pinn[rtcio_num].pinn_wakeup_enable = 0;
    LP_GPIO.pinn[rtcio_num].pinn_int_type = 0;
}

/**
 * @brief Enable interrupt function and set interrupt type
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @param type  Interrupt type on high level or low level.
 */
static inline void rtcio_ll_intr_enable(int rtcio_num, gpio_int_type_t type)
{
    LP_GPIO.pinn[rtcio_num].pinn_int_type = type;
}

/**
 * @brief Enable rtc io output in deep sleep.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_enable_output_in_sleep(int rtcio_num)
{
    LP_IO_MUX.gpion[rtcio_num].gpion_mcu_oe = 1;
}

/**
 * @brief Disable rtc io output in deep sleep.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_disable_output_in_sleep(int rtcio_num)
{
    LP_IO_MUX.gpion[rtcio_num].gpion_mcu_oe = 0;
}

/**
 * @brief Enable rtc io input in deep sleep.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_enable_input_in_sleep(int rtcio_num)
{
    LP_IO_MUX.gpion[rtcio_num].gpion_mcu_ie = 1;
}

/**
 * @brief Disable rtc io input in deep sleep.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_disable_input_in_sleep(int rtcio_num)
{
    LP_IO_MUX.gpion[rtcio_num].gpion_mcu_ie = 0;
}

/**
 * @brief Enable rtc io keep another setting in deep sleep.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_enable_sleep_setting(int rtcio_num)
{
    LP_IO_MUX.gpion[rtcio_num].gpion_slp_sel = 1;
}

/**
 * @brief Disable rtc io keep another setting in deep sleep. (Default)
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 */
static inline void rtcio_ll_disable_sleep_setting(int rtcio_num)
{
    LP_IO_MUX.gpion[rtcio_num].gpion_slp_sel = 0;
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
    return LP_GPIO.pinn[rtcio_num].pinn_wakeup_enable;
}

/**
 * @brief Get the rtc io interrupt status
 *
 * @return  bit 0~7 corresponding to 0 ~ SOC_RTCIO_PIN_COUNT.
 */
static inline uint32_t rtcio_ll_get_interrupt_status(void)
{
    return HAL_FORCE_READ_U32_REG_FIELD(LP_GPIO.status, status_interrupt);
}

/**
 * @brief Clear all LP IO pads status
 */
static inline void rtcio_ll_clear_interrupt_status(void)
{
    LP_GPIO.status_w1tc.val = 0xFF;
}

/**
 * @brief Select RTC GPIO input to a signal.
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @param signal_idx LP peripheral input signal index (0 .. IN_SIGNAL_MAX - 1).
 * @param inv True to invert input signal; False then no invert.
 */
static inline void rtcio_ll_matrix_in(int rtcio_num, int signal_idx, bool inv)
{
    lp_gpio_funca_in_sel_cfg_reg_t reg;
    reg.funca_in_sel = rtcio_num;
    reg.funca_in_inv_sel = inv;
    reg.siga_in_sel = 1; // Signal should not bypass LP_GPIO matrix
    LP_GPIO.funca_in_sel_cfg[signal_idx].val = reg.val;
}

/**
 * @brief Select signal output to a RTC GPIO
 *
 * @param rtcio_num The index of rtcio. 0 ~ MAX(rtcio).
 * @param signal_idx Peripheral output signal index (0 .. OUT_SIGNAL_MAX - 1).
 * @param out_inv True to invert output signal; False then no invert.
 * @param oen_inv True to invert output enable signal; False then no invert.
 */
static inline void rtcio_ll_matrix_out(int rtcio_num, int signal_idx, bool out_inv, bool oen_inv)
{
    lp_gpio_funcn_out_sel_cfg_reg_t reg;
    reg.funcn_out_sel = signal_idx;
    reg.funcn_out_inv_sel = out_inv;
    reg.funcn_oe_inv_sel = oen_inv;
    LP_GPIO.funcn_out_sel_cfg[rtcio_num].val = reg.val;

    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_GPIO.enable_w1ts, enable_w1ts, BIT(rtcio_num));
}

#ifdef __cplusplus
}
#endif
