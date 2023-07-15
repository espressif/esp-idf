/*
 * SPDX-FileCopyrightText: 2010-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "soc/gpio_pins.h"  //for GPIO_MATRIX_CONST_ONE_INPUT, GPIO_MATRIX_CONST_ZERO_INPUT

/**
 * @brief Configure IO Pad as General Purpose IO,
 *        so that it can be connected to internal Matrix,
 *        then combined with one or more peripheral signals.
 *
 * @param iopad_num IO Pad number
 */
void esp_rom_gpio_pad_select_gpio(uint32_t iopad_num);

/**
 * @brief Enable internal pull up, and disable internal pull down.
 *
 * @param iopad_num IO Pad number
 */
void esp_rom_gpio_pad_pullup_only(uint32_t iopad_num);

/**
 * @brief Unhold the IO Pad.
 * @note When the Pad is set to hold, the state is latched at that moment and won't get changed.
 *
 * @param iopad_num IP Pad number
 */
void esp_rom_gpio_pad_unhold(uint32_t gpio_num);

/**
 * @brief Set IO Pad current drive capability.
 *
 * @param iopad_num IO Pad number
 * @param drv Numeric to indicate the capability of current drive
 *      - 0: 5mA
 *      - 1: 10mA
 *      - 2: 20mA
 *      - 3: 40mA
 */
void esp_rom_gpio_pad_set_drv(uint32_t iopad_num, uint32_t drv);

/**
 * @brief Combine a GPIO input with a peripheral signal, which tagged as input attribute.
 *
 * @note There's no limitation on the number of signals that a GPIO can combine with.
 *
 * @param gpio_num GPIO number, especially, `GPIO_MATRIX_CONST_ZERO_INPUT` means connect logic 0 to signal
 *                                          `GPIO_MATRIX_CONST_ONE_INPUT` means connect logic 1 to signal
 * @param signal_idx Peripheral signal index (tagged as input attribute)
 * @param inv  Whether the GPIO input to be inverted or not
 */
void esp_rom_gpio_connect_in_signal(uint32_t gpio_num, uint32_t signal_idx, bool inv);

/**
 * @brief Combine a peripheral signal which tagged as output attribute with a GPIO.
 *
 * @note There's no limitation on the number of signals that a GPIO can combine with.
 *
 * @param gpio_num GPIO number, especially, `SIG_GPIO_OUT_IDX` means disconnect GPIO and other peripherals. Only the GPIO driver can control the output level.
 * @param signal_idx Peripheral signal index (tagged as output attribute)
 * @param out_inv Whether to signal to be inverted or not
 * @param oen_inv Whether the output enable control is inverted or not
 */
void esp_rom_gpio_connect_out_signal(uint32_t gpio_num, uint32_t signal_idx, bool out_inv, bool oen_inv);

#ifdef __cplusplus
}
#endif
