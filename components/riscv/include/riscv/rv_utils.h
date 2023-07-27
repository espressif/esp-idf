/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
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

//TODO: IDF-7771
#define RVHAL_EXCM_LEVEL    4

/* --------------------------------------------------- CPU Control -----------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

FORCE_INLINE_ATTR void __attribute__((always_inline)) rv_utils_wait_for_intr(void)
{
    asm volatile ("wfi\n");
}

/* ------------------------------------------------- Memory Barrier ----------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */
//TODO: IDF-7898
FORCE_INLINE_ATTR void rv_utils_memory_barrier(void)
{
    asm volatile("fence iorw, iorw" : : : "memory");
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
#if SOC_INT_CLIC_SUPPORTED
    //TODO: IDF-7848
    return RV_READ_CSR(mcycle);
#else
    return RV_READ_CSR(CSR_PCCR_MACHINE);
#endif
}

FORCE_INLINE_ATTR void __attribute__((always_inline)) rv_utils_set_cycle_count(uint32_t ccount)
{
#if SOC_INT_CLIC_SUPPORTED
    //TODO: IDF-7848
    RV_WRITE_CSR(mcycle, ccount);
#else
    RV_WRITE_CSR(CSR_PCCR_MACHINE, ccount);
#endif
}

/* ------------------------------------------------- CPU Interrupts ----------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

// ---------------- Interrupt Descriptors ------------------

// --------------- Interrupt Configuration -----------------
#if SOC_INT_CLIC_SUPPORTED
//TODO: IDF-7863
FORCE_INLINE_ATTR void rv_utils_set_mtvt(uint32_t mtvt_val)
{
#define MTVT 0x307
    RV_WRITE_CSR(MTVT, mtvt_val);
}
#endif

FORCE_INLINE_ATTR void rv_utils_set_mtvec(uint32_t mtvec_val)
{
#if SOC_INT_CLIC_SUPPORTED
    //TODO: IDF-7863
    mtvec_val |= 3;
#else
    mtvec_val |= 1; // Set MODE field to treat MTVEC as a vector base address
#endif
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

//TODO: IDF-7795, clic related
#if (SOC_CPU_CORES_NUM > 1)
FORCE_INLINE_ATTR void __attribute__((always_inline)) rv_utils_restore_intlevel(uint32_t restoreval)
{
    REG_SET_FIELD(CLIC_INT_THRESH_REG, CLIC_CPU_INT_THRESH, ((restoreval << (8 - NLBITS))) | 0x1f);
}

FORCE_INLINE_ATTR uint32_t __attribute__((always_inline)) rv_utils_set_intlevel(uint32_t intlevel)
{
    uint32_t old_mstatus = RV_CLEAR_CSR(mstatus, MSTATUS_MIE);
    uint32_t old_thresh;

    old_thresh = REG_READ(CLIC_INT_THRESH_REG);
    old_thresh = old_thresh >> (24 + (8 - NLBITS));

    REG_SET_FIELD(CLIC_INT_THRESH_REG, CLIC_CPU_INT_THRESH, ((intlevel << (8 - NLBITS))) | 0x1f);
    /**
     * TODO: IDF-7898
     * Here is an issue that,
     * 1. Set the CLIC_INT_THRESH_REG to mask off interrupts whose level is lower than `intlevel`.
     * 2. Set MSTATUS_MIE (global interrupt), then program may jump to interrupt vector.
     * 3. The register value change in Step 1 may happen during Step 2.
     *
     * To prevent this, here a fence is used
     */
    rv_utils_memory_barrier();
    RV_SET_CSR(mstatus, old_mstatus & MSTATUS_MIE);

    return old_thresh;
}
#endif  //#if (SOC_CPU_CORES_NUM > 1)

FORCE_INLINE_ATTR uint32_t rv_utils_intr_get_enabled_mask(void)
{
//TODO: IDF-7795
#if SOC_INT_CLIC_SUPPORTED
    unsigned intr_ena_mask = 0;
    unsigned intr_num;
    for (intr_num = 0; intr_num < 32; intr_num++) {
        if (REG_GET_BIT(CLIC_INT_CTRL_REG(intr_num + CLIC_EXT_INTR_NUM_OFFSET), CLIC_INT_IE))
            intr_ena_mask |= BIT(intr_num);
    }
    return intr_ena_mask;
#else
    return REG_READ(INTERRUPT_CORE0_CPU_INT_ENABLE_REG);
#endif
}

FORCE_INLINE_ATTR void rv_utils_intr_edge_ack(unsigned int intr_num)
{
//TODO: IDF-7795
#if SOC_INT_CLIC_SUPPORTED
    REG_SET_BIT(CLIC_INT_CTRL_REG(intr_num + CLIC_EXT_INTR_NUM_OFFSET) , CLIC_INT_IP);
#else
    REG_SET_BIT(INTERRUPT_CORE0_CPU_INT_CLEAR_REG, intr_num);
#endif
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
    RV_WRITE_CSR(tcontrol, TCONTROL_MPTE | TCONTROL_MTE);
    RV_WRITE_CSR(tdata1, TDATA1_USER | TDATA1_MACHINE | TDATA1_EXECUTE);
    RV_WRITE_CSR(tdata2, bp_addr);
}

FORCE_INLINE_ATTR void rv_utils_set_watchpoint(int wp_num,
                                               uint32_t wp_addr,
                                               size_t size,
                                               bool on_read,
                                               bool on_write)
{
    RV_WRITE_CSR(tselect, wp_num);
    RV_WRITE_CSR(tcontrol, TCONTROL_MPTE | TCONTROL_MTE);
    RV_WRITE_CSR(tdata1, TDATA1_USER                   |
                         TDATA1_MACHINE                |
                         TDATA1_MATCH                  |
                         (on_read  ? TDATA1_LOAD  : 0) |
                         (on_write ? TDATA1_STORE : 0));
    /* From RISC-V Debug Specification:
     * NAPOT (Naturally Aligned Power-Of-Two):
     * Matches when the top M bits of any compare value match the top M bits of tdata2.
     * M is XLEN − 1 minus the index of the least-significant bit containing 0 in tdata2.
     *
     * Note: Expectng that size is number power of 2
     *
     * Examples for understanding how to calculate NAPOT:
     *
     * nnnn...nnnn0 2-byte  NAPOT range
     * nnnn...nnn01 4-byte  NAPOT range
     * nnnn...nn011 8-byte  NAPOT range
     * nnnn...n0111 16-byte NAPOT range
     * nnnn...01111 32-byte NAPOT range
     *  * where n are bits from original address
     */
    const uint32_t half_size = size >> 1;
    uint32_t napot = wp_addr;
    napot &= ~half_size;      /* set the least-significant bit with zero */
    napot |= half_size - 1;   /* fill all bits with ones after least-significant bit */
    RV_WRITE_CSR(tdata2, napot);
}

FORCE_INLINE_ATTR void rv_utils_clear_breakpoint(int bp_num)
{
    RV_WRITE_CSR(tselect, bp_num);
    /* tdata1 is a WARL(write any read legal) register
     * We can just write 0 to it
     */
    RV_WRITE_CSR(tdata1, 0);
}

FORCE_INLINE_ATTR void rv_utils_clear_watchpoint(int wp_num)
{
    /* riscv have the same registers for breakpoints and watchpoints */
    rv_utils_clear_breakpoint(wp_num);
}

FORCE_INLINE_ATTR bool rv_utils_is_trigger_fired(int id)
{
    RV_WRITE_CSR(tselect, id);
    return (RV_READ_CSR(tdata1) >> TDATA1_HIT_S) & 1;
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

#if SOC_BRANCH_PREDICTOR_SUPPORTED
FORCE_INLINE_ATTR void rv_utils_en_branch_predictor(void)
{
#define MHCR 0x7c1
#define MHCR_RS (1<<4)   /* R/W, address return stack set bit */
#define MHCR_BFE (1<<5)  /* R/W, allow predictive jump set bit */
#define MHCR_BTB (1<<12) /* R/W, branch target prediction enable bit */
    RV_SET_CSR(MHCR, MHCR_RS|MHCR_BFE|MHCR_BTB);
}
#endif

#ifdef __cplusplus
}
#endif
