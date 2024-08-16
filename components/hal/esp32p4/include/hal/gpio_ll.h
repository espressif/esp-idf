/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The LL layer for ESP32-P4 GPIO register operations

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "soc/soc.h"
#include "soc/gpio_periph.h"
#include "soc/gpio_struct.h"
#include "soc/io_mux_struct.h"
#include "soc/hp_system_struct.h"
#include "soc/lp_iomux_struct.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "soc/pmu_struct.h"
#include "soc/usb_serial_jtag_struct.h"
#include "soc/usb_wrap_struct.h"
#include "soc/clk_tree_defs.h"
#include "hal/gpio_types.h"
#include "hal/misc.h"
#include "hal/assert.h"

#ifdef __cplusplus
extern "C" {
#endif

// Get GPIO hardware instance with giving gpio num
#define GPIO_LL_GET_HW(num) (((num) == 0) ? (&GPIO) : NULL)

#define GPIO_LL_INTR0_ENA      (BIT(0))
#define GPIO_LL_INTR1_ENA      (BIT(1))
#define GPIO_LL_INTR2_ENA      (BIT(3))
#define GPIO_LL_INTR3_ENA      (BIT(4))

/**
 * @brief Get the configuration for an IO
 *
 * @param hw Peripheral GPIO hardware instance address.
 * @param gpio_num GPIO number
 * @param pu Pull-up enabled or not
 * @param pd Pull-down enabled or not
 * @param ie Input enabled or not
 * @param oe Output enabled or not
 * @param od Open-drain enabled or not
 * @param drv Drive strength value
 * @param fun_sel IOMUX function selection value
 * @param sig_out Outputting peripheral signal index
 * @param slp_sel Pin sleep mode enabled or not
 */
static inline void gpio_ll_get_io_config(gpio_dev_t *hw, uint32_t gpio_num,
                                         bool *pu, bool *pd, bool *ie, bool *oe, bool *od, uint32_t *drv,
                                         uint32_t *fun_sel, uint32_t *sig_out, bool *slp_sel)
{
    uint32_t bit_shift = (gpio_num < 32) ? gpio_num : (gpio_num - 32);
    uint32_t bit_mask = 1 << bit_shift;
    *pu = IO_MUX.gpio[gpio_num].fun_wpu;
    *pd = IO_MUX.gpio[gpio_num].fun_wpd;
    *ie = IO_MUX.gpio[gpio_num].fun_ie;
    *oe = (((gpio_num < 32) ? hw->enable.val : hw->enable1.val) & bit_mask) >> bit_shift;
    *od = hw->pin[gpio_num].pad_driver;
    *drv = IO_MUX.gpio[gpio_num].fun_drv;
    *fun_sel = IO_MUX.gpio[gpio_num].mcu_sel;
    *sig_out = hw->func_out_sel_cfg[gpio_num].out_sel;
    *slp_sel = IO_MUX.gpio[gpio_num].slp_sel;
}

/**
  * @brief Enable pull-up on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_pullup_en(gpio_dev_t *hw, uint32_t gpio_num)
{
    IO_MUX.gpio[gpio_num].fun_wpu = 1;
}

/**
  * @brief Disable pull-up on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
__attribute__((always_inline))
static inline void gpio_ll_pullup_dis(gpio_dev_t *hw, uint32_t gpio_num)
{
    // The pull-up value of the USB pins are controlled by the pins’ pull-up value together with USB pull-up value
    // USB DP pin is default to PU enabled
    // Note that esp32p4 has supported USB_EXCHG_PINS feature. If this efuse is burnt, the gpio pin
    // which should be checked is USB_INT_PHY0_DM_GPIO_NUM instead.
    // TODO: read the specific efuse with efuse_ll.h

    // One more noticeable point is P4 has two internal PHYs connecting to USJ and USB_WRAP(OTG1.1) separately.
    // We only consider the default connection here: PHY0 -> USJ, PHY1 -> USB_OTG
    if (gpio_num == USB_USJ_INT_PHY_DP_GPIO_NUM) {
        USB_SERIAL_JTAG.conf0.pad_pull_override = 1;
        USB_SERIAL_JTAG.conf0.dp_pullup = 0;
    } else if (gpio_num == USB_OTG_INT_PHY_DP_GPIO_NUM) {
        USB_WRAP.otg_conf.pad_pull_override = 1;
        USB_WRAP.otg_conf.dp_pullup = 0;
    }

    IO_MUX.gpio[gpio_num].fun_wpu = 0;
}

/**
  * @brief Enable pull-down on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_pulldown_en(gpio_dev_t *hw, uint32_t gpio_num)
{
    IO_MUX.gpio[gpio_num].fun_wpd = 1;
}

/**
  * @brief Disable pull-down on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
__attribute__((always_inline))
static inline void gpio_ll_pulldown_dis(gpio_dev_t *hw, uint32_t gpio_num)
{
    IO_MUX.gpio[gpio_num].fun_wpd = 0;
}

/**
 * @brief  GPIO set interrupt trigger type
 *
 * @param  hw Peripheral GPIO hardware instance address.
 * @param  gpio_num GPIO number. If you want to set the trigger type of e.g. of GPIO16, gpio_num should be GPIO_NUM_16 (16);
 * @param  intr_type Interrupt type, select from gpio_int_type_t
 */
static inline void gpio_ll_set_intr_type(gpio_dev_t *hw, uint32_t gpio_num, gpio_int_type_t intr_type)
{
    hw->pin[gpio_num].int_type = intr_type;
}

/**
  * @brief Get GPIO interrupt status
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param core_id interrupt core id
  * @param status interrupt status
  */
__attribute__((always_inline))
static inline void gpio_ll_get_intr_status(gpio_dev_t *hw, uint32_t core_id, uint32_t *status)
{
    (void)core_id; // TODO: IDF-7995 There are 4 interrupt sources for GPIO on P4. New feature! Need to fix this function later.
    *status = hw->intr_0.int_0;
}

/**
  * @brief Get GPIO interrupt status high
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param core_id interrupt core id
  * @param status interrupt status high
  */
__attribute__((always_inline))
static inline void gpio_ll_get_intr_status_high(gpio_dev_t *hw, uint32_t core_id, uint32_t *status)
{
    (void)core_id; // TODO: IDF-7995 There are 4 interrupt sources for GPIO on P4. New feature! Need to fix this function later.
    *status = hw->intr1_0.int1_0;
}

/**
  * @brief Clear GPIO interrupt status
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param mask interrupt status clear mask
  */
__attribute__((always_inline))
static inline void gpio_ll_clear_intr_status(gpio_dev_t *hw, uint32_t mask)
{
    hw->status_w1tc.status_w1tc = mask;
}

/**
  * @brief Clear GPIO interrupt status high
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param mask interrupt status high clear mask
  */
__attribute__((always_inline))
static inline void gpio_ll_clear_intr_status_high(gpio_dev_t *hw, uint32_t mask)
{
    hw->status1_w1tc.status1_w1tc = mask;
}

/**
 * @brief  Enable GPIO module interrupt signal
 *
 * @param  hw Peripheral GPIO hardware instance address.
 * @param  core_id Interrupt enabled CPU to corresponding ID
 * @param  gpio_num GPIO number. If you want to enable the interrupt of e.g. GPIO16, gpio_num should be GPIO_NUM_16 (16);
 */
__attribute__((always_inline))
static inline void gpio_ll_intr_enable_on_core(gpio_dev_t *hw, uint32_t core_id, uint32_t gpio_num)
{
    (void)core_id;
    hw->pin[gpio_num].int_ena = GPIO_LL_INTR0_ENA;     //enable for GPIO_INTR0 interrupt signal TODO: IDF-7995
}

/**
 * @brief  Disable GPIO module interrupt signal
 *
 * @param  hw Peripheral GPIO hardware instance address.
 * @param  gpio_num GPIO number. If you want to disable the interrupt of e.g. GPIO16, gpio_num should be GPIO_NUM_16 (16);
 */
__attribute__((always_inline))
static inline void gpio_ll_intr_disable(gpio_dev_t *hw, uint32_t gpio_num)
{
    hw->pin[gpio_num].int_ena = 0;                             //disable GPIO intr
}

/**
  * @brief Disable input mode on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
__attribute__((always_inline))
static inline void gpio_ll_input_disable(gpio_dev_t *hw, uint32_t gpio_num)
{
    IO_MUX.gpio[gpio_num].fun_ie = 0;
}

/**
  * @brief Enable input mode on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
__attribute__((always_inline))
static inline void gpio_ll_input_enable(gpio_dev_t *hw, uint32_t gpio_num)
{
    IO_MUX.gpio[gpio_num].fun_ie = 1;
}

/**
 * @brief Enable GPIO pin filter
 *
 * @param hw Peripheral GPIO hardware instance address.
 * @param gpio_num GPIO number of the pad.
 */
static inline void gpio_ll_pin_filter_enable(gpio_dev_t *hw, uint32_t gpio_num)
{
    IO_MUX.gpio[gpio_num].filter_en = 1;
}

/**
 * @brief Disable GPIO pin filter
 *
 * @param hw Peripheral GPIO hardware instance address.
 * @param gpio_num GPIO number of the pad.
 */
static inline void gpio_ll_pin_filter_disable(gpio_dev_t *hw, uint32_t gpio_num)
{
    IO_MUX.gpio[gpio_num].filter_en = 0;
}

/**
  * @brief Enable GPIO hysteresis
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_pin_input_hysteresis_enable(gpio_dev_t *hw, uint32_t gpio_num)
{
    uint64_t bit_mask = 1ULL << gpio_num;
    if (!(bit_mask & SOC_GPIO_VALID_DIGITAL_IO_PAD_MASK)) {
        // GPIO0-15
        LP_IOMUX.lp_pad_hys.reg_lp_gpio_hys |= bit_mask;
    } else {
        if (gpio_num < 32 + SOC_RTCIO_PIN_COUNT) {
            // GPIO 16-47
            HP_SYSTEM.gpio_o_hys_ctrl0.reg_gpio_0_hys_low |= (bit_mask >> SOC_RTCIO_PIN_COUNT);
        } else {
            // GPIO 48-54
            HP_SYSTEM.gpio_o_hys_ctrl1.reg_gpio_0_hys_high |= (bit_mask >> (32 + SOC_RTCIO_PIN_COUNT));
        }
    }
}

/**
  * @brief Disable GPIO hysteresis
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_pin_input_hysteresis_disable(gpio_dev_t *hw, uint32_t gpio_num)
{
    uint64_t bit_mask = 1ULL << gpio_num;
    if (!(bit_mask & SOC_GPIO_VALID_DIGITAL_IO_PAD_MASK)) {
        // GPIO0-15
        LP_IOMUX.lp_pad_hys.reg_lp_gpio_hys &= ~bit_mask;
    } else {
        if (gpio_num < 32 + SOC_RTCIO_PIN_COUNT) {
            // GPIO 16-47
            HP_SYSTEM.gpio_o_hys_ctrl0.reg_gpio_0_hys_low &= ~(bit_mask >> SOC_RTCIO_PIN_COUNT);
        } else {
            // GPIO 48-54
            HP_SYSTEM.gpio_o_hys_ctrl1.reg_gpio_0_hys_high &= ~(bit_mask >> (32 + SOC_RTCIO_PIN_COUNT));
        }
    }
}

/**
  * @brief Disable output mode on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
__attribute__((always_inline))
static inline void gpio_ll_output_disable(gpio_dev_t *hw, uint32_t gpio_num)
{
    if (gpio_num < 32) {
        hw->enable_w1tc.enable_w1tc = (0x1 << gpio_num);
    } else {
        hw->enable1_w1tc.enable1_w1tc = (0x1 << (gpio_num - 32));
    }
    // Ensure no other output signal is routed via GPIO matrix to this pin
    hw->func_out_sel_cfg[gpio_num].out_sel = SIG_GPIO_OUT_IDX;
}

/**
  * @brief Enable output mode on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
__attribute__((always_inline))
static inline void gpio_ll_output_enable(gpio_dev_t *hw, uint32_t gpio_num)
{
    if (gpio_num < 32) {
        hw->enable_w1ts.enable_w1ts = (0x1 << gpio_num);
    } else {
        hw->enable1_w1ts.enable1_w1ts = (0x1 << (gpio_num - 32));
    }
}

/**
  * @brief Disable open-drain mode on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_od_disable(gpio_dev_t *hw, uint32_t gpio_num)
{
    hw->pin[gpio_num].pad_driver = 0;
}

/**
  * @brief Enable open-drain mode on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_od_enable(gpio_dev_t *hw, uint32_t gpio_num)
{
    hw->pin[gpio_num].pad_driver = 1;
}

/**
 * @brief  GPIO set output level
 *
 * @param  hw Peripheral GPIO hardware instance address.
 * @param  gpio_num GPIO number. If you want to set the output level of e.g. GPIO16, gpio_num should be GPIO_NUM_16 (16);
 * @param  level Output level. 0: low ; 1: high
 */
__attribute__((always_inline))
static inline void gpio_ll_set_level(gpio_dev_t *hw, uint32_t gpio_num, uint32_t level)
{
    if (level) {
        if (gpio_num < 32) {
            hw->out_w1ts.out_w1ts = (1 << gpio_num);
        } else {
            hw->out1_w1ts.out1_w1ts = (1 << (gpio_num - 32));
        }
    } else {
        if (gpio_num < 32) {
            hw->out_w1tc.out_w1tc = (1 << gpio_num);
        } else {
            hw->out1_w1tc.out1_w1tc = (1 << (gpio_num - 32));
        }
    }
}

/**
 * @brief  GPIO get input level
 *
 * @warning If the pad is not configured for input (or input and output) the returned value is always 0.
 *
 * @param  hw Peripheral GPIO hardware instance address.
 * @param  gpio_num GPIO number. If you want to get the logic level of e.g. pin GPIO16, gpio_num should be GPIO_NUM_16 (16);
 *
 * @return
 *     - 0 the GPIO input level is 0
 *     - 1 the GPIO input level is 1
 */
__attribute__((always_inline))
static inline int gpio_ll_get_level(gpio_dev_t *hw, uint32_t gpio_num)
{
    if (gpio_num < 32) {
        return (hw->in.in_data_next >> gpio_num) & 0x1;
    } else {
        return (hw->in1.in1_data_next >> (gpio_num - 32)) & 0x1;
    }
}

/**
 * @brief Enable GPIO wake-up function.
 *
 * @param hw Peripheral GPIO hardware instance address.
 * @param gpio_num GPIO number.
 */
static inline void gpio_ll_wakeup_enable(gpio_dev_t *hw, uint32_t gpio_num)
{
    hw->pin[gpio_num].wakeup_enable = 1;
}

/**
 * @brief Disable GPIO wake-up function.
 *
 * @param hw Peripheral GPIO hardware instance address.
 * @param gpio_num GPIO number
 */
static inline void gpio_ll_wakeup_disable(gpio_dev_t *hw, uint32_t gpio_num)
{
    hw->pin[gpio_num].wakeup_enable = 0;
}

/**
  * @brief Set GPIO pad drive capability
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number, only support output GPIOs
  * @param strength Drive capability of the pad
  */
static inline void gpio_ll_set_drive_capability(gpio_dev_t *hw, uint32_t gpio_num, gpio_drive_cap_t strength)
{
    IO_MUX.gpio[gpio_num].fun_drv = strength;
}

/**
  * @brief Get GPIO pad drive capability
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number, only support output GPIOs
  * @param strength Pointer to accept drive capability of the pad
  */
static inline void gpio_ll_get_drive_capability(gpio_dev_t *hw, uint32_t gpio_num, gpio_drive_cap_t *strength)
{
    *strength = (gpio_drive_cap_t)(IO_MUX.gpio[gpio_num].fun_drv);
}

// On ESP32P4, all digital GPIO pads can be held together during Deep-sleep through PMU.hp_sys[PMU_MODE_HP_SLEEP].syscntl.hp_pad_hold_all = 1
// However, since the hold register for digital IOs is in TOP domain (HP_SYSTEM.gpio_o_hold_ctrlx), it gets powered down in Deep-sleep.
// Therefore, after waking up from Deep-sleep, the register has been reset, it is not able to hold at that time.
// In all, the users can not achieve the purpose of being hold all the time. So this feature is considered not usable on P4.

/**
  * @brief Enable gpio pad hold function.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number, only support output GPIOs
  */
__attribute__((always_inline))
static inline void gpio_ll_hold_en(gpio_dev_t *hw, uint32_t gpio_num)
{
    uint64_t bit_mask = 1ULL << gpio_num;
    if (!(bit_mask & SOC_GPIO_VALID_DIGITAL_IO_PAD_MASK)) {
        // GPIO 0-15
        uint32_t hold_mask = HAL_FORCE_READ_U32_REG_FIELD(LP_IOMUX.lp_pad_hold, reg_lp_gpio_hold);
        hold_mask |= bit_mask;
        HAL_FORCE_MODIFY_U32_REG_FIELD(LP_IOMUX.lp_pad_hold, reg_lp_gpio_hold, hold_mask);
    } else {
        if (gpio_num < 32 + SOC_RTCIO_PIN_COUNT) {
            // GPIO 16-47
            HP_SYSTEM.gpio_o_hold_ctrl0.reg_gpio_0_hold_low |= (bit_mask >> SOC_RTCIO_PIN_COUNT);
        } else {
            // GPIO 48-54
            HP_SYSTEM.gpio_o_hold_ctrl1.reg_gpio_0_hold_high |= (bit_mask >> (32 + SOC_RTCIO_PIN_COUNT));
        }
    }
}

/**
  * @brief Disable gpio pad hold function.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number, only support output GPIOs
  */
__attribute__((always_inline))
static inline void gpio_ll_hold_dis(gpio_dev_t *hw, uint32_t gpio_num)
{
    uint64_t bit_mask = 1ULL << gpio_num;
    if (!(bit_mask & SOC_GPIO_VALID_DIGITAL_IO_PAD_MASK)) {
        // GPIO 0-15
        uint32_t hold_mask = HAL_FORCE_READ_U32_REG_FIELD(LP_IOMUX.lp_pad_hold, reg_lp_gpio_hold);
        hold_mask &= ~bit_mask;
        HAL_FORCE_MODIFY_U32_REG_FIELD(LP_IOMUX.lp_pad_hold, reg_lp_gpio_hold, hold_mask);
    } else {
        if (gpio_num < 32 + SOC_RTCIO_PIN_COUNT) {
            // GPIO 16-47
            HP_SYSTEM.gpio_o_hold_ctrl0.reg_gpio_0_hold_low &= ~(bit_mask >> SOC_RTCIO_PIN_COUNT);
        } else {
            // GPIO 48-54
            HP_SYSTEM.gpio_o_hold_ctrl1.reg_gpio_0_hold_high &= ~(bit_mask >> (32 + SOC_RTCIO_PIN_COUNT));
        }
    }
}

/**
  * @brief Get digital gpio pad hold status.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number, only support output GPIOs
  *
  * @note caller must ensure that gpio_num is a digital io pad
  *
  * @return
  *     - true  digital gpio pad is held
  *     - false digital gpio pad is unheld
  */
__attribute__((always_inline))
static inline bool gpio_ll_is_digital_io_hold(gpio_dev_t *hw, uint32_t gpio_num)
{
    uint64_t bit_mask = 1ULL << gpio_num;
    if (!(bit_mask & SOC_GPIO_VALID_DIGITAL_IO_PAD_MASK)) {
        // GPIO 0-15
        abort();
    } else {
        if (gpio_num < 32 + SOC_RTCIO_PIN_COUNT) {
            // GPIO 16-47
            return !!(HP_SYSTEM.gpio_o_hold_ctrl0.reg_gpio_0_hold_low & (bit_mask >> SOC_RTCIO_PIN_COUNT));
        } else {
            // GPIO 48-54
            return !!(HP_SYSTEM.gpio_o_hold_ctrl1.reg_gpio_0_hold_high & (bit_mask >> (32 + SOC_RTCIO_PIN_COUNT)));
        }
    }
}

/**
  * @brief Set pad input to a peripheral signal through the IO_MUX.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number of the pad.
  * @param signal_idx Peripheral signal id to input. One of the ``*_IN_IDX`` signals in ``soc/gpio_sig_map.h``.
  */
__attribute__((always_inline))
static inline void gpio_ll_iomux_in(gpio_dev_t *hw, uint32_t gpio, uint32_t signal_idx)
{
    hw->func_in_sel_cfg[signal_idx].sig_in_sel = 0;
    IO_MUX.gpio[gpio].fun_ie = 1;
}

/**
 * @brief  Select a function for the pin in the IOMUX
 *
 * @param  pin_name Pin name to configure
 * @param  func Function to assign to the pin
 */
static inline void gpio_ll_iomux_func_sel(uint32_t pin_name, uint32_t func)
{
    // Disable USB PHY configuration if pins (24, 25) (26, 27) needs to select an IOMUX function
    // P4 has two internal PHYs connecting to USJ and USB_WRAP(OTG1.1) separately.
    // We only consider the default connection here: PHY0 -> USJ, PHY1 -> USB_OTG
    if (pin_name == IO_MUX_GPIO24_REG || pin_name == IO_MUX_GPIO25_REG) {
        USB_SERIAL_JTAG.conf0.usb_pad_enable = 0;
    } else if (pin_name == IO_MUX_GPIO26_REG || pin_name == IO_MUX_GPIO27_REG) {
        USB_WRAP.otg_conf.usb_pad_enable = 0;
    }
    PIN_FUNC_SELECT(pin_name, func);
}

/**
 * @brief  Select a function for the pin in the IOMUX
 *
 * @param  hw Peripheral GPIO hardware instance address.
 * @param  gpio_num GPIO number
 * @param  func Function to assign to the pin
 */
__attribute__((always_inline))
static inline void gpio_ll_func_sel(gpio_dev_t *hw, uint8_t gpio_num, uint32_t func)
{
    // Disable USB PHY configuration if pins (24, 25) (26, 27) needs to select an IOMUX function
    // P4 has two internal PHYs connecting to USJ and USB_WRAP(OTG1.1) separately.
    // We only consider the default connection here: PHY0 -> USJ, PHY1 -> USB_OTG
    if (gpio_num == USB_USJ_INT_PHY_DM_GPIO_NUM || gpio_num == USB_USJ_INT_PHY_DP_GPIO_NUM) {
        USB_SERIAL_JTAG.conf0.usb_pad_enable = 0;
    } else if (gpio_num == USB_OTG_INT_PHY_DM_GPIO_NUM || gpio_num == USB_OTG_INT_PHY_DP_GPIO_NUM) {
        USB_WRAP.otg_conf.usb_pad_enable = 0;
    }
    IO_MUX.gpio[gpio_num].mcu_sel = func;
}

/**
  * @brief Set peripheral output to an GPIO pad through the IO_MUX.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num gpio_num GPIO number of the pad.
  * @param func The function number of the peripheral pin to output pin.
  *        One of the ``FUNC_X_*`` of specified pin (X) in ``soc/io_mux_reg.h``.
  * @param oen_inv True if the output enable needs to be inverted, otherwise False.
  */
static inline void gpio_ll_iomux_out(gpio_dev_t *hw, uint8_t gpio_num, int func, uint32_t oen_inv)
{
    hw->func_out_sel_cfg[gpio_num].oen_sel = 0;
    hw->func_out_sel_cfg[gpio_num].oen_inv_sel = oen_inv;
    gpio_ll_func_sel(hw, gpio_num, func);
}

/**
 * @brief Set clock source of IO MUX module
 *
 * @param src IO MUX clock source (only a subset of soc_module_clk_t values are valid)
 */
static inline void gpio_ll_iomux_set_clk_src(soc_module_clk_t src)
{
    switch (src) {
    case SOC_MOD_CLK_XTAL:
        HP_SYS_CLKRST.peri_clk_ctrl26.reg_iomux_clk_src_sel = 0;
        break;
    case SOC_MOD_CLK_PLL_F80M:
        HP_SYS_CLKRST.peri_clk_ctrl26.reg_iomux_clk_src_sel = 1;
        break;
    default:
        // Unsupported IO_MUX clock source
        HAL_ASSERT(false);
    }
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define gpio_ll_iomux_set_clk_src(...) (void)__DECLARE_RCC_ATOMIC_ENV; gpio_ll_iomux_set_clk_src(__VA_ARGS__)

/**
 * @brief Get the GPIO number that is routed to the input peripheral signal through GPIO matrix.
 *
 * @param hw Peripheral GPIO hardware instance address.
 * @param in_sig_idx Peripheral signal index (tagged as input attribute).
 *
 * @return
 *    - -1     Signal bypassed GPIO matrix
 *    - Others GPIO number
 */
static inline int gpio_ll_get_in_signal_connected_io(gpio_dev_t *hw, uint32_t in_sig_idx)
{
    gpio_func_in_sel_cfg_reg_t reg;
    reg.val = hw->func_in_sel_cfg[in_sig_idx].val;
    return (reg.sig_in_sel ? reg.in_sel : -1);
}

/**
  * @brief Force hold digital io pad.
  * @note GPIO force hold, whether the chip in sleep mode or wakeup mode.
  */
static inline void gpio_ll_force_hold_all(void)
{
    // WT flag, it gets self-cleared after the configuration is done
    PMU.imm.pad_hold_all.tie_high_hp_pad_hold_all = 1;
}

/**
  * @brief Force unhold digital io pad.
  * @note GPIO force unhold, whether the chip in sleep mode or wakeup mode.
  */
static inline void gpio_ll_force_unhold_all(void)
{
    // WT flag, it gets self-cleared after the configuration is done
    PMU.imm.pad_hold_all.tie_low_hp_pad_hold_all = 1;
}

/**
  * @brief Enable GPIO pin to use sleep mode pin functions during light sleep.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_sel_en(gpio_dev_t *hw, uint32_t gpio_num)
{
    IO_MUX.gpio[gpio_num].slp_sel = 1;
}

/**
  * @brief Disable GPIO pin to use sleep mode pin functions during light sleep.
  * Pin functions remains the same in both normal execution and in light-sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_sel_dis(gpio_dev_t *hw, uint32_t gpio_num)
{
    IO_MUX.gpio[gpio_num].slp_sel = 0;
}

/**
  * @brief Disable GPIO pull-up in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_pullup_dis(gpio_dev_t *hw, uint32_t gpio_num)
{
    IO_MUX.gpio[gpio_num].mcu_wpu = 0;
}

/**
  * @brief Enable GPIO pull-up in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_pullup_en(gpio_dev_t *hw, uint32_t gpio_num)
{
    IO_MUX.gpio[gpio_num].mcu_wpu = 1;
}

/**
  * @brief Enable GPIO pull-down in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_pulldown_en(gpio_dev_t *hw, uint32_t gpio_num)
{
    IO_MUX.gpio[gpio_num].mcu_wpd = 1;
}

/**
  * @brief Disable GPIO pull-down in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_pulldown_dis(gpio_dev_t *hw, uint32_t gpio_num)
{
    IO_MUX.gpio[gpio_num].mcu_wpd = 0;
}

/**
  * @brief Disable GPIO input in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_input_disable(gpio_dev_t *hw, uint32_t gpio_num)
{
    IO_MUX.gpio[gpio_num].mcu_ie = 0;
}

/**
  * @brief Enable GPIO input in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_input_enable(gpio_dev_t *hw, uint32_t gpio_num)
{
    IO_MUX.gpio[gpio_num].mcu_ie = 1;
}

/**
  * @brief Disable GPIO output in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_output_disable(gpio_dev_t *hw, uint32_t gpio_num)
{
    IO_MUX.gpio[gpio_num].mcu_oe = 0;
}

/**
  * @brief Enable GPIO output in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_output_enable(gpio_dev_t *hw, uint32_t gpio_num)
{
    IO_MUX.gpio[gpio_num].mcu_oe = 1;
}

#ifdef __cplusplus
}
#endif
