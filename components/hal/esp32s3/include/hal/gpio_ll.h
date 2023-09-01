/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The LL layer for ESP32-S3 GPIO register operations

#pragma once

#include <stdbool.h>
#include "soc/soc.h"
#include "soc/gpio_periph.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_io_reg.h"
#include "soc/usb_serial_jtag_reg.h"
#include "hal/gpio_types.h"
#include "soc/gpio_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

// Get GPIO hardware instance with giving gpio num
#define GPIO_LL_GET_HW(num) (((num) == 0) ? (&GPIO) : NULL)

// On ESP32S3, pro cpu and app cpu shares the same interrupt enable bit
#define GPIO_LL_INTR_ENA      (BIT(0))
#define GPIO_LL_NMI_INTR_ENA  (BIT(1))

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
    uint32_t iomux_reg_val = REG_READ(GPIO_PIN_MUX_REG[gpio_num]);
    *pu = (iomux_reg_val & FUN_PU_M) >> FUN_PU_S;
    *pd = (iomux_reg_val & FUN_PD_M) >> FUN_PD_S;
    *ie = (iomux_reg_val & FUN_IE_M) >> FUN_IE_S;
    *oe = (((gpio_num < 32) ? hw->enable : hw->enable1.val) & bit_mask) >> bit_shift;
    *od = hw->pin[gpio_num].pad_driver;
    *drv = (iomux_reg_val & FUN_DRV_M) >> FUN_DRV_S;
    *fun_sel = (iomux_reg_val & MCU_SEL_M) >> MCU_SEL_S;
    *sig_out = hw->func_out_sel_cfg[gpio_num].func_sel;
    *slp_sel = (iomux_reg_val & SLP_SEL_M) >> SLP_SEL_S;
}

/**
  * @brief Enable pull-up on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_pullup_en(gpio_dev_t *hw, uint32_t gpio_num)
{
    REG_SET_BIT(GPIO_PIN_MUX_REG[gpio_num], FUN_PU);
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
    // Note that from esp32s3 ECO1, USB_EXCHG_PINS feature has been supported. If this efuse is burnt, the gpio pin
    // which should be checked is USB_INT_PHY0_DM_GPIO_NUM instead.
    if (gpio_num == USB_INT_PHY0_DP_GPIO_NUM) {
        SET_PERI_REG_MASK(USB_SERIAL_JTAG_CONF0_REG, USB_SERIAL_JTAG_PAD_PULL_OVERRIDE);
        CLEAR_PERI_REG_MASK(USB_SERIAL_JTAG_CONF0_REG, USB_SERIAL_JTAG_DP_PULLUP);
    }
    REG_CLR_BIT(IO_MUX_GPIO0_REG + (gpio_num * 4), FUN_PU);
}

/**
  * @brief Enable pull-down on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_pulldown_en(gpio_dev_t *hw, uint32_t gpio_num)
{
    REG_SET_BIT(GPIO_PIN_MUX_REG[gpio_num], FUN_PD);
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
    REG_CLR_BIT(IO_MUX_GPIO0_REG + (gpio_num * 4), FUN_PD);
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
    // On ESP32S3, pcpu_int register represents GPIO0-31 interrupt status on both cores
    (void)core_id;
    *status = hw->pcpu_int;
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
    // On ESP32S3, pcpu_int1 register represents GPIO32-48 interrupt status on both cores
    (void)core_id;
    *status = hw->pcpu_int1.intr;
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
    hw->status_w1tc = mask;
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
    hw->status1_w1tc.intr_st = mask;
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
    GPIO.pin[gpio_num].int_ena = GPIO_LL_INTR_ENA;     //enable intr
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
    PIN_INPUT_DISABLE(IO_MUX_GPIO0_REG + (gpio_num * 4));
}

/**
  * @brief Enable input mode on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_input_enable(gpio_dev_t *hw, uint32_t gpio_num)
{
    PIN_INPUT_ENABLE(IO_MUX_GPIO0_REG + (gpio_num * 4));
}

/**
 * @brief Enable GPIO pin filter
 *
 * @param hw Peripheral GPIO hardware instance address.
 * @param gpio_num GPIO number of the pad.
 */
static inline void gpio_ll_pin_filter_enable(gpio_dev_t *hw, uint32_t gpio_num)
{
    PIN_FILTER_EN(IO_MUX_GPIO0_REG + (gpio_num * 4));
}

/**
 * @brief Disable GPIO pin filter
 *
 * @param hw Peripheral GPIO hardware instance address.
 * @param gpio_num GPIO number of the pad.
 */
static inline void gpio_ll_pin_filter_disable(gpio_dev_t *hw, uint32_t gpio_num)
{
    PIN_FILTER_DIS(IO_MUX_GPIO0_REG + (gpio_num * 4));
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
        hw->enable_w1tc = (0x1 << gpio_num);
    } else {
        hw->enable1_w1tc.data = (0x1 << (gpio_num - 32));
    }

    // Ensure no other output signal is routed via GPIO matrix to this pin
    REG_WRITE(GPIO_FUNC0_OUT_SEL_CFG_REG + (gpio_num * 4),
              SIG_GPIO_OUT_IDX);
}

/**
  * @brief Enable output mode on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline __attribute__((always_inline)) void gpio_ll_output_enable(gpio_dev_t *hw, uint32_t gpio_num)
{
    if (gpio_num < 32) {
        hw->enable_w1ts = (0x1 << gpio_num);
    } else {
        hw->enable1_w1ts.data = (0x1 << (gpio_num - 32));
    }
}

/**
  * @brief Disable open-drain mode on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline __attribute__((always_inline)) void gpio_ll_od_disable(gpio_dev_t *hw, uint32_t gpio_num)
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
 * @brief  Select a function for the pin in the IOMUX
 *
 * @param  hw Peripheral GPIO hardware instance address.
 * @param  gpio_num GPIO number
 * @param  func Function to assign to the pin
 */
static inline __attribute__((always_inline)) void gpio_ll_func_sel(gpio_dev_t *hw, uint8_t gpio_num, uint32_t func)
{
    if (gpio_num == USB_INT_PHY0_DM_GPIO_NUM || gpio_num == USB_INT_PHY0_DP_GPIO_NUM) {
        CLEAR_PERI_REG_MASK(USB_SERIAL_JTAG_CONF0_REG, USB_SERIAL_JTAG_USB_PAD_ENABLE);
    }
    PIN_FUNC_SELECT(IO_MUX_GPIO0_REG + (gpio_num * 4), func);
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
            hw->out_w1ts = (1 << gpio_num);
        } else {
            hw->out1_w1ts.data = (1 << (gpio_num - 32));
        }
    } else {
        if (gpio_num < 32) {
            hw->out_w1tc = (1 << gpio_num);
        } else {
            hw->out1_w1tc.data = (1 << (gpio_num - 32));
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
static inline int gpio_ll_get_level(gpio_dev_t *hw, uint32_t gpio_num)
{
    if (gpio_num < 32) {
        return (hw->in >> gpio_num) & 0x1;
    } else {
        return (hw->in1.data >> (gpio_num - 32)) & 0x1;
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
    hw->pin[gpio_num].wakeup_enable = 0x1;
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
    SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[gpio_num], FUN_DRV_V, strength, FUN_DRV_S);
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
    *strength = (gpio_drive_cap_t)GET_PERI_REG_BITS2(GPIO_PIN_MUX_REG[gpio_num], FUN_DRV_V, FUN_DRV_S);
}

/**
  * @brief Enable all digital gpio pad hold function during Deep-sleep.
  *
  * @param hw Peripheral GPIO hardware instance address.
  */
static inline void gpio_ll_deep_sleep_hold_en(gpio_dev_t *hw)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_DG_PAD_FORCE_UNHOLD);
    SET_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_DG_PAD_AUTOHOLD_EN_M);
}

/**
  * @brief Disable all digital gpio pad hold function during Deep-sleep.
  *
  * @param hw Peripheral GPIO hardware instance address.
  */
static inline void gpio_ll_deep_sleep_hold_dis(gpio_dev_t *hw)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_DG_PAD_AUTOHOLD_EN_M);
}

/**
 * @brief  Get deep sleep hold status
 *
 * @param  hw Peripheral GPIO hardware instance address.
 *
 * @return
 *     - true  deep sleep hold is enabled
 *     - false deep sleep hold is disabled
 */
__attribute__((always_inline))
static inline bool gpio_ll_deep_sleep_hold_is_en(gpio_dev_t *hw)
{
    return !GET_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_DG_PAD_FORCE_UNHOLD) && GET_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_DG_PAD_AUTOHOLD_EN_M);
}

/**
  * @brief Enable gpio pad hold function.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number, only support output GPIOs
  */
static inline void gpio_ll_hold_en(gpio_dev_t *hw, uint32_t gpio_num)
{
    SET_PERI_REG_MASK(RTC_CNTL_DIG_PAD_HOLD_REG, GPIO_HOLD_MASK[gpio_num]);
}

/**
  * @brief Disable gpio pad hold function.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number, only support output GPIOs
  */
static inline void gpio_ll_hold_dis(gpio_dev_t *hw, uint32_t gpio_num)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_PAD_HOLD_REG, GPIO_HOLD_MASK[gpio_num]);
}

/**
  * @brief Get gpio pad hold status.
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
    return GET_PERI_REG_MASK(RTC_CNTL_DIG_PAD_HOLD_REG, BIT(gpio_num - 21));
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
    PIN_INPUT_ENABLE(IO_MUX_GPIO0_REG + (gpio * 4));
}

/**
 * @brief  Select a function for the pin in the IOMUX
 *
 * @param  pin_name Pin name to configure
 * @param  func Function to assign to the pin
 */
static inline __attribute__((always_inline)) void gpio_ll_iomux_func_sel(uint32_t pin_name, uint32_t func)
{
    if (pin_name == IO_MUX_GPIO19_REG || pin_name == IO_MUX_GPIO20_REG) {
        CLEAR_PERI_REG_MASK(USB_SERIAL_JTAG_CONF0_REG, USB_SERIAL_JTAG_USB_PAD_ENABLE);
    }
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
    typeof(hw->func_in_sel_cfg[in_sig_idx]) reg;
    reg.val = hw->func_in_sel_cfg[in_sig_idx].val;
    return (reg.sig_in_sel ? reg.func_sel : -1);
}

/**
  * @brief Force hold digital gpio pad.
  * @note GPIO force hold, whether the chip in sleep mode or wakeup mode.
  */
static inline void gpio_ll_force_hold_all(void)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_DG_PAD_FORCE_UNHOLD);
    SET_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_DG_PAD_FORCE_HOLD);
}

/**
  * @brief Force unhold digital gpio pad.
  * @note GPIO force unhold, whether the chip in sleep mode or wakeup mode.
  */
static inline void gpio_ll_force_unhold_all(void)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_DG_PAD_FORCE_HOLD);
    SET_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_DG_PAD_FORCE_UNHOLD);
    SET_PERI_REG_MASK(RTC_CNTL_DIG_ISO_REG, RTC_CNTL_CLR_DG_PAD_AUTOHOLD);
}

/**
  * @brief Enable GPIO pin used for wakeup from sleep.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_sel_en(gpio_dev_t *hw, uint32_t gpio_num)
{
    PIN_SLP_SEL_ENABLE(GPIO_PIN_MUX_REG[gpio_num]);
}

/**
  * @brief Disable GPIO pin used for wakeup from sleep.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_sel_dis(gpio_dev_t *hw, uint32_t gpio_num)
{
    PIN_SLP_SEL_DISABLE(GPIO_PIN_MUX_REG[gpio_num]);
}

/**
  * @brief Disable GPIO pull-up in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_pullup_dis(gpio_dev_t *hw, uint32_t gpio_num)
{
    PIN_SLP_PULLUP_DISABLE(GPIO_PIN_MUX_REG[gpio_num]);
}

/**
  * @brief Enable GPIO pull-up in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_pullup_en(gpio_dev_t *hw, uint32_t gpio_num)
{
    PIN_SLP_PULLUP_ENABLE(GPIO_PIN_MUX_REG[gpio_num]);
}

/**
  * @brief Enable GPIO pull-down in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_pulldown_en(gpio_dev_t *hw, uint32_t gpio_num)
{
    PIN_SLP_PULLDOWN_ENABLE(GPIO_PIN_MUX_REG[gpio_num]);
}

/**
  * @brief Disable GPIO pull-down in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_pulldown_dis(gpio_dev_t *hw, uint32_t gpio_num)
{
    PIN_SLP_PULLDOWN_DISABLE(GPIO_PIN_MUX_REG[gpio_num]);
}

/**
  * @brief Disable GPIO input in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_input_disable(gpio_dev_t *hw, uint32_t gpio_num)
{
    PIN_SLP_INPUT_DISABLE(GPIO_PIN_MUX_REG[gpio_num]);
}

/**
  * @brief Enable GPIO input in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_input_enable(gpio_dev_t *hw, uint32_t gpio_num)
{
    PIN_SLP_INPUT_ENABLE(GPIO_PIN_MUX_REG[gpio_num]);
}

/**
  * @brief Disable GPIO output in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_output_disable(gpio_dev_t *hw, uint32_t gpio_num)
{
    PIN_SLP_OUTPUT_DISABLE(GPIO_PIN_MUX_REG[gpio_num]);
}

/**
  * @brief Enable GPIO output in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_output_enable(gpio_dev_t *hw, uint32_t gpio_num)
{
    PIN_SLP_OUTPUT_ENABLE(GPIO_PIN_MUX_REG[gpio_num]);
}

#ifdef __cplusplus
}
#endif
