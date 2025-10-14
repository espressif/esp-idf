/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "esp_attr.h"
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

#define GPIO_REG_READ(reg)              READ_PERI_REG(reg)
#define GPIO_REG_WRITE(reg, val)        WRITE_PERI_REG(reg, val)
#define GPIO_ID_PIN0                    0
#define GPIO_ID_PIN(n)                  (GPIO_ID_PIN0+(n))
#define GPIO_PIN_ADDR(i)                (GPIO_PIN0_REG + i*4)

#define GPIO_FUNC_IN_HIGH               0x3F
#define GPIO_FUNC_IN_LOW                0x3E

#define GPIO_ID_IS_PIN_REGISTER(reg_id) \
    ((reg_id >= GPIO_ID_PIN0) && (reg_id <= GPIO_ID_PIN(GPIO_PIN_COUNT-1)))

#define GPIO_REGID_TO_PINIDX(reg_id) ((reg_id) - GPIO_ID_PIN0)

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
  * @param uint32_t gpio : gpio number, 0~54
  *                        gpio == 0x3E, input 0 to signal
  *                        gpio == 0x3F, input 1 to signal
  *
  * @param uint32_t signal_idx : signal index.
  *
  * @param bool inv : the signal is inv or not
  *
  * @return None
  */
void gpio_matrix_in(uint32_t gpio, uint32_t signal_idx, bool inv);

/**
  * @brief set signal output to gpio, one signal can output to several gpios.
  *
  * @param uint32_t gpio : gpio number, 0~54
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
void gpio_matrix_out(uint32_t gpio, uint32_t signal_idx, bool out_inv, bool oen_inv);

/**
  * @brief Select pad as a gpio function from IOMUX.
  *
  * @param uint32_t gpio_num : gpio number, 0~54
  *
  * @return None
  */
void gpio_pad_select_gpio(uint32_t gpio_num);

/**
  * @brief Set pad driver capability.
  *
  * @param uint32_t gpio_num : gpio number, 0~54
  *
  * @param uint32_t drv : 0-3
  *
  * @return None
  */
void gpio_pad_set_drv(uint32_t gpio_num, uint32_t drv);

/**
  * @brief Pull up the pad from gpio number.
  *
  * @param uint32_t gpio_num : gpio number, 0~54
  *
  * @return None
  */
void gpio_pad_pullup(uint32_t gpio_num);

/**
  * @brief Pull down the pad from gpio number.
  *
  * @param uint32_t gpio_num : gpio number, 0~54
  *
  * @return None
  */
void gpio_pad_pulldown(uint32_t gpio_num);

/**
  * @brief Unhold the pad from gpio number.
  *
  * @param uint32_t gpio_num : gpio number, 0~54
  *
  * @return None
  */
void gpio_pad_unhold(uint32_t gpio_num);

/**
  * @brief Hold the pad from gpio number.
  *
  * @param uint32_t gpio_num : gpio number, 0~54
  *
  * @return None
  */
void gpio_pad_hold(uint32_t gpio_num);

/**
  * @brief enable gpio pad input.
  *
  * @param uint32_t gpio_num : gpio number, 0~54
  *
  * @return None
  */
void gpio_pad_input_enable(uint32_t gpio_num);

/**
  * @brief disable gpio pad input.
  *
  * @param uint32_t gpio_num : gpio number, 0~54
  *
  * @return None
  */
void gpio_pad_input_disable(uint32_t gpio_num);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif
