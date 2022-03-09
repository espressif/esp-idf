/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The LL layer for ESP32 GPIO register operations

#pragma once

#include <stdbool.h>
#include "soc/soc.h"
#include "soc/gpio_periph.h"
#include "soc/gpio_struct.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_io_reg.h"
#include "hal/gpio_types.h"
#include "hal/misc.h"

#ifdef __cplusplus
extern "C" {
#endif

// Get GPIO hardware instance with giving gpio num
#define GPIO_LL_GET_HW(num) (((num) == 0) ? (&GPIO) : NULL)

#define GPIO_LL_APP_CPU_INTR_ENA      (BIT(0))
#define GPIO_LL_APP_CPU_NMI_INTR_ENA  (BIT(1))
#define GPIO_LL_PRO_CPU_INTR_ENA      (BIT(2))
#define GPIO_LL_PRO_CPU_NMI_INTR_ENA  (BIT(3))
#define GPIO_LL_SDIO_EXT_INTR_ENA     (BIT(4))

/**
  * @brief Enable pull-up on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_pullup_en(gpio_dev_t *hw, gpio_num_t gpio_num)
{
    REG_SET_BIT(GPIO_PIN_MUX_REG[gpio_num], FUN_PU);
}

/**
  * @brief Disable pull-up on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_pullup_dis(gpio_dev_t *hw, gpio_num_t gpio_num)
{
    REG_CLR_BIT(GPIO_PIN_MUX_REG[gpio_num], FUN_PU);
}

/**
  * @brief Return pull-up status on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  * @return if GPIO gpio_num`s FUN_PU is true
  */
static inline bool gpio_ll_pullup_is_enabled(gpio_dev_t *hw, gpio_num_t gpio_num)
{
    return REG_GET_BIT(GPIO_PIN_MUX_REG[gpio_num], FUN_PU) ? true : false;
}

/**
  * @brief Enable pull-down on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_pulldown_en(gpio_dev_t *hw, gpio_num_t gpio_num)
{
    REG_SET_BIT(GPIO_PIN_MUX_REG[gpio_num], FUN_PD);
}

/**
  * @brief Disable pull-down on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_pulldown_dis(gpio_dev_t *hw, gpio_num_t gpio_num)
{
    REG_CLR_BIT(GPIO_PIN_MUX_REG[gpio_num], FUN_PD);
}

/**
  * @brief Return pull-down status on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  * @return if GPIO gpio_num`s FUN_PD is true
  */
static inline bool gpio_ll_pulldown_is_enabled(gpio_dev_t *hw, gpio_num_t gpio_num)
{
    return REG_GET_BIT(GPIO_PIN_MUX_REG[gpio_num], FUN_PD) ? true : false;
}

/**
  * @brief Enable GPIO pin used for wakeup from sleep.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_sel_en(gpio_dev_t *hw, gpio_num_t gpio_num)
{
    PIN_SLP_SEL_ENABLE(GPIO_PIN_MUX_REG[gpio_num]);
}

/**
  * @brief Disable GPIO pin used for wakeup from sleep.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_sel_dis(gpio_dev_t *hw, gpio_num_t gpio_num)
{
    PIN_SLP_SEL_DISABLE(GPIO_PIN_MUX_REG[gpio_num]);
}

/**
  * @brief Return slp-sel status on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  * @return if GPIO gpio_num`s SLP_SEL is true
  */
static inline bool gpio_ll_sleep_sel_is_enabled(gpio_dev_t *hw, gpio_num_t gpio_num)
{
    return REG_GET_BIT(GPIO_PIN_MUX_REG[gpio_num], SLP_SEL) ? true : false;
}

/**
  * @brief Disable GPIO pull-up in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_pullup_dis(gpio_dev_t *hw, gpio_num_t gpio_num)
{
    PIN_SLP_PULLUP_DISABLE(GPIO_PIN_MUX_REG[gpio_num]);
}

/**
  * @brief Enable GPIO pull-up in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_pullup_en(gpio_dev_t *hw, gpio_num_t gpio_num)
{
    PIN_SLP_PULLUP_ENABLE(GPIO_PIN_MUX_REG[gpio_num]);
}

/**
  * @brief Return slp-pull-up status on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  * @return if GPIO gpio_num`s SLP_PU is true
  */
static inline bool gpio_ll_sleep_pullup_is_enabled(gpio_dev_t *hw, gpio_num_t gpio_num)
{
    return REG_GET_BIT(GPIO_PIN_MUX_REG[gpio_num], SLP_PU) ? true : false;
}

/**
  * @brief Enable GPIO pull-down in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_pulldown_en(gpio_dev_t *hw, gpio_num_t gpio_num)
{
    PIN_SLP_PULLDOWN_ENABLE(GPIO_PIN_MUX_REG[gpio_num]);
}

/**
  * @brief Disable GPIO pull-down in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_pulldown_dis(gpio_dev_t *hw, gpio_num_t gpio_num)
{
    PIN_SLP_PULLDOWN_DISABLE(GPIO_PIN_MUX_REG[gpio_num]);
}

/**
  * @brief Return slp-pull-down status on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  * @return if GPIO gpio_num`s SLP_PD is true
  */
static inline bool gpio_ll_sleep_pulldown_is_enabled(gpio_dev_t *hw, gpio_num_t gpio_num)
{
    return REG_GET_BIT(GPIO_PIN_MUX_REG[gpio_num], SLP_PD) ? true : false;
}

/**
 * @brief  GPIO set interrupt trigger type
 *
 * @param  hw Peripheral GPIO hardware instance address.
 * @param  gpio_num GPIO number. If you want to set the trigger type of e.g. of GPIO16, gpio_num should be GPIO_NUM_16 (16);
 * @param  intr_type Interrupt type, select from gpio_int_type_t
 */
static inline void gpio_ll_set_intr_type(gpio_dev_t *hw, gpio_num_t gpio_num, gpio_int_type_t intr_type)
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
static inline void gpio_ll_get_intr_status(gpio_dev_t *hw, uint32_t core_id, uint32_t *status)
{
    *status = (core_id == 0) ? hw->pcpu_int : hw->acpu_int;
}

/**
  * @brief Get GPIO interrupt status high
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param core_id interrupt core id
  * @param status interrupt status high
  */
static inline void gpio_ll_get_intr_status_high(gpio_dev_t *hw, uint32_t core_id, uint32_t *status)
{
    *status = (core_id == 0) ? HAL_FORCE_READ_U32_REG_FIELD(hw->pcpu_int1, intr) : HAL_FORCE_READ_U32_REG_FIELD(hw->acpu_int1, intr);
}

/**
  * @brief Clear GPIO interrupt status
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param mask interrupt status clear mask
  */
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
static inline void gpio_ll_clear_intr_status_high(gpio_dev_t *hw, uint32_t mask)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->status1_w1tc, intr_st, mask);
}

/**
 * @brief  Enable GPIO module interrupt signal
 *
 * @param  hw Peripheral GPIO hardware instance address.
 * @param  core_id Interrupt enabled CPU to corresponding ID
 * @param  gpio_num GPIO number. If you want to enable the interrupt of e.g. GPIO16, gpio_num should be GPIO_NUM_16 (16);
 */
static inline void gpio_ll_intr_enable_on_core(gpio_dev_t *hw, uint32_t core_id, gpio_num_t gpio_num)
{
    if (core_id == 0) {
        hw->pin[gpio_num].int_ena = GPIO_LL_PRO_CPU_INTR_ENA;     //enable pro cpu intr
    } else {
        hw->pin[gpio_num].int_ena = GPIO_LL_APP_CPU_INTR_ENA;     //enable pro cpu intr
    }
}

/**
 * @brief  Disable GPIO module interrupt signal
 *
 * @param  hw Peripheral GPIO hardware instance address.
 * @param  gpio_num GPIO number. If you want to disable the interrupt of e.g. GPIO16, gpio_num should be GPIO_NUM_16 (16);
 */
static inline void gpio_ll_intr_disable(gpio_dev_t *hw, gpio_num_t gpio_num)
{
    hw->pin[gpio_num].int_ena = 0;                             //disable GPIO intr
}

/**
  * @brief Disable input mode on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_input_disable(gpio_dev_t *hw, gpio_num_t gpio_num)
{
    PIN_INPUT_DISABLE(GPIO_PIN_MUX_REG[gpio_num]);
}

/**
  * @brief Enable input mode on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_input_enable(gpio_dev_t *hw, gpio_num_t gpio_num)
{
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[gpio_num]);
}

/**
  * @brief Disable output mode on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_output_disable(gpio_dev_t *hw, gpio_num_t gpio_num)
{
    if (gpio_num < 32) {
        hw->enable_w1tc = (0x1 << gpio_num);
    } else {
        HAL_FORCE_MODIFY_U32_REG_FIELD(hw->enable1_w1tc, data, (0x1 << (gpio_num - 32)));
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
static inline void gpio_ll_output_enable(gpio_dev_t *hw, gpio_num_t gpio_num)
{
    if (gpio_num < 32) {
        hw->enable_w1ts = (0x1 << gpio_num);
    } else {
        HAL_FORCE_MODIFY_U32_REG_FIELD(hw->enable1_w1ts, data, (0x1 << (gpio_num - 32)));
    }
}

/**
  * @brief Disable GPIO input in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_input_disable(gpio_dev_t *hw, gpio_num_t gpio_num)
{
    PIN_SLP_INPUT_DISABLE(GPIO_PIN_MUX_REG[gpio_num]);
}

/**
  * @brief Enable GPIO input in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_input_enable(gpio_dev_t *hw, gpio_num_t gpio_num)
{
    PIN_SLP_INPUT_ENABLE(GPIO_PIN_MUX_REG[gpio_num]);
}

/**
  * @brief Disable GPIO output in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_output_disable(gpio_dev_t *hw, gpio_num_t gpio_num)
{
    PIN_SLP_OUTPUT_DISABLE(GPIO_PIN_MUX_REG[gpio_num]);
}

/**
  * @brief Enable GPIO output in sleep mode.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_sleep_output_enable(gpio_dev_t *hw, gpio_num_t gpio_num)
{
    PIN_SLP_OUTPUT_ENABLE(GPIO_PIN_MUX_REG[gpio_num]);
}

/**
  * @brief Disable open-drain mode on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_od_disable(gpio_dev_t *hw, gpio_num_t gpio_num)
{
    hw->pin[gpio_num].pad_driver = 0;
}

/**
  * @brief Enable open-drain mode on GPIO.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number
  */
static inline void gpio_ll_od_enable(gpio_dev_t *hw, gpio_num_t gpio_num)
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
static inline void gpio_ll_set_level(gpio_dev_t *hw, gpio_num_t gpio_num, uint32_t level)
{
    if (level) {
        if (gpio_num < 32) {
            hw->out_w1ts = (1 << gpio_num);
        } else {
            HAL_FORCE_MODIFY_U32_REG_FIELD(hw->out1_w1ts, data, (1 << (gpio_num - 32)));
        }
    } else {
        if (gpio_num < 32) {
            hw->out_w1tc = (1 << gpio_num);
        } else {
            HAL_FORCE_MODIFY_U32_REG_FIELD(hw->out1_w1tc, data, (1 << (gpio_num - 32)));
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
static inline int gpio_ll_get_level(gpio_dev_t *hw, gpio_num_t gpio_num)
{
    if (gpio_num < 32) {
        return (hw->in >> gpio_num) & 0x1;
    } else {
        return (HAL_FORCE_READ_U32_REG_FIELD(hw->in1, data) >> (gpio_num - 32)) & 0x1;
    }
}

/**
 * @brief Enable GPIO wake-up function.
 *
 * @param hw Peripheral GPIO hardware instance address.
 * @param gpio_num GPIO number.
 * @param intr_type GPIO wake-up type. Only GPIO_INTR_LOW_LEVEL or GPIO_INTR_HIGH_LEVEL can be used.
 */
static inline void gpio_ll_wakeup_enable(gpio_dev_t *hw, gpio_num_t gpio_num, gpio_int_type_t intr_type)
{
    hw->pin[gpio_num].int_type = intr_type;
    hw->pin[gpio_num].wakeup_enable = 0x1;
}

/**
 * @brief Disable GPIO wake-up function.
 *
 * @param hw Peripheral GPIO hardware instance address.
 * @param gpio_num GPIO number
 */
static inline void gpio_ll_wakeup_disable(gpio_dev_t *hw, gpio_num_t gpio_num)
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
static inline void gpio_ll_set_drive_capability(gpio_dev_t *hw, gpio_num_t gpio_num, gpio_drive_cap_t strength)
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
static inline void gpio_ll_get_drive_capability(gpio_dev_t *hw, gpio_num_t gpio_num, gpio_drive_cap_t *strength)
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
  * @brief Enable gpio pad hold function.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number, only support output GPIOs
  */
static inline void gpio_ll_hold_en(gpio_dev_t *hw, gpio_num_t gpio_num)
{
    SET_PERI_REG_MASK(RTC_IO_DIG_PAD_HOLD_REG, GPIO_HOLD_MASK[gpio_num]);
}

/**
  * @brief Disable gpio pad hold function.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number, only support output GPIOs
  */
static inline void gpio_ll_hold_dis(gpio_dev_t *hw, gpio_num_t gpio_num)
{
    CLEAR_PERI_REG_MASK(RTC_IO_DIG_PAD_HOLD_REG, GPIO_HOLD_MASK[gpio_num]);
}

/**
  * @brief Set pad input to a peripheral signal through the IOMUX.
  *
  * @param hw Peripheral GPIO hardware instance address.
  * @param gpio_num GPIO number of the pad.
  * @param signal_idx Peripheral signal id to input. One of the ``*_IN_IDX`` signals in ``soc/gpio_sig_map.h``.
  */
static inline void gpio_ll_iomux_in(gpio_dev_t *hw, uint32_t gpio, uint32_t signal_idx)
{
    hw->func_in_sel_cfg[signal_idx].sig_in_sel = 0;
    PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[gpio]);
}

/**
 * @brief  Select a function for the pin in the IOMUX
 *
 * @param  pin_name Pin name to configure
 * @param  func Function to assign to the pin
 */
static inline __attribute__((always_inline)) void gpio_ll_iomux_func_sel(uint32_t pin_name, uint32_t func)
{
    PIN_FUNC_SELECT(pin_name, func);
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
    gpio_ll_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_num], func);
}

#ifdef __cplusplus
}
#endif
