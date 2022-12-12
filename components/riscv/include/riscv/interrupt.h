/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

enum intr_type {
    INTR_TYPE_LEVEL = 0,
    INTR_TYPE_EDGE
};
/*************************** Software interrupt dispatcher ***************************/

/** Callback type of the interrupt handler */
typedef void (*intr_handler_t)(void*);

/** Set the interrupt handler function for the given CPU interrupt
 * @param rv_int_num  CPU interrupt number
 * @param fn  Handler function
 * @param arg  Handler argument
 */
void intr_handler_set(int rv_int_num, intr_handler_t fn, void* arg);

/** Get the interrupt handler function for the given CPU interrupt
 *
 *@return interrupt handler registered for a particular interrupt number, or NULL otherwise
 */
intr_handler_t intr_handler_get(int rv_int_num);

/** Get the interrupt handler argument associated with the given CPU interrupt
 *
 *@return interrupt handler argument for a particular interrupt number, or NULL otherwise
 */
void *intr_handler_get_arg(int rv_int_num);

/*************************** Interrupt matrix ***************************/

/**
 * this function will be removed in later, please use `intr_matrix_set` instead
 * Route the peripheral interrupt signal to the CPU
 * @param periph_intr_source  Peripheral interrupt number, one of ETS_XXX_SOURCE
 * @param rv_int_num  CPU interrupt number
 */
void intr_matrix_route(int periph_intr_source, int rv_int_num);

/*************************** ESP-RV Interrupt Controller ***************************/

/**
  * @brief Enable interrupts from interrupt controller.
  *
  * @param uint32_t unmask, unmask bits for interrupts, each bit for an interrupt
  *
  * return none
  */
void esprv_intc_int_enable(uint32_t unmask);

/**
  * @brief Disable interrupts from interrupt controller.
  *
  * @param uint32_t mask, mask bits for interrupts, each bit for an interrupt
  *
  * return none
  */
void esprv_intc_int_disable(uint32_t mask);

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
void esprv_intc_int_set_type(int intr_num, enum intr_type type);

/**
 * @brief Get the current type of an interrupt
 *
 * Get the current type of a particular interrupt (level or edge). An interrupt's
 * type can be set by calling esprv_intc_int_set_type().
 *
 * @param intr_num Interrupt number
 * @return Interrupt type
 */
enum intr_type esprv_intc_int_get_type(int intr_num);

/**
 * Set interrupt priority in the interrupt controller
 * @param rv_int_num CPU interrupt number
 * @param priority Interrupt priority level, 1 to 7
 */
void esprv_intc_int_set_priority(int rv_int_num, int priority);

/**
 * @brief Get the current priority of an interrupt
 *
 * Get the current priority of an interrupt.
 *
 * @param rv_int_num CPU interrupt number
 * @return Interrupt priority level, 1 to 7
 */
int esprv_intc_int_get_priority(int rv_int_num);

/**
 * Set interrupt priority threshold.
 * Interrupts with priority levels lower than the threshold are masked.
 *
 * @param priority_threshold  Interrupt priority threshold, 0 to 7
 */
void esprv_intc_int_set_threshold(int priority_threshold);

/**
 * @brief Get interrupt unmask
 * @param none
 * @return uint32_t interrupt unmask
 */
uint32_t esprv_intc_get_interrupt_unmask(void);

#ifdef __cplusplus
}
#endif
