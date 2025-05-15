/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/gpio_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup gpio_apis, uart configuration and communication related apis
  * @brief gpio apis
  */

/** @addtogroup gpio_apis
  * @{
  */

#define GPIO_REG_READ(reg)       READ_PERI_REG(reg)
#define GPIO_REG_WRITE(reg, val) WRITE_PERI_REG(reg, val)

#define GPIO_OUTPUT_SET(gpio_no, bit_value) gpio_set_output_level(gpio_no, bit_value)
#define GPIO_DIS_OUTPUT(gpio_no)            gpio_output_disable(gpio_no)
#define GPIO_INPUT_GET(gpio_no)             gpio_get_input_level(gpio_no)

/**
  * @brief Set GPIO output level
  *
  * @param gpio_num GPIO number
  * @param level Output level, 0:low; 1:high
  */
void gpio_set_output_level(uint32_t gpio_num, uint32_t level);

/**
  * @brief Get GPIO input level
  *
  * @param gpio_num GPIO number
  *
  * @return 0:the GPIO_input level is low; 1:the GPIO input level is high
  */
uint32_t gpio_get_input_level(uint32_t gpio_num);

/**
  * @brief set gpio input to a signal, one gpio can input to several signals.
  *
  * @param uint32_t gpio_num : gpio number
  *                        gpio == GPIO_MATRIX_CONST_ZERO_INPUT, input 0 to signal
  *                        gpio == GPIO_MATRIX_CONST_ONE_INPUT,  input 1 to signal
  *
  * @param uint32_t signal_idx : signal index.
  *
  * @param bool inv : the signal is inv or not
  *
  * @return None
  */
void gpio_matrix_in(uint32_t gpio_num, uint32_t signal_idx, bool inv);

/**
 * @brief bypass gpio matrix input to signals
 * @param signal_idx  Internal signal index
 */
void gpio_bypass_matrix_in(uint32_t signal_idx);

/**
  * @brief set signal output to gpio, one signal can output to several gpios.
  *
  * @param uint32_t gpio_num : gpio number
  *
  * @param uint32_t signal_idx : signal index.
  *                        signal_idx == 0x100, cancel output put to the gpio
  *
  * @param bool out_inv : the signal output is invert or not
  *
  * @param bool oen_inv : the signal output enable is invert or not
  *
  * @return None
  */
void gpio_matrix_out(uint32_t gpio_num, uint32_t signal_idx, bool out_inv, bool oen_inv);

/**
  * @brief enable gpio output.
  *
  * @param uint32_t gpio_num : gpio number
  *
  * @return None
  */
void gpio_output_enable(uint32_t gpio_num);

/**
  * @brief disable gpio output.
  *
  * @param uint32_t gpio_num : gpio number
  *
  * @return None
  */
void gpio_output_disable(uint32_t gpio_num);

/**
  * @brief Select pad as a gpio function from IOMUX.
  *
  * @param uint32_t gpio_num : gpio number
  *
  * @return None
  */
void gpio_pad_select_gpio(uint32_t gpio_num);

/**
  * @brief Select a mux for the pad(LP pad) in the IOMUX.
  * @note This function is only for LP pad and not all chips have LP pad.
  *
  * @param gpio_num GPIO number should be LP GPIO
  * @param mux_sel Select pad as LP GPIO or HP GPIO, 1:LP GPIO,0:HP GPIO
  */
void gpio_lppad_select_mux(uint32_t gpio_num, uint32_t mux_sel);

/**
  * @brief Set pad driver capability.
  *
  * @param uint32_t gpio_num : gpio number
  *
  * @param uint32_t drv : 0-3
  *
  * @return None
  */
void gpio_pad_set_drv(uint32_t gpio_num, uint32_t drv);

/**
  * @brief Set MSPI dedicated pad driver capability.
  *
  * @param uint32_t gpio_num : MSPI dedicated gpio number
  *
  * @param uint32_t drv : 0-3
  *
  * @return None
  */
void gpio_ded_pad_set_drv(uint32_t ded_gpio_num, uint32_t drv);

/**
  * @brief Pull up the pad from gpio number.
  *
  * @param uint32_t gpio_num : gpio number
  *
  * @return None
  */
void gpio_pad_pullup(uint32_t gpio_num);

/**
  * @brief Pull up the MSPI dedicated pad from gpio number.
  *
  * @param uint32_t gpio_num : MSPI dedicated gpio number
  *
  * @return None
  */
void gpio_ded_pad_pullup(uint32_t ded_gpio_num);

/**
  * @brief Pull down the pad from gpio number.
  *
  * @param uint32_t gpio_num : gpio number
  *
  * @return None
  */
void gpio_pad_pulldown(uint32_t gpio_num);

/**
  * @brief Pull down the MSPI dedicated pad from gpio number.
  *
  * @param uint32_t gpio_num : MSPI dedicated gpio number
  *
  * @return None
  */
void gpio_ded_pad_pulldown(uint32_t ded_gpio_num);

/**
  * @brief enable gpio pad input.
  *
  * @param uint32_t gpio_num : gpio number
  *
  * @return None
  */
void gpio_pad_input_enable(uint32_t gpio_num);

/**
  * @brief disable gpio pad input.
  *
  * @param uint32_t gpio_num : gpio number
  *
  * @return None
  */
void gpio_pad_input_disable(uint32_t gpio_num);

/**
  * @brief Unhold the pad from gpio number.
  *
  * @param uint32_t gpio_num : gpio number
  *
  * @return None
  */
void gpio_pad_unhold(uint32_t gpio_num);

/**
  * @brief Unhold the mspi dedicated pad from gpio number.
  *
  * @param uint32_t gpio_num : mspi dedicated gpio number
  *
  * @return None
  */
void gpio_ded_pad_unhold(uint32_t ded_gpio_num);

/**
  * @brief Hold the pad from gpio number.
  *
  * @param uint32_t gpio_num : gpio number
  *
  * @return None
  */
void gpio_pad_hold(uint32_t gpio_num);

/**
  * @brief Hold the mspi dedicated pad from gpio number.
  *
  * @param uint32_t gpio_num : mspi dedicated gpio number
  *
  * @return None
  */
void gpio_ded_pad_hold(uint32_t gpio_num);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif
