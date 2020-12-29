// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#include "soc/soc.h"
#include "soc/interrupt_core0_reg.h"
#include "riscv/interrupt.h"
#include "riscv/csr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief enable interrupts specified by the mask
 *
 * @param mask bitmask of interrupts that needs to be enabled
 */
static inline void intr_cntrl_ll_enable_interrupts(uint32_t mask)
{
    unsigned old_mstatus = RV_CLEAR_CSR(mstatus, MSTATUS_MIE);
    esprv_intc_int_enable(mask);
    RV_SET_CSR(mstatus, old_mstatus & MSTATUS_MIE);
}

/**
 * @brief disable interrupts specified by the mask
 *
 * @param mask bitmask of interrupts that needs to be disabled
 */
static inline void intr_cntrl_ll_disable_interrupts(uint32_t mask)
{
    unsigned old_mstatus = RV_CLEAR_CSR(mstatus, MSTATUS_MIE);
    esprv_intc_int_disable(mask);
    RV_SET_CSR(mstatus, old_mstatus & MSTATUS_MIE);
}

/**
 * @brief checks if given interrupt number has a valid handler
 *
 * @param intr interrupt number ranged from 0 to 31
 * @param cpu cpu number ranged betweeen 0 to SOC_CPU_CORES_NUM - 1
 * @return true for valid handler, false otherwise
 */
static inline bool intr_cntrl_ll_has_handler(uint8_t intr, uint8_t cpu)
{
    return intr_handler_get(intr);
}

/**
 * @brief sets interrupt handler and optional argument of a given interrupt number
 *
 * @param intr interrupt number ranged from 0 to 31
 * @param handler handler invoked when an interrupt occurs
 * @param arg optional argument to pass to the handler
 */
static inline void intr_cntrl_ll_set_int_handler(uint8_t intr, interrupt_handler_t handler, void *arg)
{
    intr_handler_set(intr, (void *)handler, arg);
}

/**
 * @brief Gets argument passed to handler of a given interrupt number
 *
 * @param intr interrupt number ranged from 0 to 31
 *
 * @return argument used by handler of passed interrupt number
 */
static inline void *intr_cntrl_ll_get_int_handler_arg(uint8_t intr)
{
    return intr_handler_get_arg(intr);
}

/**
 * @brief Disables interrupts that are not located in iram
 *
 * @param newmask mask of interrupts TO KEEP ENABLED (note: this is probably a bug, see IDF-2308)
 * @return oldmask previous interrupt mask value
 */
static inline uint32_t intr_cntrl_ll_disable_int_mask(uint32_t newmask)
{
    // Disable interrupts in order to atomically update the interrupt enable register
    unsigned old_mstatus = RV_CLEAR_CSR(mstatus, MSTATUS_MIE);

    uint32_t old_int_enable = REG_READ(INTERRUPT_CORE0_CPU_INT_ENABLE_REG);
    REG_WRITE(INTERRUPT_CORE0_CPU_INT_ENABLE_REG, old_int_enable & newmask);

    RV_SET_CSR(mstatus, old_mstatus & MSTATUS_MIE);
    return old_int_enable;
}

/**
 * @brief Enables interrupts that are not located in iram
 *
 * @param newmask mask of interrupts needs to be enabled
 */
static inline void intr_cntrl_ll_enable_int_mask(uint32_t newmask)
{
    unsigned old_mstatus = RV_CLEAR_CSR(mstatus, MSTATUS_MIE);
    esprv_intc_int_enable(newmask);
    RV_SET_CSR(mstatus, old_mstatus & MSTATUS_MIE);
}

/**
 * @brief Acknowledge an edge-trigger interrupt by clearing its pending flag
 *
 * @param intr interrupt number ranged from 0 to 31
 */
static inline void intr_cntrl_ll_edge_int_acknowledge(int intr)
{
    REG_SET_BIT(INTERRUPT_CORE0_CPU_INT_CLEAR_REG, intr);
}

/**
 * @brief Sets the interrupt level int the interrupt controller.
 *
 * @param interrupt_number Interrupt number 0 to 31
 * @param level priority between 1 (lowest) to 7 (highest)
 */
static inline void intr_cntrl_ll_set_int_level(int intr, int level)
{
    esprv_intc_int_set_priority(intr, level);
}

/**
 * @brief Set the type of an interrupt in the controller.
 *
 * @param interrupt_number Interrupt number 0 to 31
 * @param type interrupt type as edge or level triggered
 */
static inline void intr_cntrl_ll_set_int_type(int intr, int_type_t type)
{
    esprv_intc_int_set_type(BIT(intr), type);
}

#ifdef __cplusplus
}
#endif
