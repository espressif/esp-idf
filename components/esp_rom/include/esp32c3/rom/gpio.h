/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
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

#define GPIO_REG_READ(reg)              READ_PERI_REG(reg)
#define GPIO_REG_WRITE(reg, val)        WRITE_PERI_REG(reg, val)
#define GPIO_ID_PIN0                    0
#define GPIO_ID_PIN(n)                  (GPIO_ID_PIN0+(n))
#define GPIO_PIN_ADDR(i)                (GPIO_PIN0_REG + i*4)

#define GPIO_FUNC_IN_HIGH               0x1E
#define GPIO_FUNC_IN_LOW                0x1F

#define GPIO_ID_IS_PIN_REGISTER(reg_id) \
    ((reg_id >= GPIO_ID_PIN0) && (reg_id <= GPIO_ID_PIN(GPIO_PIN_COUNT-1)))

#define GPIO_REGID_TO_PINIDX(reg_id) ((reg_id) - GPIO_ID_PIN0)

typedef enum {
    GPIO_PIN_INTR_DISABLE = 0,
    GPIO_PIN_INTR_POSEDGE = 1,
    GPIO_PIN_INTR_NEGEDGE = 2,
    GPIO_PIN_INTR_ANYEDGE = 3,
    GPIO_PIN_INTR_LOLEVEL = 4,
    GPIO_PIN_INTR_HILEVEL = 5
} GPIO_INT_TYPE;


/**
  * @brief Change GPIO(0-21) pin output by setting, clearing, or disabling pins, GPIO0<->BIT(0).
  *         There is no particular ordering guaranteed; so if the order of writes is significant,
  *         calling code should divide a single call into multiple calls.
  *
  * @param  uint32_t set_mask : the gpios that need high level.
  *
  * @param  uint32_t clear_mask : the gpios that need low level.
  *
  * @param  uint32_t enable_mask : the gpios that need be changed.
  *
  * @param  uint32_t disable_mask : the gpios that need diable output.
  *
  * @return None
  */
void gpio_output_set(uint32_t set_mask, uint32_t clear_mask, uint32_t enable_mask, uint32_t disable_mask);

/**
  * @brief Sample the value of GPIO input pins(0-21) and returns a bitmask.
  *
  * @param None
  *
  * @return uint32_t : bitmask for GPIO input pins, BIT(0) for GPIO0.
  */
uint32_t gpio_input_get(void);

/**
  * @brief Set GPIO to wakeup the ESP32.
  *        Please do not call this function in SDK.
  *
  * @param uint32_t i: gpio number.
  *
  * @param GPIO_INT_TYPE intr_state : only GPIO_PIN_INTR_LOLEVEL\GPIO_PIN_INTR_HILEVEL can be used
  *
  * @return None
  */
void gpio_pin_wakeup_enable(uint32_t i, GPIO_INT_TYPE intr_state);

/**
  * @brief disable GPIOs to wakeup the ESP32.
  *        Please do not call this function in SDK.
  *
  * @param None
  *
  * @return None
  */
void gpio_pin_wakeup_disable(void);

/**
  * @brief set gpio input to a signal, one gpio can input to several signals.
  *
  * @param uint32_t gpio : gpio number, 0-21
  *                        gpio == 0x1F, input 0 to signal
  *                        gpio == 0x1E, input 1 to signal
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
  * @param uint32_t gpio : gpio number, 0~21
  *
  * @param uint32_t signal_idx : signal index.
  *                        signal_idx == 0x80, cancel output put to the gpio
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
  * @param uint32_t gpio_num : gpio number, 0~21
  *
  * @return None
  */
void gpio_pad_select_gpio(uint32_t gpio_num);

/**
  * @brief Set pad driver capability.
  *
  * @param uint32_t gpio_num : gpio number, 0~21
  *
  * @param uint32_t drv : 0-3
  *
  * @return None
  */
void gpio_pad_set_drv(uint32_t gpio_num, uint32_t drv);

/**
  * @brief Pull up the pad from gpio number.
  *
  * @param uint32_t gpio_num : gpio number, 0~21
  *
  * @return None
  */
void gpio_pad_pullup(uint32_t gpio_num);

/**
  * @brief Pull down the pad from gpio number.
  *
  * @param uint32_t gpio_num : gpio number, 0~21
  *
  * @return None
  */
void gpio_pad_pulldown(uint32_t gpio_num);

/**
  * @brief Unhold the pad from gpio number.
  *
  * @param uint32_t gpio_num : gpio number, 0~21
  *
  * @return None
  */
void gpio_pad_unhold(uint32_t gpio_num);

/**
  * @brief Hold the pad from gpio number.
  *
  * @param uint32_t gpio_num : gpio number, 0~21
  *
  * @return None
  */
void gpio_pad_hold(uint32_t gpio_num);

/**
  * @brief enable gpio pad input.
  *
  * @param uint32_t gpio_num : gpio number, 0~21
  *
  * @return None
  */
void gpio_pad_input_enable(uint32_t gpio_num);

/**
  * @brief disable gpio pad input.
  *
  * @param uint32_t gpio_num : gpio number, 0~21
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
