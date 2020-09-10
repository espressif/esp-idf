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
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The HAL layer for GPIO

#pragma once

#include "soc/gpio_periph.h"
#include "hal/gpio_ll.h"
#include "hal/gpio_types.h"

#ifdef CONFIG_LEGACY_INCLUDE_COMMON_HEADERS
#include "soc/rtc_io_reg.h"
#endif

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
    uint32_t version;
} gpio_hal_context_t;

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
  * @brief Clear GPIO interrupt status
  *
  * @param hal Context of the HAL layer
  * @param mask interrupt status clear mask
  */
#define gpio_hal_clear_intr_status(hal, mask) gpio_ll_clear_intr_status((hal)->dev, mask)

/**
  * @brief Clear GPIO interrupt status high
  *
  * @param hal Context of the HAL layer
  * @param mask interrupt status high clear mask
  */
#define gpio_hal_clear_intr_status_high(hal, mask) gpio_ll_clear_intr_status_high((hal)->dev, mask)

/**
 * @brief  Enable GPIO module interrupt signal
 *
 * @param  hal Context of the HAL layer
 * @param  gpio_num GPIO number. If you want to enable the interrupt of e.g. GPIO16, gpio_num should be GPIO_NUM_16 (16);
 * @param  core_id Interrupt enabled CPU to corresponding ID
 */
void gpio_hal_intr_enable_on_core(gpio_hal_context_t *hal, gpio_num_t gpio_num, uint32_t core_id);

/**
 * @brief  Disable GPIO module interrupt signal
 *
 * @param  hal Context of the HAL layer
 * @param  gpio_num GPIO number. If you want to disable the interrupt of e.g. GPIO16, gpio_num should be GPIO_NUM_16 (16);
 */
void gpio_hal_intr_disable(gpio_hal_context_t *hal, gpio_num_t gpio_num);

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
 * @param intr_type GPIO wake-up type. Only GPIO_INTR_LOW_LEVEL or GPIO_INTR_HIGH_LEVEL can be used.
 */
#define gpio_hal_wakeup_enable(hal, gpio_num, intr_type) gpio_ll_wakeup_enable((hal)->dev, gpio_num, intr_type)

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
  * The state of digital gpio cannot be held during Deep-sleep, and it will resume the hold function
  * when the chip wakes up from Deep-sleep. If the digital gpio also needs to be held during Deep-sleep,
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
  *     you should configure gpio18 as output mode and set it to hight level before calling `gpio_hold_dis`.
  *
  * @param hal Context of the HAL layer
  * @param gpio_num GPIO number, only support output GPIOs
  */
#define gpio_hal_hold_dis(hal, gpio_num) gpio_ll_hold_dis((hal)->dev, gpio_num)

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
  * @brief Set pad input to a peripheral signal through the IOMUX.
  *
  * @param hal Context of the HAL layer
  * @param gpio_num GPIO number of the pad.
  * @param signal_idx Peripheral signal id to input. One of the ``*_IN_IDX`` signals in ``soc/gpio_sig_map.h``.
  */
#define gpio_hal_iomux_in(hal, gpio_num, signal_idx) gpio_ll_iomux_in((hal)->dev, gpio_num, signal_idx)

/**
  * @brief Set peripheral output to an GPIO pad through the IOMUX.
  *
  * @param hal Context of the HAL layer
  * @param gpio_num gpio_num GPIO number of the pad.
  * @param func The function number of the peripheral pin to output pin.
  *        One of the ``FUNC_X_*`` of specified pin (X) in ``soc/io_mux_reg.h``.
  * @param oen_inv True if the output enable needs to be inverted, otherwise False.
  */
#define gpio_hal_iomux_out(hal, gpio_num, func, oen_inv) gpio_ll_iomux_out((hal)->dev, gpio_num, func, oen_inv)

#if SOC_GPIO_SUPPORT_FORCE_HOLD
/**
  * @brief Force hold digital and rtc gpio pad.
  * @note GPIO force hold, whether the chip in sleep mode or wakeup mode.
  *
  * @param hal Context of the HAL layer
  * */
#define gpio_hal_force_hold_all(hal) gpio_ll_force_hold_all((hal)->dev)

/**
  * @brief Force unhold digital and rtc gpio pad.
  * @note GPIO force unhold, whether the chip in sleep mode or wakeup mode.
  *
  * @param hal Context of the HAL layer
  * */
#define gpio_hal_force_unhold_all(hal) gpio_ll_force_unhold_all((hal)->dev)
#endif

#ifdef __cplusplus
}
#endif