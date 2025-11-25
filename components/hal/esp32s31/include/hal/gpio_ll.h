/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
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
#include "soc/io_mux_reg.h"
#include "soc/io_mux_struct.h"
#include "soc/hp_system_struct.h"
#include "soc/lp_system_struct.h"
#include "soc/lp_iomux_struct.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "soc/hp_system_reg.h"
#include "soc/lp_system_reg.h"
#include "soc/pmu_struct.h"
#include "soc/usb_serial_jtag_struct.h"
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

#define GPIO_LL_INTR_SOURCE0   ETS_GPIO_INTR0_SOURCE

/**
 * @brief Get the configuration for an IO
 *
 * @param hw Peripheral GPIO hardware instance address.
 * @param gpio_num GPIO number
 * @param[out] io_config Pointer to the structure that saves the specific IO configuration
 */
static inline void gpio_ll_get_io_config(gpio_dev_t *hw, uint32_t gpio_num, gpio_io_config_t *io_config)
{
    // TODO: [ESP32S31] IDF-14780
}

/**
  * @brief Enable pull-up on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_pullup_en(gpio_dev_t *hw, uint32_t gpio_num)
{
    // TODO: [ESP32S31] IDF-14780
}

static inline void gpio_ll_set_output_signal_matrix_source(gpio_dev_t *hw, uint32_t gpio_num, uint32_t signal_idx, bool out_inv)
{
    // TODO: [ESP32S31] IDF-14780
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
    // TODO: [ESP32S31] IDF-14780
}

/**
  * @brief Enable pull-down on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_pulldown_en(gpio_dev_t *hw, uint32_t gpio_num)
{
    // TODO: [ESP32S31] IDF-14780
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
    // TODO: [ESP32S31] IDF-14780
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
    *status = hw->intr_01.int_01;
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
    // hw->pin[gpio_num].int_ena = 0;                             //disable GPIO intr
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
    // IO_MUX.gpio[gpio_num].fun_ie = 0;
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
    // IO_MUX.gpio[gpio_num].fun_ie = 1;
}

/**
 * @brief Enable GPIO pin filter
 *
 * @param hw Peripheral GPIO hardware instance address.
 * @param gpio_num GPIO number of the pad.
 */
static inline void gpio_ll_pin_filter_enable(gpio_dev_t *hw, uint32_t gpio_num)
{
    // IO_MUX.gpio[gpio_num].filter_en = 1;
}

/**
 * @brief Disable GPIO pin filter
 *
 * @param hw Peripheral GPIO hardware instance address.
 * @param gpio_num GPIO number of the pad.
 */
static inline void gpio_ll_pin_filter_disable(gpio_dev_t *hw, uint32_t gpio_num)
{
    // IO_MUX.gpio[gpio_num].filter_en = 0;
}

/**
  * @brief Enable GPIO hysteresis
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_pin_input_hysteresis_enable(gpio_dev_t *hw, uint32_t gpio_num)
{
    // TODO: [ESP32S31] IDF-14780
}

/**
  * @brief Disable GPIO hysteresis
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_pin_input_hysteresis_disable(gpio_dev_t *hw, uint32_t gpio_num)
{
    // TODO: [ESP32S31] IDF-14780
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
 * @brief Disconnect any peripheral output signal routed via GPIO matrix to the pin
 *
 * @param  hw Peripheral GPIO hardware instance address.
 * @param  gpio_num GPIO number
 */
__attribute__((always_inline))
static inline void gpio_ll_matrix_out_default(gpio_dev_t *hw, uint32_t gpio_num)
{
    gpio_func_out_sel_cfg_reg_t reg = {
      .out_sel = SIG_GPIO_OUT_IDX,
    };
    hw->func_out_sel_cfg[gpio_num].val = reg.val;
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
            hw->out_w1ts.val = 1 << gpio_num;
        } else {
            hw->out1_w1ts.val = 1 << (gpio_num - 32);
        }
    } else {
        if (gpio_num < 32) {
            hw->out_w1tc.val = 1 << gpio_num;
        } else {
            hw->out1_w1tc.val = 1 << (gpio_num - 32);
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
    // TODO: [ESP32S31] IDF-14780
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
    // TODO: [ESP32S31] IDF-14780
}

// On ESP32S31, all digital GPIO pads can be held together during Deep-sleep through PMU.hp_sys[PMU_MODE_HP_SLEEP].syscntl.hp_pad_hold_all = 1
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
  if(gpio_num < 8){
    REG_SET_BIT(LP_SYSTEM_REG_PADCTRL_REG, (0x1 << (gpio_num + 8)));
  }
  else if(gpio_num < 40){
    REG_SET_BIT(LP_SYSTEM_REG_HP_GPIO_O_HOLD_CTRL0_REG, (0x1 << (gpio_num - 8)));
  }
  else{
    REG_SET_BIT(LP_SYSTEM_REG_HP_GPIO_O_HOLD_CTRL1_REG, (0x1 << (gpio_num - 40)));
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
  if(gpio_num < 8){
    REG_CLR_BIT(LP_SYSTEM_REG_PADCTRL_REG, (0x1 << (gpio_num + 8)));
  }
  else if(gpio_num < 40){
    REG_CLR_BIT(LP_SYSTEM_REG_HP_GPIO_O_HOLD_CTRL0_REG, (0x1 << (gpio_num - 8)));
  }
  else{
    REG_CLR_BIT(LP_SYSTEM_REG_HP_GPIO_O_HOLD_CTRL1_REG, (0x1 << (gpio_num - 40)));
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
        // if (gpio_num < 32 + SOC_RTCIO_PIN_COUNT) {
        //     // GPIO 16-47
        //     return !!(LP_SYSTEM.gpio_o_hold_ctrl0.reg_gpio_0_hold_ctrl0 & (bit_mask >> SOC_RTCIO_PIN_COUNT));
        // }
    }
}

/**
  * @brief Configure peripheral signal input whether to bypass GPIO matrix.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param signal_idx Peripheral signal id to input. One of the ``*_IN_IDX`` signals in ``soc/gpio_sig_map.h``.
  * @param from_gpio_matrix True if not to bypass GPIO matrix, otherwise False.
  */
__attribute__((always_inline))
static inline void gpio_ll_set_input_signal_from(gpio_dev_t *hw, uint32_t signal_idx, bool from_gpio_matrix)
{
    hw->func_in_sel_cfg[signal_idx].sig_in_sel = from_gpio_matrix;
}

/**
  * @brief Configure the source of output enable signal for the GPIO pin.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number of the pad.
  * @param ctrl_by_periph True if use output enable signal from peripheral, false if force the output enable signal to be sourced from bit n of GPIO_ENABLE_REG
  * @param oen_inv True if the output enable needs to be inverted, otherwise False.
  */
static inline void gpio_ll_set_output_enable_ctrl(gpio_dev_t *hw, uint8_t gpio_num, bool ctrl_by_periph, bool oen_inv)
{
    hw->func_out_sel_cfg[gpio_num].oen_inv_sel = oen_inv;       // control valid only when using gpio matrix to route signal to the IO
    hw->func_out_sel_cfg[gpio_num].oen_sel = !ctrl_by_periph;
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
    // TODO: [ESP32S31] IDF-14780
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
        HP_SYS_CLKRST.iomux_ctrl0.reg_iomux_clk_src_sel = 0;
        break;
    case SOC_MOD_CLK_PLL_F80M:
        HP_SYS_CLKRST.iomux_ctrl0.reg_iomux_clk_src_sel = 1;
        break;
    default:
        // Unsupported IO_MUX clock source
        HAL_ASSERT(false);
    }
}


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
    // PMU.imm.pad_hold_all.tie_high_hp_pad_hold_all = 1;
}

/**
  * @brief Force unhold digital io pad.
  * @note GPIO force unhold, whether the chip in sleep mode or wakeup mode.
  */
static inline void gpio_ll_force_unhold_all(void)
{
    // WT flag, it gets self-cleared after the configuration is done
    // PMU.imm.pad_hold_all.tie_low_hp_pad_hold_all = 1;
}

/**
  * @brief Enable GPIO pin to use sleep mode pin functions during light sleep.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
__attribute__((always_inline))
static inline void gpio_ll_sleep_sel_en(gpio_dev_t *hw, uint32_t gpio_num)
{
    // TODO: [ESP32S31] IDF-14780
}

/**
  * @brief Disable GPIO pin to use sleep mode pin functions during light sleep.
  * Pin functions remains the same in both normal execution and in light-sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
__attribute__((always_inline))
static inline void gpio_ll_sleep_sel_dis(gpio_dev_t *hw, uint32_t gpio_num)
{
    // TODO: [ESP32S31] IDF-14780
}

/**
  * @brief Disable GPIO pull-up in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
__attribute__((always_inline))
static inline void gpio_ll_sleep_pullup_dis(gpio_dev_t *hw, uint32_t gpio_num)
{
    // TODO: [ESP32S31] IDF-14780
}

/**
  * @brief Enable GPIO pull-up in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
__attribute__((always_inline))
static inline void gpio_ll_sleep_pullup_en(gpio_dev_t *hw, uint32_t gpio_num)
{
    // TODO: [ESP32S31] IDF-14780
}

/**
  * @brief Enable GPIO pull-down in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
__attribute__((always_inline))
static inline void gpio_ll_sleep_pulldown_en(gpio_dev_t *hw, uint32_t gpio_num)
{
    // TODO: [ESP32S31] IDF-14780
}

/**
  * @brief Disable GPIO pull-down in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
__attribute__((always_inline))
static inline void gpio_ll_sleep_pulldown_dis(gpio_dev_t *hw, uint32_t gpio_num)
{
    // TODO: [ESP32S31] IDF-14780
}

/**
  * @brief Disable GPIO input in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
__attribute__((always_inline))
static inline void gpio_ll_sleep_input_disable(gpio_dev_t *hw, uint32_t gpio_num)
{
    // TODO: [ESP32S31] IDF-14780
}

/**
  * @brief Enable GPIO input in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
__attribute__((always_inline))
static inline void gpio_ll_sleep_input_enable(gpio_dev_t *hw, uint32_t gpio_num)
{
    // TODO: [ESP32S31] IDF-14780
}

/**
  * @brief Disable GPIO output in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
__attribute__((always_inline))
static inline void gpio_ll_sleep_output_disable(gpio_dev_t *hw, uint32_t gpio_num)
{
    // TODO: [ESP32S31] IDF-14780
}

/**
  * @brief Enable GPIO output in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
__attribute__((always_inline))
static inline void gpio_ll_sleep_output_enable(gpio_dev_t *hw, uint32_t gpio_num)
{
    // TODO: [ESP32S31] IDF-14780
}


#ifdef __cplusplus
}
#endif
