/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The HAL layer for GPIO

#pragma once

#include "soc/soc_caps.h"
#include "hal/gpio_ll.h"
#include "hal/gpio_types.h"
#include "soc/gpio_sig_map.h"

#ifdef __cplusplus
extern "C" {
#endif

// Get GPIO hardware instance with giving gpio num
#define GPIO_HAL_GET_HW(num) GPIO_LL_GET_HW(num)

/**
 * Context that should be maintained by both the driver and the HAL
 */

typedef struct {
    gpio_dev_t *dev;
} gpio_hal_context_t;

/**
 * @brief Get the configuration for an IO
 *
 * @param hal Context of the HAL layer
 * @param gpio_num GPIO number
 * @param[out] out_io_config Pointer to the structure that saves the specific IO configuration
 */
#define gpio_hal_get_io_config(hal, gpio_num, out_io_config) gpio_ll_get_io_config((hal)->dev, gpio_num, out_io_config)

/**
  * @brief Enable pull-up on GPIO.
  *
  * @param hal Context of the HAL layer
  * @param gpio_num GPIO number
  */
#define gpio_hal_pullup_en(hal, gpio_num) gpio_ll_pullup_en((hal)->dev, gpio_num)

/**
  * @brief Disable pull-up on GPIO.
  *
  * @param hal Context of the HAL layer
  * @param gpio_num GPIO number
  */
#define gpio_hal_pullup_dis(hal, gpio_num) gpio_ll_pullup_dis((hal)->dev, gpio_num)

/**
  * @brief Enable pull-down on GPIO.
  *
  * @param hal Context of the HAL layer
  * @param gpio_num GPIO number
  */
#define gpio_hal_pulldown_en(hal, gpio_num) gpio_ll_pulldown_en((hal)->dev, gpio_num)

/**
  * @brief Disable pull-down on GPIO.
  *
  * @param hal Context of the HAL layer
  * @param gpio_num GPIO number
  */
#define gpio_hal_pulldown_dis(hal, gpio_num) gpio_ll_pulldown_dis((hal)->dev, gpio_num)

/**
 * @brief  GPIO set interrupt trigger type
 *
 * @param  hal Context of the HAL layer
 * @param  gpio_num GPIO number. If you want to set the trigger type of e.g. of GPIO16, gpio_num should be GPIO_NUM_16 (16);
 * @param  intr_type Interrupt type, select from gpio_int_type_t
 */
#define gpio_hal_set_intr_type(hal, gpio_num, intr_type) gpio_ll_set_intr_type((hal)->dev, gpio_num, intr_type)

/**
  * @brief Get GPIO interrupt status
  *
  * @param hal Context of the HAL layer
  * @param core_id interrupt core id
  * @param status interrupt status
  */
#define gpio_hal_get_intr_status(hal, core_id, status) gpio_ll_get_intr_status((hal)->dev, core_id, status)

/**
  * @brief Get GPIO interrupt status high
  *
  * @param hal Context of the HAL layer
  * @param core_id interrupt core id
  * @param status interrupt status high
  */
#define gpio_hal_get_intr_status_high(hal, core_id, status) gpio_ll_get_intr_status_high((hal)->dev, core_id, status)

/**
 * @brief Clear GPIO interrupt status bit
 *
 * @param hal Context of the HAL layer
 * @param gpio_num GPIO number. If you want to clear the interrupt status bit of e.g. GPIO16, gpio_num should be GPIO_NUM_16 (16);
 */
#define gpio_hal_clear_intr_status_bit(hal, gpio_num) (((gpio_num) < 32) ? gpio_ll_clear_intr_status((hal)->dev, 1 << gpio_num) \
                                                                         : gpio_ll_clear_intr_status_high((hal)->dev, 1 << (gpio_num - 32)))

/**
 * @brief  Enable GPIO module interrupt signal
 *
 * @param  hal Context of the HAL layer
 * @param  gpio_num GPIO number. If you want to enable the interrupt of e.g. GPIO16, gpio_num should be GPIO_NUM_16 (16);
 * @param  core_id Interrupt enabled CPU to corresponding ID
 */
void gpio_hal_intr_enable_on_core(gpio_hal_context_t *hal, uint32_t gpio_num, uint32_t core_id);

/**
 * @brief  Disable GPIO module interrupt signal
 *
 * @param  hal Context of the HAL layer
 * @param  gpio_num GPIO number. If you want to disable the interrupt of e.g. GPIO16, gpio_num should be GPIO_NUM_16 (16);
 */
void gpio_hal_intr_disable(gpio_hal_context_t *hal, uint32_t gpio_num);

/**
  * @brief Disable input mode on GPIO.
  *
  * @param hal Context of the HAL layer
  * @param gpio_num GPIO number
  */
#define gpio_hal_input_disable(hal, gpio_num) gpio_ll_input_disable((hal)->dev, gpio_num)

/**
  * @brief Enable input mode on GPIO.
  *
  * @param hal Context of the HAL layer
  * @param gpio_num GPIO number
  */
#define gpio_hal_input_enable(hal, gpio_num) gpio_ll_input_enable((hal)->dev, gpio_num)

/**
  * @brief Disable output mode on GPIO.
  *
  * @param hal Context of the HAL layer
  * @param gpio_num GPIO number
  */
#define gpio_hal_output_disable(hal, gpio_num) gpio_ll_output_disable((hal)->dev, gpio_num)

/**
  * @brief Enable output mode on GPIO.
  *
  * @param hal Context of the HAL layer
  * @param gpio_num GPIO number
  */
#define gpio_hal_output_enable(hal, gpio_num) gpio_ll_output_enable((hal)->dev, gpio_num)

/**
  * @brief Configure the source of output enable signal for the pad (only takes effect if func sel is selected to be GPIO).
  *
  * @param hal Context of the HAL layer
  * @param gpio_num GPIO number
  * @param ctrl_by_periph True if use output enable signal from peripheral, false if force the output enable signal to be sourced from bit n of GPIO_ENABLE_REG
  * @param oen_inv True if the output enable needs to be inverted, otherwise False
  */
#define gpio_hal_set_output_enable_ctrl(hal, gpio_num, ctrl_by_periph, oen_inv) gpio_ll_set_output_enable_ctrl((hal)->dev, gpio_num, ctrl_by_periph, oen_inv)

/**
  * @brief Disable open-drain mode on GPIO.
  *
  * @param hal Context of the HAL layer
  * @param gpio_num GPIO number
  */
#define gpio_hal_od_disable(hal, gpio_num) gpio_ll_od_disable((hal)->dev, gpio_num)

/**
  * @brief Enable open-drain mode on GPIO.
  *
  * @param hal Context of the HAL layer
  * @param gpio_num GPIO number
  */
#define gpio_hal_od_enable(hal, gpio_num) gpio_ll_od_enable((hal)->dev, gpio_num)

/**
  * @brief Disconnect any peripheral output signal routed via GPIO matrix to the pin
  *
  * @param hal Context of the HAL layer
  * @param gpio_num GPIO number
  */
#define gpio_hal_matrix_out_default(hal, gpio_num) gpio_ll_set_output_signal_matrix_source((hal)->dev, gpio_num, SIG_GPIO_OUT_IDX, false)

/**
 * @brief  Select a function for the pin in the IOMUX
 *
 * @param  hal Context of the HAL layer
 * @param  gpio_num GPIO number
 * @param  func Function to assign to the pin
 */
#define gpio_hal_func_sel(hal, gpio_num, func)  gpio_ll_func_sel((hal)->dev, gpio_num, func)

/**
 * @brief  Get the GPIO number that is routed to the input peripheral signal through GPIO matrix
 *
 * @param  hal Context of the HAL layer
 * @param  in_sig_idx Peripheral signal index (tagged as input attribute)
 *
 * @return
 *    - -1     Signal bypassed GPIO matrix
 *    - Others GPIO number
 */
#define gpio_hal_get_in_signal_connected_io(hal, in_sig_idx) gpio_ll_get_in_signal_connected_io((hal)->dev, in_sig_idx)

/**
 * @brief  GPIO set output level
 *
 * @param  hal Context of the HAL layer
 * @param  gpio_num GPIO number. If you want to set the output level of e.g. GPIO16, gpio_num should be GPIO_NUM_16 (16);
 * @param  level Output level. 0: low ; 1: high
 */
#define gpio_hal_set_level(hal, gpio_num, level) gpio_ll_set_level((hal)->dev, gpio_num, level)

/**
 * @brief  GPIO get input level
 *
 * @warning If the pad is not configured for input (or input and output) the returned value is always 0.
 *
 * @param  hal Context of the HAL layer
 * @param  gpio_num GPIO number. If you want to get the logic level of e.g. pin GPIO16, gpio_num should be GPIO_NUM_16 (16);
 *
 * @return
 *     - 0 the GPIO input level is 0
 *     - 1 the GPIO input level is 1
 */
#define gpio_hal_get_level(hal, gpio_num) gpio_ll_get_level((hal)->dev, gpio_num)

/**
 * @brief Enable GPIO wake-up function.
 *
 * @param hal Context of the HAL layer
 * @param gpio_num GPIO number.
 */
#define gpio_hal_wakeup_enable(hal, gpio_num) gpio_ll_wakeup_enable((hal)->dev, gpio_num)

/**
 * @brief Disable GPIO wake-up function.
 *
 * @param hal Context of the HAL layer
 * @param gpio_num GPIO number
 */
#define gpio_hal_wakeup_disable(hal, gpio_num) gpio_ll_wakeup_disable((hal)->dev, gpio_num)

/**
  * @brief Set GPIO pad drive capability
  *
  * @param hal Context of the HAL layer
  * @param gpio_num GPIO number, only support output GPIOs
  * @param strength Drive capability of the pad
  */
#define gpio_hal_set_drive_capability(hal, gpio_num, strength) gpio_ll_set_drive_capability((hal)->dev, gpio_num, strength)

/**
  * @brief Get GPIO pad drive capability
  *
  * @param hal Context of the HAL layer
  * @param gpio_num GPIO number, only support output GPIOs
  * @param strength Pointer to accept drive capability of the pad
  */
#define gpio_hal_get_drive_capability(hal, gpio_num, strength) gpio_ll_get_drive_capability((hal)->dev, gpio_num, strength)

/**
  * @brief Enable gpio pad hold function.
  *
  * The gpio pad hold function works in both input and output modes, but must be output-capable gpios.
  * If pad hold enabled:
  *   in output mode: the output level of the pad will be force locked and can not be changed.
  *   in input mode: the input value read will not change, regardless the changes of input signal.
  *
  * On ESP32/S2/C3/S3/C2, the state of digital gpio cannot be held during Deep-sleep, and it will resume the hold
  * function when the chip wakes up from Deep-sleep. If the digital gpio also needs to be held during Deep-sleep,
  * `gpio_deep_sleep_hold_en` should also be called.
  *
  * Power down or call gpio_hold_dis will disable this function.
  *
  * @param hal Context of the HAL layer
  * @param gpio_num GPIO number, only support output GPIOs
  */
#define gpio_hal_hold_en(hal, gpio_num) gpio_ll_hold_en((hal)->dev, gpio_num)

/**
  * @brief Disable gpio pad hold function.
  *
  * When the chip is woken up from Deep-sleep, the gpio will be set to the default mode, so, the gpio will output
  * the default level if this function is called. If you don't want the level changes, the gpio should be configured to
  * a known state before this function is called.
  *  e.g.
  *     If you hold gpio18 high during Deep-sleep, after the chip is woken up and `gpio_hold_dis` is called,
  *     gpio18 will output low level(because gpio18 is input mode by default). If you don't want this behavior,
  *     you should configure gpio18 as output mode and set it to high level before calling `gpio_hold_dis`.
  *
  * @param hal Context of the HAL layer
  * @param gpio_num GPIO number, only support output GPIOs
  */
#define gpio_hal_hold_dis(hal, gpio_num) gpio_ll_hold_dis((hal)->dev, gpio_num)

/**
  * @brief Get whether digital gpio pad is held
  *
  * @param hal Context of the HAL layer
  * @param gpio_num GPIO number, only support output GPIOs
  *
  * @note digital io means io pad powered by VDD3P3_CPU or VDD_SPI
  *       rtc io means io pad powered by VDD3P3_RTC
  *       caller must ensure that gpio_num is a digital io pad
  *
  * @return
  *     - true  digital gpio pad is held
  *     - false digital gpio pad is unheld
  */
#define gpio_hal_is_digital_io_hold(hal, gpio_num) gpio_ll_is_digital_io_hold((hal)->dev, gpio_num)

#if !SOC_GPIO_SUPPORT_HOLD_SINGLE_IO_IN_DSLP
/**
  * @brief Enable all digital gpio pad hold function during Deep-sleep.
  *
  * When the chip is in Deep-sleep mode, all digital gpio will hold the state before sleep, and when the chip is woken up,
  * the status of digital gpio will not be held. Note that the pad hold feature only works when the chip is in Deep-sleep mode,
  * when not in sleep mode, the digital gpio state can be changed even you have called this function.
  *
  * Power down or call gpio_hold_dis will disable this function, otherwise, the digital gpio hold feature works as long as the chip enter Deep-sleep.
  *
  * @param hal Context of the HAL layer
  */
#define gpio_hal_deep_sleep_hold_en(hal) gpio_ll_deep_sleep_hold_en((hal)->dev)

/**
  * @brief Disable all digital gpio pad hold function during Deep-sleep.
  *
  * @param hal Context of the HAL layer
  */
#define gpio_hal_deep_sleep_hold_dis(hal) gpio_ll_deep_sleep_hold_dis((hal)->dev)

/**
  * @brief Get whether all digital gpio pad hold function during Deep-sleep is enabled.
  *
  * @param hal Context of the HAL layer
  *
  * @return
  *     - true  deep sleep hold is enabled
  *     - false deep sleep hold is disabled
  */
#define gpio_hal_deep_sleep_hold_is_en(hal) gpio_ll_deep_sleep_hold_is_en((hal)->dev)
#endif //!SOC_GPIO_SUPPORT_HOLD_SINGLE_IO_IN_DSLP

/**
  * @brief Set pad input to a peripheral signal through the IOMUX.
  *
  * @param hal Context of the HAL layer
  * @param gpio_num GPIO number of the pad.
  * @param func The index number of the IOMUX function to be selected for the pin.
  *        One of the ``FUNC_X_*`` of specified pin (X) in ``soc/io_mux_reg.h``.
  * @param signal_idx Peripheral signal index to input. One of the ``*_IN_IDX`` signals in ``soc/gpio_sig_map.h``.
  */
void gpio_hal_iomux_in(gpio_hal_context_t *hal, uint32_t gpio_num, int func, uint32_t signal_idx);

/**
  * @brief Set peripheral output to an GPIO pad through the IOMUX.
  *
  * @param hal Context of the HAL layer
  * @param gpio_num GPIO number of the pad.
  * @param func The index number of the IOMUX function to be selected for the pin.
  *        One of the ``FUNC_X_*`` of specified pin (X) in ``soc/io_mux_reg.h``.
  */
void gpio_hal_iomux_out(gpio_hal_context_t *hal, uint32_t gpio_num, int func);

/**
 * @brief Set pad input to a peripheral signal through the GPIO matrix.
 *
 * @note There's no limitation on the number of signals that a GPIO can combine with.
 *
 * @param hal Context of the HAL layer
 * @param gpio_num GPIO number, especially, `GPIO_MATRIX_CONST_ZERO_INPUT` means connect logic 0 to signal;
 *                                          `GPIO_MATRIX_CONST_ONE_INPUT` means connect logic 1 to signal.
 * @param signal_idx Peripheral signal index (tagged as input attribute). One of the ``*_IN_IDX`` signals in ``soc/gpio_sig_map.h``.
 * @param in_inv Whether the GPIO input to be inverted or not.
 */
void gpio_hal_matrix_in(gpio_hal_context_t *hal, uint32_t gpio_num, uint32_t signal_idx, bool in_inv);

/**
 * @brief Set peripheral output to an GPIO pad through the GPIO matrix.
 *
 * @note There's no limitation on the number of signals that a GPIO can combine with.
 *
 * @param hal Context of the HAL layer
 * @param gpio_num GPIO number
 * @param signal_idx Peripheral signal index (tagged as output attribute). One of the ``*_OUT_IDX`` signals in ``soc/gpio_sig_map.h``. Particularly, `SIG_GPIO_OUT_IDX` means disconnect GPIO and other peripherals. Only the GPIO driver can control the output level.
 * @param out_inv Whether to signal to be inverted or not.
 * @param oen_inv Whether the output enable control is inverted or not.
 */
void gpio_hal_matrix_out(gpio_hal_context_t *hal, uint32_t gpio_num, uint32_t signal_idx, bool out_inv, bool oen_inv);

#if SOC_GPIO_SUPPORT_FORCE_HOLD
/**
  * @brief Force hold all digital gpio pads (including those powered by VDD3P3_RTC power domain).
  * @note GPIO force hold, whether the chip in sleep mode or wakeup mode.
  */
#define gpio_hal_force_hold_all() gpio_ll_force_hold_all()

/**
  * @brief Force unhold all digital gpio pads (including those powered by VDD3P3_RTC power domain).
  * @note GPIO force unhold, whether the chip in sleep mode or wakeup mode.
  */
#define gpio_hal_force_unhold_all() gpio_ll_force_unhold_all()
#endif

/**
  * @brief Enable pull-up on GPIO when system sleep.
  *
  * @param hal Context of the HAL layer
  * @param gpio_num GPIO number
  */
#define gpio_hal_sleep_pullup_en(hal, gpio_num) gpio_ll_sleep_pullup_en((hal)->dev, gpio_num)

/**
  * @brief Disable pull-up on GPIO when system sleep.
  *
  * @param hal Context of the HAL layer
  * @param gpio_num GPIO number
  */
#define gpio_hal_sleep_pullup_dis(hal, gpio_num) gpio_ll_sleep_pullup_dis((hal)->dev, gpio_num)

/**
  * @brief Enable pull-down on GPIO when system sleep.
  *
  * @param hal Context of the HAL layer
  * @param gpio_num GPIO number
  */
#define gpio_hal_sleep_pulldown_en(hal, gpio_num) gpio_ll_sleep_pulldown_en((hal)->dev, gpio_num)

/**
  * @brief Disable pull-down on GPIO when system sleep.
  *
  * @param hal Context of the HAL layer
  * @param gpio_num GPIO number
  */
#define gpio_hal_sleep_pulldown_dis(hal, gpio_num) gpio_ll_sleep_pulldown_dis((hal)->dev, gpio_num)

/**
  * @brief Enable sleep select on GPIO.
  *
  * @param hal Context of the HAL layer
  * @param gpio_num GPIO number
  */
#define gpio_hal_sleep_sel_en(hal, gpio_num) gpio_ll_sleep_sel_en((hal)->dev, gpio_num)

/**
  * @brief Disable sleep select on GPIO.
  *
  * @param hal Context of the HAL layer
  * @param gpio_num GPIO number
  */
#define gpio_hal_sleep_sel_dis(hal, gpio_num) gpio_ll_sleep_sel_dis((hal)->dev, gpio_num)

/**
  * @brief Disable input mode on GPIO when system sleep.
  *
  * @param hal Context of the HAL layer
  * @param gpio_num GPIO number
  */
#define gpio_hal_sleep_input_disable(hal, gpio_num) gpio_ll_sleep_input_disable((hal)->dev, gpio_num)

/**
  * @brief Enable input mode on GPIO when system sleep.
  *
  * @param hal Context of the HAL layer
  * @param gpio_num GPIO number
  */
#define gpio_hal_sleep_input_enable(hal, gpio_num) gpio_ll_sleep_input_enable((hal)->dev, gpio_num)

/**
  * @brief Disable output mode on GPIO when system sleep.
  *
  * @param hal Context of the HAL layer
  * @param gpio_num GPIO number
  */
#define gpio_hal_sleep_output_disable(hal, gpio_num) gpio_ll_sleep_output_disable((hal)->dev, gpio_num)

/**
  * @brief Enable output mode on GPIO when system sleep.
  *
  * @param hal Context of the HAL layer
  * @param gpio_num GPIO number
  */
#define gpio_hal_sleep_output_enable(hal, gpio_num) gpio_ll_sleep_output_enable((hal)->dev, gpio_num)

#if SOC_GPIO_SUPPORT_HP_PERIPH_PD_SLEEP_WAKEUP && (SOC_RTCIO_PIN_COUNT == 0)
/**
 * @brief Enable GPIO HP periph powerdown sleep wake-up function.
 *
 * @param hal Context of the HAL layer
 * @param gpio_num GPIO number.
 * @param intr_type GPIO wake-up type. Only GPIO_INTR_LOW_LEVEL or GPIO_INTR_HIGH_LEVEL can be used.
 */
#define gpio_hal_wakeup_enable_on_hp_periph_powerdown_sleep(hal, gpio_num, intr_type) gpio_ll_wakeup_enable_on_hp_periph_powerdown_sleep((hal)->dev, gpio_num, intr_type)

/**
 * @brief Disable GPIO HP periph powerdown sleep wake-up function.
 *
 * @param hal Context of the HAL layer
 * @param gpio_num GPIO number
 */
#define gpio_hal_wakeup_disable_on_hp_periph_powerdown_sleep(hal, gpio_num) gpio_ll_wakeup_disable_on_hp_periph_powerdown_sleep((hal)->dev, gpio_num)

/**
 * @brief Get the status of whether an IO is used for HP periph powerdown sleep wake-up.
 *
 * @param hal Context of the HAL layer
 * @param gpio_num GPIO number
 *
 * @return True if the pin is enabled to wake up from HP periph powerdown sleep
 */
#define gpio_hal_wakeup_is_enabled_on_hp_periph_powerdown_sleep(hal, gpio_num) gpio_ll_hp_periph_powerdown_sleep_wakeup_is_enabled((hal)->dev, gpio_num)
#endif //SOC_GPIO_SUPPORT_HP_PERIPH_PD_SLEEP_WAKEUP && (SOC_RTCIO_PIN_COUNT == 0)

#if SOC_GPIO_SUPPORT_PIN_HYS_FILTER
/**
 * @brief Control gpio hysteresis enable/disable by software.
 *
 * @param hal Context of the HAL layer
 * @param gpio_num GPIO number
 * @param enable enable or disable the hysteresis
 */
void gpio_hal_hysteresis_soft_enable(gpio_hal_context_t *hal, uint32_t gpio_num, bool enable);

#if SOC_GPIO_SUPPORT_PIN_HYS_CTRL_BY_EFUSE
/**
 * @brief Set gpio hysteresis enable/disable by efuse.
 *
 * @param hal Context of the HAL layer
 * @param gpio_num GPIO number
 */
#define gpio_hal_hysteresis_from_efuse(hal, gpio_num) gpio_ll_pin_input_hysteresis_ctrl_sel_efuse((hal)->dev, gpio_num)
#endif
#endif  // SOC_GPIO_SUPPORT_PIN_HYS_FILTER

/**
 * @brief Helper function to disconnect internal circuits from an GPIO in sleep mode.
 *        This function disables input, output, pullup, pulldown for an GPIO in sleep mode.
 *
 * @param hal Context of the HAL layer
 * @param gpio_num GPIO number
 */
void gpio_hal_isolate_in_sleep(gpio_hal_context_t *hal, uint32_t gpio_num);

#ifdef __cplusplus
}
#endif
