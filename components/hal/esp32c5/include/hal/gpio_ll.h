/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The LL layer for ESP32-C5 GPIO register operations

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "sdkconfig.h"  // TODO: IDF-9197 remove
#include "soc/soc.h"
#include "soc/gpio_periph.h"
#include "soc/gpio_struct.h"
#include "soc/lp_aon_struct.h"
#include "soc/pmu_struct.h"
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
#include "soc/lp_io_struct.h"
#include "soc/pcr_struct.h"
#include "soc/clk_tree_defs.h"
#include "soc/usb_serial_jtag_struct.h"
#include "soc/io_mux_struct.h"
#include "hal/gpio_types.h"
#elif CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
#include "soc/lp_gpio_struct.h"
#include "soc/usb_serial_jtag_reg.h"
#include "soc/pcr_struct.h"
#include "soc/clk_tree_defs.h"
#include "soc/io_mux_struct.h"
#include "hal/gpio_types.h"
#include "hal/misc.h"
#endif
#include "hal/assert.h"

#ifdef __cplusplus
extern "C" {
#endif

// Get GPIO hardware instance with giving gpio num
#define GPIO_LL_GET_HW(num) (((num) == 0) ? (&GPIO) : NULL)

#define GPIO_LL_PRO_CPU_INTR_ENA      (BIT(0))
#define GPIO_LL_PRO_CPU_NMI_INTR_ENA  (BIT(1))

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
    *pu = IOMUX.gpio[gpio_num].fun_wpu;
    *pd = IOMUX.gpio[gpio_num].fun_wpd;
    *ie = IOMUX.gpio[gpio_num].fun_ie;
    *oe = (hw->enable.val & (1 << gpio_num)) >> gpio_num;
    *od = hw->pin[gpio_num].pad_driver;
    *drv = IOMUX.gpio[gpio_num].fun_drv;
    *fun_sel = IOMUX.gpio[gpio_num].mcu_sel;
    *sig_out = hw->func_out_sel_cfg[gpio_num].out_sel;
    *slp_sel = IOMUX.gpio[gpio_num].slp_sel;
}

/**
  * @brief Enable pull-up on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_pullup_en(gpio_dev_t *hw, uint32_t gpio_num)
{
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    IOMUX.gpio[gpio_num].fun_wpu = 1;
#elif CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
    // REG_SET_BIT(IO_MUX_GPIO0_REG + (gpio_num * 4), FUN_PU);
    abort();
#endif
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
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    IOMUX.gpio[gpio_num].fun_wpu = 0;
#elif CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
    // REG_CLR_BIT(IO_MUX_GPIO0_REG + (gpio_num * 4), FUN_PU);
    abort();
#endif
}

/**
  * @brief Enable pull-down on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_pulldown_en(gpio_dev_t *hw, uint32_t gpio_num)
{
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    IOMUX.gpio[gpio_num].fun_wpd = 1;
#elif CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
    // REG_SET_BIT(IO_MUX_GPIO0_REG + (gpio_num * 4), FUN_PD);
    abort();
#endif
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
    // The pull-up value of the USB pins are controlled by the pins’ pull-up value together with USB pull-up value
    // USB DP pin is default to PU enabled
    // Note that esp32C5 has supported USB_EXCHG_PINS feature. If this efuse is burnt, the gpio pin
    // which should be checked is USB_INT_PHY0_DM_GPIO_NUM instead.
    // TODO: read the specific efuse with efuse_ll.h
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    if (gpio_num == USB_INT_PHY0_DP_GPIO_NUM) {
        USB_SERIAL_JTAG.conf0.pad_pull_override = 1;
        USB_SERIAL_JTAG.conf0.dp_pullup = 0;
    }
    IOMUX.gpio[gpio_num].fun_wpd = 0;
#elif CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
    // if (gpio_num == USB_INT_PHY0_DP_GPIO_NUM) {
    //     SET_PERI_REG_MASK(USB_SERIAL_JTAG_CONF0_REG, USB_SERIAL_JTAG_PAD_PULL_OVERRIDE);
    //     CLEAR_PERI_REG_MASK(USB_SERIAL_JTAG_CONF0_REG, USB_SERIAL_JTAG_DP_PULLUP);
    // }
    // REG_CLR_BIT(IO_MUX_GPIO0_REG + (gpio_num * 4), FUN_PD);
    abort();
#endif
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
    (void)core_id;
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    *status = hw->pcpu_int.procpu_int;
#elif CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
    abort();
#endif
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
    *status = 0; // Less than 32 GPIOs in ESP32-C5
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
    // Less than 32 GPIOs on ESP32-C5 Do nothing.
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
    GPIO.pin[gpio_num].int_ena = GPIO_LL_PRO_CPU_INTR_ENA;     //enable pro cpu intr
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
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    IOMUX.gpio[gpio_num].fun_ie = 0;
#elif CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
    // PIN_INPUT_DISABLE(IO_MUX_GPIO0_REG + (gpio_num * 4));
    abort();
#endif
}

/**
  * @brief Enable input mode on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_input_enable(gpio_dev_t *hw, uint32_t gpio_num)
{
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    IOMUX.gpio[gpio_num].fun_ie = 1;
#elif CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
    // PIN_INPUT_ENABLE(IO_MUX_GPIO0_REG + (gpio_num * 4));
    abort();
#endif
}

/**
 * @brief Enable GPIO pin filter
 *
 * @param hw Peripheral GPIO hardware instance address.
 * @param gpio_num GPIO number of the pad.
 */
static inline void gpio_ll_pin_filter_enable(gpio_dev_t *hw, uint32_t gpio_num)
{
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    IOMUX.gpio[gpio_num].filter_en = 1;
#elif CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
    // PIN_FILTER_EN(IO_MUX_GPIO0_REG + (gpio_num * 4));
    abort();
#endif
}

/**
 * @brief Disable GPIO pin filter
 *
 * @param hw Peripheral GPIO hardware instance address.
 * @param gpio_num GPIO number of the pad.
 */
static inline void gpio_ll_pin_filter_disable(gpio_dev_t *hw, uint32_t gpio_num)
{
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    IOMUX.gpio[gpio_num].filter_en = 0;
#elif CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
    PIN_FILTER_DIS(IO_MUX_GPIO0_REG + (gpio_num * 4));
    abort();
#endif
}

#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
/**
  * @brief Enable GPIO hysteresis
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_pin_input_hysteresis_enable(gpio_dev_t *hw, uint32_t gpio_num)
{
    // On ESP32C5, there is an efuse bit that controls the hysteresis enable or not for all IOs.
    // We are not going to use the hardware control in IDF for C5.
    // Therefore, we need to always switch to use software control first.
    // i.e. Swt hys_sel to 1, so that hys_en determines whether hysteresis is enabled or not
    IOMUX.gpio[gpio_num].hys_sel = 1;
    IOMUX.gpio[gpio_num].hys_en = 1;
}

/**
  * @brief Disable GPIO hysteresis
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_pin_input_hysteresis_disable(gpio_dev_t *hw, uint32_t gpio_num)
{
    IOMUX.gpio[gpio_num].hys_sel = 1;
    IOMUX.gpio[gpio_num].hys_en = 0;
}
#endif

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
        hw->out_w1ts.out_w1ts = (1 << gpio_num);
    } else {
        hw->out_w1tc.out_w1tc = (1 << gpio_num);
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
    return (hw->in.in_data_next >> gpio_num) & 0x1;
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
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    IOMUX.gpio[gpio_num].fun_drv = strength;
#elif CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
    // SET_PERI_REG_BITS(IO_MUX_GPIO0_REG + (gpio_num * 4), FUN_DRV_V, strength, FUN_DRV_S);
    abort();
#endif
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
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    *strength = (gpio_drive_cap_t)(IOMUX.gpio[gpio_num].fun_drv);
#elif CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
    // *strength = (gpio_drive_cap_t)GET_PERI_REG_BITS2(IO_MUX_GPIO0_REG + (gpio_num * 4), FUN_DRV_V, FUN_DRV_S);
    abort();
#endif
}

/**
  * @brief Enable gpio pad hold function.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number, only support output GPIOs
  */
static inline void gpio_ll_hold_en(gpio_dev_t *hw, uint32_t gpio_num)
{
    LP_AON.gpio_hold0.gpio_hold0 |= GPIO_HOLD_MASK[gpio_num];
}

/**
  * @brief Disable gpio pad hold function.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number, only support output GPIOs
  */
static inline void gpio_ll_hold_dis(gpio_dev_t *hw, uint32_t gpio_num)
{
    LP_AON.gpio_hold0.gpio_hold0 &= ~GPIO_HOLD_MASK[gpio_num];
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
  * @brief Set pad input to a peripheral signal through the IOMUX.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number of the pad.
  * @param signal_idx Peripheral signal id to input. One of the ``*_IN_IDX`` signals in ``soc/gpio_sig_map.h``.
  */
__attribute__((always_inline))
static inline void gpio_ll_iomux_in(gpio_dev_t *hw, uint32_t gpio, uint32_t signal_idx)
{
    hw->func_in_sel_cfg[signal_idx].sig_in_sel = 0;
    IOMUX.gpio[gpio].fun_ie = 1;
}

/**
 * @brief  Select a function for the pin in the IOMUX
 *
 * @param  pin_name Pin name to configure
 * @param  func Function to assign to the pin
 */
static inline void gpio_ll_iomux_func_sel(uint32_t pin_name, uint32_t func)
{
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    // Disable USB Serial JTAG if pins 25 or pins 26 needs to select an IOMUX function
    if (pin_name == IO_MUX_GPIO25_REG || pin_name == IO_MUX_GPIO26_REG) {
        USB_SERIAL_JTAG.conf0.usb_pad_enable = 0;
    }
#elif CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
    // // Disable USB Serial JTAG if pins 12 or pins 13 needs to select an IOMUX function
    // if (pin_name == IO_MUX_GPIO12_REG || pin_name == IO_MUX_GPIO13_REG) {
    //     CLEAR_PERI_REG_MASK(USB_SERIAL_JTAG_CONF0_REG, USB_SERIAL_JTAG_USB_PAD_ENABLE);
    // }
    abort();
#endif
    PIN_FUNC_SELECT(pin_name, func);
}

/**
 * @brief  Control the pin in the IOMUX
 *
 * @param  bmap   write mask of control value
 * @param  val    Control value
 * @param  shift  write mask shift of control value
 */
static inline __attribute__((always_inline)) void gpio_ll_set_pin_ctrl(uint32_t val, uint32_t bmap, uint32_t shift)
{
    SET_PERI_REG_BITS(PIN_CTRL, bmap, val, shift);
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
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    // Disable USB Serial JTAG if pins 25 or pins 26 needs to select an IOMUX function
    if (gpio_num == USB_INT_PHY0_DM_GPIO_NUM || gpio_num == USB_INT_PHY0_DP_GPIO_NUM) {
        USB_SERIAL_JTAG.conf0.usb_pad_enable = 0;
    }
    IOMUX.gpio[gpio_num].mcu_sel = func;
#elif CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
    // // Disable USB Serial JTAG if pins 12 or pins 13 needs to select an IOMUX function
    // if (gpio_num == USB_INT_PHY0_DM_GPIO_NUM || gpio_num == USB_INT_PHY0_DP_GPIO_NUM) {
    //     CLEAR_PERI_REG_MASK(USB_SERIAL_JTAG_CONF0_REG, USB_SERIAL_JTAG_USB_PAD_ENABLE);
    // }
    // PIN_FUNC_SELECT(IO_MUX_GPIO0_REG + (gpio_num * 4), func);
    abort();
#endif
}

/**
  * @brief Set peripheral output to an GPIO pad through the IOMUX.
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
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
     case SOC_MOD_CLK_XTAL:
        PCR.iomux_clk_conf.iomux_func_clk_sel = 0;
        break;
    case SOC_MOD_CLK_RC_FAST:
        PCR.iomux_clk_conf.iomux_func_clk_sel = 1;
        break;
    case SOC_MOD_CLK_PLL_F80M:
        PCR.iomux_clk_conf.iomux_func_clk_sel = 2;
        break;
#elif CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
    case SOC_MOD_CLK_XTAL:
        PCR.iomux_clk_conf.iomux_func_clk_sel = 3;
        break;
    case SOC_MOD_CLK_PLL_F80M:
        PCR.iomux_clk_conf.iomux_func_clk_sel = 1;
        break;
#endif
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
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    IOMUX.gpio[gpio_num].slp_sel = 1;
#elif CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
    // PIN_SLP_SEL_ENABLE(IO_MUX_GPIO0_REG + (gpio_num * 4));
    abort();
#endif
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
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    IOMUX.gpio[gpio_num].slp_sel = 0;
#elif CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
    // PIN_SLP_SEL_DISABLE(IO_MUX_GPIO0_REG + (gpio_num * 4));
    abort();
#endif
}

/**
  * @brief Disable GPIO pull-up in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_pullup_dis(gpio_dev_t *hw, uint32_t gpio_num)
{
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    IOMUX.gpio[gpio_num].mcu_wpu = 0;
#elif CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
    // PIN_SLP_PULLUP_DISABLE(IO_MUX_GPIO0_REG + (gpio_num * 4));
    abort();
#endif
}

/**
  * @brief Enable GPIO pull-up in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_pullup_en(gpio_dev_t *hw, uint32_t gpio_num)
{
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    IOMUX.gpio[gpio_num].mcu_wpu = 1;
#elif CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
    // PIN_SLP_PULLUP_ENABLE(IO_MUX_GPIO0_REG + (gpio_num * 4));
    abort();
#endif
}

/**
  * @brief Enable GPIO pull-down in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_pulldown_en(gpio_dev_t *hw, uint32_t gpio_num)
{
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    IOMUX.gpio[gpio_num].mcu_wpd = 1;
#elif CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
    // PIN_SLP_PULLDOWN_ENABLE(IO_MUX_GPIO0_REG + (gpio_num * 4));
    abort();
#endif
}

/**
  * @brief Disable GPIO pull-down in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_pulldown_dis(gpio_dev_t *hw, uint32_t gpio_num)
{
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    IOMUX.gpio[gpio_num].mcu_wpd = 0;
#elif CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
    // PIN_SLP_PULLDOWN_DISABLE(IO_MUX_GPIO0_REG + (gpio_num * 4));
    abort();
#endif
}

/**
  * @brief Disable GPIO input in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_input_disable(gpio_dev_t *hw, uint32_t gpio_num)
{
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    IOMUX.gpio[gpio_num].mcu_ie = 0;
#elif CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
    // PIN_SLP_INPUT_DISABLE(IO_MUX_GPIO0_REG + (gpio_num * 4));
    abort();
#endif
}

/**
  * @brief Enable GPIO input in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_input_enable(gpio_dev_t *hw, uint32_t gpio_num)
{
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    IOMUX.gpio[gpio_num].mcu_ie = 1;
#elif CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
    // PIN_SLP_INPUT_ENABLE(IO_MUX_GPIO0_REG + (gpio_num * 4));
    abort();
#endif
}

/**
  * @brief Disable GPIO output in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_output_disable(gpio_dev_t *hw, uint32_t gpio_num)
{
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    IOMUX.gpio[gpio_num].mcu_oe = 0;
#elif CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
    // PIN_SLP_OUTPUT_DISABLE(IO_MUX_GPIO0_REG + (gpio_num * 4));
    abort();
#endif
}

/**
  * @brief Enable GPIO output in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_output_enable(gpio_dev_t *hw, uint32_t gpio_num)
{
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
    IOMUX.gpio[gpio_num].mcu_oe = 1;
#elif CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
    // PIN_SLP_OUTPUT_ENABLE(IO_MUX_GPIO0_REG + (gpio_num * 4));
    abort();
#endif
}

#ifdef __cplusplus
}
#endif
