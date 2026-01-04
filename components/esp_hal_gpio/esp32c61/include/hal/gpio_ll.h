/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The LL layer for ESP32-C61 GPIO register operations

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "soc/soc.h"
#include "soc/gpio_struct.h"
#include "soc/lp_aon_struct.h"
#include "soc/pmu_struct.h"
#include "soc/pcr_struct.h"
#include "soc/clk_tree_defs.h"
#include "soc/usb_serial_jtag_struct.h"
#include "soc/io_mux_struct.h"
#include "soc/io_mux_reg.h"
#include "soc/interrupts.h"
#include "soc/gpio_periph.h"
#include "hal/gpio_types.h"
#include "hal/assert.h"
#include "hal/config.h"
#include "rom/gpio.h"

// Get GPIO hardware instance with giving gpio num
#define GPIO_LL_GET_HW(num) (((num) == 0) ? (&GPIO) : NULL)

#define GPIO_LL_PRO_CPU_INTR_ENA      (BIT(0))
#define GPIO_LL_PRO_CPU_NMI_INTR_ENA  (BIT(1))

#define GPIO_LL_INTR_SOURCE0   ETS_GPIO_INTERRUPT_PRO_SOURCE

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get the configuration for an IO
 *
 * @param hw Peripheral GPIO hardware instance address.
 * @param gpio_num GPIO number
 * @param[out] io_config Pointer to the structure that saves the specific IO configuration
 */
static inline void gpio_ll_get_io_config(gpio_dev_t *hw, uint32_t gpio_num, gpio_io_config_t *io_config)
{
    io_config->pu = IO_MUX.gpion[gpio_num].gpion_fun_wpu;
    io_config->pd = IO_MUX.gpion[gpio_num].gpion_fun_wpd;
    io_config->ie = IO_MUX.gpion[gpio_num].gpion_fun_ie;
    io_config->oe = (hw->enable.val & (1 << gpio_num)) >> gpio_num;
    io_config->oe_ctrl_by_periph = !(hw->funcn_out_sel_cfg[gpio_num].funcn_oe_sel);
    io_config->oe_inv = hw->funcn_out_sel_cfg[gpio_num].funcn_oe_inv_sel;
    io_config->od = hw->pinn[gpio_num].pinn_pad_driver;
    io_config->drv = (gpio_drive_cap_t)IO_MUX.gpion[gpio_num].gpion_fun_drv;
    io_config->fun_sel = IO_MUX.gpion[gpio_num].gpion_mcu_sel;
    io_config->sig_out = hw->funcn_out_sel_cfg[gpio_num].funcn_out_sel;
    io_config->slp_sel = IO_MUX.gpion[gpio_num].gpion_slp_sel;
}

/**
  * @brief Enable pull-up on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_pullup_en(gpio_dev_t *hw, uint32_t gpio_num)
{
    IO_MUX.gpion[gpio_num].gpion_fun_wpu = 1;
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
    // Note that esp32C61 has supported USB_EXCHG_PINS feature. If this efuse is burnt, the gpio pin
    // which should be checked is USB_INT_PHY0_DM_GPIO_NUM instead.
    // TODO: read the specific efuse with efuse_ll.h
    if (gpio_num == USB_INT_PHY0_DP_GPIO_NUM) {
        USB_SERIAL_JTAG.conf0.exchg_pins_override = 1;
        USB_SERIAL_JTAG.conf0.dp_pullup = 0;
    }
    IO_MUX.gpion[gpio_num].gpion_fun_wpu = 0;
}

/**
  * @brief Enable pull-down on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_pulldown_en(gpio_dev_t *hw, uint32_t gpio_num)
{
    IO_MUX.gpion[gpio_num].gpion_fun_wpd = 1;
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

    IO_MUX.gpion[gpio_num].gpion_fun_wpd = 0;
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
    hw->pinn[gpio_num].pinn_int_type = intr_type;
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
    (void)core_id;
    *status = hw->procpu_int.procpu_int;
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
    *status = 0; // Less than 32 GPIOs in ESP32-C61
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
    // Less than 32 GPIOs on ESP32-C61 Do nothing.
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
    HAL_ASSERT(core_id == 0 && "target SoC only has a single core");
    GPIO.pinn[gpio_num].pinn_int_ena = GPIO_LL_PRO_CPU_INTR_ENA;     //enable pro cpu intr
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
    hw->pinn[gpio_num].pinn_int_ena = 0;                             //disable GPIO intr
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
    IO_MUX.gpion[gpio_num].gpion_fun_ie = 0;
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
    IO_MUX.gpion[gpio_num].gpion_fun_ie = 1;
}

/**
 * @brief Enable GPIO pin filter
 *
 * @param hw Peripheral GPIO hardware instance address.
 * @param gpio_num GPIO number of the pad.
 */
static inline void gpio_ll_pin_filter_enable(gpio_dev_t *hw, uint32_t gpio_num)
{
    IO_MUX.gpion[gpio_num].gpion_filter_en = 1;
}

/**
 * @brief Disable GPIO pin filter
 *
 * @param hw Peripheral GPIO hardware instance address.
 * @param gpio_num GPIO number of the pad.
 */
static inline void gpio_ll_pin_filter_disable(gpio_dev_t *hw, uint32_t gpio_num)
{
    IO_MUX.gpion[gpio_num].gpion_filter_en = 0;
}

/**
  * @brief Enable GPIO hysteresis
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_pin_input_hysteresis_enable(gpio_dev_t *hw, uint32_t gpio_num)
{
    // Always set hys_sel to 1 to use software control hysteresis, since no efuse bit reserved for hysteresis on ESP32C61
    IO_MUX.gpion[gpio_num].gpion_hys_sel = 1;
    IO_MUX.gpion[gpio_num].gpion_hys_en = 1;
}

/**
  * @brief Disable GPIO hysteresis
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_pin_input_hysteresis_disable(gpio_dev_t *hw, uint32_t gpio_num)
{
    IO_MUX.gpion[gpio_num].gpion_hys_sel = 1;
    IO_MUX.gpion[gpio_num].gpion_hys_en = 0;
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
    hw->enable_w1tc.enable_w1tc = (0x1 << gpio_num);
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
    hw->enable_w1ts.enable_w1ts = (0x1 << gpio_num);
}

/**
  * @brief Disable open-drain mode on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_od_disable(gpio_dev_t *hw, uint32_t gpio_num)
{
    hw->pinn[gpio_num].pinn_pad_driver = 0;
}

/**
  * @brief Enable open-drain mode on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_od_enable(gpio_dev_t *hw, uint32_t gpio_num)
{
    hw->pinn[gpio_num].pinn_pad_driver = 1;
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
#if HAL_CONFIG(GPIO_USE_ROM_API)
    rom_gpio_set_output_level(gpio_num, level);
#else
    if (level) {
        hw->out_w1ts.val = 1 << gpio_num;
    } else {
        hw->out_w1tc.val = 1 << gpio_num;
    }
#endif
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
#if HAL_CONFIG(GPIO_USE_ROM_API)
    return rom_gpio_get_input_level(gpio_num);
#else
    return (hw->in.in_data_next >> gpio_num) & 0x1;
#endif
}

/**
 * @brief Enable GPIO wake-up function.
 *
 * @param hw Peripheral GPIO hardware instance address.
 * @param gpio_num GPIO number.
 */
static inline void gpio_ll_wakeup_enable(gpio_dev_t *hw, uint32_t gpio_num)
{
    hw->pinn[gpio_num].pinn_wakeup_enable = 1;
}

/**
 * @brief Disable GPIO wake-up function.
 *
 * @param hw Peripheral GPIO hardware instance address.
 * @param gpio_num GPIO number
 */
static inline void gpio_ll_wakeup_disable(gpio_dev_t *hw, uint32_t gpio_num)
{
    hw->pinn[gpio_num].pinn_wakeup_enable = 0;
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
    IO_MUX.gpion[gpio_num].gpion_fun_drv = strength;
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
    *strength = (gpio_drive_cap_t)(IO_MUX.gpion[gpio_num].gpion_fun_drv);
}

/**
  * @brief Enable gpio pad hold function.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number, only support output GPIOs
  */
__attribute__((always_inline))
static inline void gpio_ll_hold_en(gpio_dev_t *hw, uint32_t gpio_num)
{
    uint32_t mask = LP_AON.gpio_hold0.gpio_hold0;
    LP_AON.gpio_hold0.gpio_hold0 = mask | GPIO_HOLD_MASK[gpio_num];
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
    uint32_t mask = LP_AON.gpio_hold0.gpio_hold0;
    LP_AON.gpio_hold0.gpio_hold0 = mask & (~GPIO_HOLD_MASK[gpio_num]);
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
    return !!(LP_AON.gpio_hold0.gpio_hold0 & BIT(gpio_num));
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
 * @brief Connect a GPIO input with a peripheral signal, which tagged as input attribute.
 *
 * @note There's no limitation on the number of signals that a GPIO can combine with.
 *
 * @param signal_idx Peripheral signal index (tagged as input attribute)
 * @param gpio_num GPIO number, especially, `GPIO_MATRIX_CONST_ZERO_INPUT` means connect logic 0 to signal
 *                                          `GPIO_MATRIX_CONST_ONE_INPUT` means connect logic 1 to signal
 * @param in_inv True if the GPIO input needs to be inverted, otherwise False.
 */
static inline void gpio_ll_set_input_signal_matrix_source(gpio_dev_t *hw, uint32_t signal_idx, uint32_t gpio_num, bool in_inv)
{
    hw->func_in_sel_cfg[signal_idx].func_in_sel = gpio_num;
    hw->func_in_sel_cfg[signal_idx].func_in_inv_sel = in_inv;
    gpio_ll_set_input_signal_from(hw, signal_idx, true);
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
    return (reg.sig_in_sel ? reg.func_in_sel : -1);
}

/**
  * @brief Configure the source of output enable signal for the pad (only takes effect if func sel is selected to be GPIO).
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number of the pad.
  * @param ctrl_by_periph True if use output enable signal from peripheral, false if force the output enable signal to be sourced from bit n of GPIO_ENABLE_REG
  * @param oen_inv True if the output enable needs to be inverted, otherwise False.
  */
static inline void gpio_ll_set_output_enable_ctrl(gpio_dev_t *hw, uint8_t gpio_num, bool ctrl_by_periph, bool oen_inv)
{
    hw->funcn_out_sel_cfg[gpio_num].funcn_oe_inv_sel = oen_inv;       // control valid only when using gpio matrix to route signal to the IO
    hw->funcn_out_sel_cfg[gpio_num].funcn_oe_sel = !ctrl_by_periph;
}

/**
 * @brief Connect a peripheral signal which tagged as output attribute with a GPIO.
 *
 * @note There's no limitation on the number of signals that a GPIO can combine with.
 *
 * @param gpio_num GPIO number
 * @param signal_idx Peripheral signal index (tagged as output attribute). Particularly, `SIG_GPIO_OUT_IDX` means disconnect GPIO and other peripherals. Only the GPIO driver can control the output level.
 * @param out_inv True if the signal output needs to be inverted, otherwise False.
 */
static inline void gpio_ll_set_output_signal_matrix_source(gpio_dev_t *hw, uint32_t gpio_num, uint32_t signal_idx, bool out_inv)
{
    hw->funcn_out_sel_cfg[gpio_num].funcn_out_sel = signal_idx;
    hw->funcn_out_sel_cfg[gpio_num].funcn_out_inv_sel = out_inv;
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
    // Disable USB Serial JTAG if pins 12 or pins 13 needs to select an IOMUX function
    if (gpio_num == USB_INT_PHY0_DM_GPIO_NUM || gpio_num == USB_INT_PHY0_DP_GPIO_NUM) {
        USB_SERIAL_JTAG.conf0.usb_pad_enable = 0;
    }
    IO_MUX.gpion[gpio_num].gpion_mcu_sel = func;
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
        PCR.iomux_clk_conf.iomux_func_clk_sel = 0;
        break;
    case SOC_MOD_CLK_RC_FAST:
        PCR.iomux_clk_conf.iomux_func_clk_sel = 1;
        break;
    case SOC_MOD_CLK_PLL_F80M:
        PCR.iomux_clk_conf.iomux_func_clk_sel = 2;
        break;
    default:
        // Unsupported IO_MUX clock source
        HAL_ASSERT(false);
    }
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
__attribute__((always_inline))
static inline void gpio_ll_sleep_sel_en(gpio_dev_t *hw, uint32_t gpio_num)
{
    IO_MUX.gpion[gpio_num].gpion_slp_sel = 1;
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
    IO_MUX.gpion[gpio_num].gpion_slp_sel = 0;
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
    IO_MUX.gpion[gpio_num].gpion_mcu_wpu = 0;
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
    IO_MUX.gpion[gpio_num].gpion_mcu_wpu = 1;
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
    IO_MUX.gpion[gpio_num].gpion_mcu_wpd = 1;
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
    IO_MUX.gpion[gpio_num].gpion_mcu_wpd = 0;
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
    IO_MUX.gpion[gpio_num].gpion_mcu_ie = 0;
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
    IO_MUX.gpion[gpio_num].gpion_mcu_ie = 1;
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
    IO_MUX.gpion[gpio_num].gpion_mcu_oe = 0;
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
    IO_MUX.gpion[gpio_num].gpion_mcu_oe = 1;
}

#ifdef __cplusplus
}
#endif
