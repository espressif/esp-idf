/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#include "soc/soc_caps.h"
#include "soc/assist_debug_reg.h"
#include "soc/interrupt_reg.h"
#include "esp_attr.h"
#include "riscv/csr.h"
#include "riscv/interrupt.h"

#ifdef __cplusplus
extern "C" {
#endif

/*performance counter*/
#define CSR_PCER_MACHINE    0x7e0
#define CSR_PCMR_MACHINE    0x7e1
#define CSR_PCCR_MACHINE    0x7e2

/* --------------------------------------------------- CPU Control -----------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

FORCE_INLINE_ATTR void __attribute__((always_inline)) rv_utils_wait_for_intr(void)
{
    asm volatile ("wfi\n");
}

/* -------------------------------------------------- CPU Registers ----------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

FORCE_INLINE_ATTR __attribute__((pure)) uint32_t rv_utils_get_core_id(void)
{
#if SOC_CPU_CORES_NUM == 1
    return 0; // No need to check core ID on single core hardware
#else
    uint32_t cpuid;
    cpuid = RV_READ_CSR(mhartid);
    return cpuid;
#endif
}

FORCE_INLINE_ATTR void *rv_utils_get_sp(void)
{
    void *sp;
    asm volatile ("mv %0, sp;" : "=r" (sp));
    return sp;
}

FORCE_INLINE_ATTR uint32_t __attribute__((always_inline)) rv_utils_get_cycle_count(void)
{
    return RV_READ_CSR(CSR_PCCR_MACHINE);
}

FORCE_INLINE_ATTR void __attribute__((always_inline)) rv_utils_set_cycle_count(uint32_t ccount)
{
    RV_WRITE_CSR(CSR_PCCR_MACHINE, ccount);
}

/* ------------------------------------------------- CPU Interrupts ----------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

// ---------------- Interrupt Descriptors ------------------

// --------------- Interrupt Configuration -----------------

FORCE_INLINE_ATTR void rv_utils_set_mtvec(uint32_t mtvec_val)
{
    mtvec_val |= 1; // Set MODE field to treat MTVEC as a vector base address
    RV_WRITE_CSR(mtvec, mtvec_val);
}

// ------------------ Interrupt Control --------------------

FORCE_INLINE_ATTR void rv_utils_intr_enable(uint32_t intr_mask)
{
    // Disable all interrupts to make updating of the interrupt mask atomic.
    unsigned old_mstatus = RV_CLEAR_CSR(mstatus, MSTATUS_MIE);
    esprv_intc_int_enable(intr_mask);
    RV_SET_CSR(mstatus, old_mstatus & MSTATUS_MIE);
}

FORCE_INLINE_ATTR void rv_utils_intr_disable(uint32_t intr_mask)
{
    // Disable all interrupts to make updating of the interrupt mask atomic.
    unsigned old_mstatus = RV_CLEAR_CSR(mstatus, MSTATUS_MIE);
    esprv_intc_int_disable(intr_mask);
    RV_SET_CSR(mstatus, old_mstatus & MSTATUS_MIE);
}

FORCE_INLINE_ATTR uint32_t rv_utils_intr_get_enabled_mask(void)
{
    return REG_READ(INTERRUPT_CORE0_CPU_INT_ENABLE_REG);
}

FORCE_INLINE_ATTR void rv_utils_intr_edge_ack(int intr_num)
{
    REG_SET_BIT(INTERRUPT_CORE0_CPU_INT_CLEAR_REG, intr_num);
}

FORCE_INLINE_ATTR void rv_utils_intr_global_enable(void)
{
    RV_SET_CSR(mstatus, MSTATUS_MIE);
}

FORCE_INLINE_ATTR void rv_utils_intr_global_disable(void)
{
    RV_CLEAR_CSR(mstatus, MSTATUS_MIE);
}

/* -------------------------------------------------- Memory Ports -----------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

/* ---------------------------------------------------- Debugging ------------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

// --------------- Breakpoints/Watchpoints -----------------

FORCE_INLINE_ATTR void rv_utils_set_breakpoint(int bp_num, uint32_t bp_addr)
{
    /* The code bellow sets breakpoint which will trigger `Breakpoint` exception
     * instead transfering control to debugger. */
    RV_WRITE_CSR(tselect, bp_num);
    RV_SET_CSR(CSR_TCONTROL, TCONTROL_MTE);
    RV_SET_CSR(CSR_TDATA1, TDATA1_USER | TDATA1_MACHINE | TDATA1_EXECUTE);
    RV_WRITE_CSR(tdata2, bp_addr);
}

FORCE_INLINE_ATTR void rv_utils_clear_breakpoint(int bp_num)
{
    RV_WRITE_CSR(tselect, bp_num);
    RV_CLEAR_CSR(CSR_TCONTROL, TCONTROL_MTE);
    RV_CLEAR_CSR(CSR_TDATA1, TDATA1_USER | TDATA1_MACHINE | TDATA1_EXECUTE);
}

FORCE_INLINE_ATTR void rv_utils_set_watchpoint(int wp_num,
                                           uint32_t wp_addr,
                                           size_t size,
                                           bool on_read,
                                           bool on_write)
{
    RV_WRITE_CSR(tselect, wp_num);
    RV_SET_CSR(CSR_TCONTROL, TCONTROL_MPTE | TCONTROL_MTE);
    RV_SET_CSR(CSR_TDATA1, TDATA1_USER | TDATA1_MACHINE);
    RV_SET_CSR_FIELD(CSR_TDATA1, (long unsigned int) TDATA1_MATCH, 1);

    // add 0 in napot encoding
    uint32_t addr_napot;
    addr_napot = ((uint32_t) wp_addr) | ((size >> 1) - 1);
    if (on_read) {
        RV_SET_CSR(CSR_TDATA1, TDATA1_LOAD);
    }
    if (on_write) {
        RV_SET_CSR(CSR_TDATA1, TDATA1_STORE);
    }
    RV_WRITE_CSR(tdata2, addr_napot);
}

FORCE_INLINE_ATTR void rv_utils_clear_watchpoint(int wp_num)
{
    RV_WRITE_CSR(tselect, wp_num);
    RV_CLEAR_CSR(CSR_TCONTROL, TCONTROL_MTE);
    RV_CLEAR_CSR(CSR_TDATA1, TDATA1_USER | TDATA1_MACHINE);
    RV_CLEAR_CSR_FIELD(CSR_TDATA1, (long unsigned int) TDATA1_MATCH);
    RV_CLEAR_CSR(CSR_TDATA1, TDATA1_MACHINE);
    RV_CLEAR_CSR(CSR_TDATA1, TDATA1_LOAD | TDATA1_STORE | TDATA1_EXECUTE);
}

// ---------------------- Debugger -------------------------

FORCE_INLINE_ATTR bool rv_utils_dbgr_is_attached(void)
{
    return REG_GET_BIT(ASSIST_DEBUG_CORE_0_DEBUG_MODE_REG, ASSIST_DEBUG_CORE_0_DEBUG_MODULE_ACTIVE);
}

FORCE_INLINE_ATTR void rv_utils_dbgr_break(void)
{
    asm volatile("ebreak\n");
}

/* ------------------------------------------------------ Misc ---------------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

FORCE_INLINE_ATTR bool rv_utils_compare_and_set(volatile uint32_t *addr, uint32_t compare_value, uint32_t new_value)
{
    // ESP32C6 starts to support atomic CAS instructions, but it is still a single core target, no need to implement
    // through lr and sc instructions for now
    // For an RV target has no atomic CAS instruction, we can achieve atomicity by disabling interrupts
    unsigned old_mstatus;
    old_mstatus = RV_CLEAR_CSR(mstatus, MSTATUS_MIE);
    // Compare and set
    uint32_t old_value;
    old_value = *addr;
    if (old_value == compare_value) {
        *addr = new_value;
    }
    // Restore interrupts
    RV_SET_CSR(mstatus, old_mstatus & MSTATUS_MIE);

    return (old_value == compare_value);
}

#ifdef __cplusplus
}
#endif
