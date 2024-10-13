/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "riscv/interrupt.h"

#ifdef __cplusplus
extern "C" {
#endif


/*************************** Former API / Backport compatibility ***************************/


/**
  * @brief Enable interrupts from interrupt controller.
  *
  * @param uint32_t unmask, unmask bits for interrupts, each bit for an interrupt
  *
  * return none
  */
void esprv_intc_int_enable(uint32_t unmask) __attribute__((deprecated("please use esprv_int_enable instead")));

/**
  * @brief Disable interrupts from interrupt controller.
  *
  * @param uint32_t mask, mask bits for interrupts, each bit for an interrupt
  *
  * return none
  */
void esprv_intc_int_disable(uint32_t mask) __attribute__((deprecated("please use esprv_int_disable instead")));

/**
 * @brief Set interrupt type
 *
 * Set the type of a particular interrupt (level or edge).
 * - Level interrupts are cleared automatically once their interrupt source has
 *   been cleared
 * - Edge interrupts must be cleared by software when they are handled.
 *
 * @param intr_num Interrupt number
 * @param type Interrupt type
 */
void esprv_intc_int_set_type(int intr_num, enum intr_type type) __attribute__((deprecated("please use esprv_int_set_type instead")));

/**
 * @brief Get the current type of an interrupt
 *
 * Get the current type of a particular interrupt (level or edge). An interrupt's
 * type can be set by calling esprv_intc_int_set_type().
 *
 * @param intr_num Interrupt number
 * @return Interrupt type
 */
static inline __attribute__((deprecated("please use esprv_int_get_type instead"))) enum intr_type esprv_intc_int_get_type(int intr_num)
{
    return esprv_int_get_type(intr_num);
}

/**
 * Set interrupt priority in the interrupt controller
 * @param rv_int_num CPU interrupt number
 * @param priority Interrupt priority level, 1 to 7
 */
void esprv_intc_int_set_priority(int rv_int_num, int priority) __attribute__((deprecated("please use esprv_int_set_priority instead")));

/**
 * @brief Get the current priority of an interrupt
 *
 * Get the current priority of an interrupt.
 *
 * @param rv_int_num CPU interrupt number
 * @return Interrupt priority level, 1 to 7
 */
static inline __attribute__((deprecated("please use esprv_int_get_priority instead"))) int esprv_intc_int_get_priority(int rv_int_num)
{
    return esprv_int_get_priority(rv_int_num);
}

/**
 * Set interrupt priority threshold.
 * Interrupts with priority levels lower than the threshold are masked.
 *
 * @param priority_threshold  Interrupt priority threshold, 0 to 7
 */
void esprv_intc_int_set_threshold(int priority_threshold) __attribute__((deprecated("please use esprv_int_set_threshold instead")));

/**
 * @brief Get interrupt unmask
 * @param none
 * @return uint32_t interrupt unmask
 */
static inline __attribute__((deprecated("please use esprv_get_interrupt_unmask instead"))) uint32_t esprv_intc_get_interrupt_unmask(void)
{
    return esprv_get_interrupt_unmask();
}

/**
 * @brief Route the peripheral interrupt signal to the CPU
 * @param periph_intr_source  Peripheral interrupt number, one of ETS_XXX_SOURCE
 * @param rv_int_num  CPU interrupt number
 */
void intr_matrix_route(int periph_intr_source, int rv_int_num) __attribute__((deprecated("please use esp_rom_route_intr_matrix instead")));

#ifdef __cplusplus
}
#endif
