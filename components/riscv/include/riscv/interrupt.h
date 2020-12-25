// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
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
  * @brief Set interrupt type, level or edge
  *
  * @param int intr_num, interrupt number
  *
  * @param enum intr_type type, interrupt type, the level interrupt
 can be cleared automatically once the interrupt source cleared, the edge interrupt should be clear by software after handled
  *
  * return none
  */
void esprv_intc_int_set_type(int intr_num, enum intr_type type);

/**
 * Set interrupt priority in the interrupt controller
 * @param rv_int_num  CPU interrupt number
 * @param priority  Interrupt priority level, 1 to 7
 */
void esprv_intc_int_set_priority(int rv_int_num, int priority);

/**
 * Set interrupt priority threshold.
 * Interrupts with priority levels lower than the threshold are masked.
 *
 * @param priority_threshold  Interrupt priority threshold, 0 to 7
 */
void esprv_intc_set_threshold(int priority_threshold);

/**
 * @brief Get interrupt unmask
 * @param none
 * @return uint32_t interrupt unmask
 */
uint32_t esprv_intc_get_interrupt_unmask(void);

#ifdef __cplusplus
}
#endif
