/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
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
#include "riscv/csr_pie.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_CPU_HAS_CSR_PC
/*performance counter*/
#define CSR_PCER_MACHINE    0x7e0
#define CSR_PCMR_MACHINE    0x7e1
#define CSR_PCCR_MACHINE    0x7e2
#endif /* SOC_CPU_HAS_CSR_PC */

#if SOC_BRANCH_PREDICTOR_SUPPORTED
#define MHCR 0x7c1
#define MHCR_RS (1<<4)   /* R/W, address return stack set bit */
#define MHCR_BFE (1<<5)  /* R/W, allow predictive jump set bit */
#define MHCR_BTB (1<<12) /* R/W, branch target prediction enable bit */
#endif  //SOC_BRANCH_PREDICTOR_SUPPORTED

#if SOC_CPU_HAS_FPU

/* FPU bits in mstatus start at bit 13 */
#define CSR_MSTATUS_FPU_SHIFT       13
/* FPU registers are clean if bits are 0b10 */
#define CSR_MSTATUS_FPU_CLEAN_STATE 2
/* FPU status in mstatus are represented with two bits */
#define CSR_MSTATUS_FPU_MASK        3
/* FPU is enabled when writing 1 to FPU bits */
#define CSR_MSTATUS_FPU_ENA         BIT(13)
/* Set FPU registers state to clean (after being dirty) */
#define CSR_MSTATUS_FPU_CLEAR       BIT(13)

#endif /* SOC_CPU_HAS_FPU */

/* SW defined level which the interrupt module will mask interrupt with priority less than threshold during critical sections
   and spinlocks */
#define RVHAL_EXCM_LEVEL    4

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
#if !SOC_CPU_HAS_CSR_PC
    return RV_READ_CSR(mcycle);
#else
    return RV_READ_CSR(CSR_PCCR_MACHINE);
#endif
}

FORCE_INLINE_ATTR void __attribute__((always_inline)) rv_utils_set_cycle_count(uint32_t ccount)
{
#if !SOC_CPU_HAS_CSR_PC
    RV_WRITE_CSR(mcycle, ccount);
#else
    RV_WRITE_CSR(CSR_PCCR_MACHINE, ccount);
#endif
}

/* ------------------------------------------------- CPU Interrupts ----------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

// --------------- Interrupt Configuration -----------------

FORCE_INLINE_ATTR void rv_utils_set_mtvec(uint32_t mtvec_val)
{
    RV_WRITE_CSR(mtvec, mtvec_val | MTVEC_MODE_CSR);
}

// ------------------ Interrupt Control --------------------

FORCE_INLINE_ATTR void rv_utils_intr_enable(uint32_t intr_mask)
{
    // Disable all interrupts to make updating of the interrupt mask atomic.
    unsigned old_mstatus = RV_CLEAR_CSR(mstatus, MSTATUS_MIE);
    esprv_int_enable(intr_mask);
    RV_SET_CSR(mstatus, old_mstatus & MSTATUS_MIE);
}

FORCE_INLINE_ATTR void rv_utils_intr_disable(uint32_t intr_mask)
{
    // Disable all interrupts to make updating of the interrupt mask atomic.
    unsigned old_mstatus = RV_CLEAR_CSR(mstatus, MSTATUS_MIE);
    esprv_int_disable(intr_mask);
    RV_SET_CSR(mstatus, old_mstatus & MSTATUS_MIE);
}

FORCE_INLINE_ATTR void rv_utils_intr_global_enable(void)
{
    RV_SET_CSR(mstatus, MSTATUS_MIE);
}

FORCE_INLINE_ATTR void rv_utils_intr_global_disable(void)
{
    RV_CLEAR_CSR(mstatus, MSTATUS_MIE);
}

/**
 * The other rv_utils functions related to each interrupt controller are defined in `interrupt_clic.h`, `interrupt_plic.h`,
 * and `interrupt_intc.h`.
 */

/* ------------------------------------------------- FPU Related ----------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

#if SOC_CPU_HAS_FPU

FORCE_INLINE_ATTR bool rv_utils_enable_fpu(void)
{
    /* Set mstatus[14:13] to 0b01 to start the floating-point unit initialization */
    RV_SET_CSR(mstatus, CSR_MSTATUS_FPU_ENA);
    /* On the ESP32-P4, the FPU can be used directly after setting `mstatus` bit 13.
     * Since the interrupt handler expects the FPU states to be either 0b10 or 0b11,
     * let's write the FPU CSR and clear the dirty bit afterwards. */
    RV_WRITE_CSR(fcsr, 1);
    RV_CLEAR_CSR(mstatus, CSR_MSTATUS_FPU_CLEAR);
    const uint32_t mstatus = RV_READ_CSR(mstatus);
    /* Make sure the FPU state is 0b10 (clean registers) */
    return ((mstatus >> CSR_MSTATUS_FPU_SHIFT) & CSR_MSTATUS_FPU_MASK) == CSR_MSTATUS_FPU_CLEAN_STATE;
}


FORCE_INLINE_ATTR void rv_utils_disable_fpu(void)
{
    /* Clear mstatus[14:13] bits to disable the floating-point unit */
    RV_CLEAR_CSR(mstatus, CSR_MSTATUS_FPU_MASK << CSR_MSTATUS_FPU_SHIFT);
}

#endif /* SOC_CPU_HAS_FPU */


/* ------------------------------------------------- PIE Related ----------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

#if SOC_CPU_HAS_PIE

FORCE_INLINE_ATTR void rv_utils_enable_pie(void)
{
    RV_WRITE_CSR(CSR_PIE_STATE_REG, 1);
}


FORCE_INLINE_ATTR void rv_utils_disable_pie(void)
{
    RV_WRITE_CSR(CSR_PIE_STATE_REG, 0);
}

#endif /* SOC_CPU_HAS_FPU */



/* -------------------------------------------------- Memory Ports -----------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

#if SOC_ASYNCHRONOUS_BUS_ERROR_MODE

FORCE_INLINE_ATTR uintptr_t rv_utils_asynchronous_bus_get_error_pc(void)
{
    uint32_t error_pc;
    uint32_t mcause, mexstatus;

    mexstatus = RV_READ_CSR(MEXSTATUS);
    /* MEXSTATUS: Bit 8: Indicates that a load/store access fault (MCAUSE=5/7)
     * is due to bus-error exception. If this bit is not cleared before exiting
     * the exception handler, it will trigger a bus error again.
     * Since we have not mechanisms to recover a normal program execution after
     * load/store error appears, do nothing. */
    if ((mexstatus & BIT(8)) == 0) {
        return 0;
    }
    mcause = RV_READ_CSR(mcause) & 0xFF;
    if (mcause == 5) { /* Load access fault */
        /* Get the oldest PC at which the load instruction failed */
        error_pc = RV_READ_CSR(LDPC1);
        if (error_pc == 0) {
            error_pc = RV_READ_CSR(LDPC0);
        }
    } else if (mcause == 7) { /* Store access fault */
        /* Get the oldest PC at which the store instruction failed */
        error_pc = RV_READ_CSR(STPC2);
        if (error_pc == 0) {
            error_pc = RV_READ_CSR(STPC1);
            if (error_pc == 0) {
                error_pc = RV_READ_CSR(STPC0);
            }
        }
    } else {
        return 0;
    }
    /* Bit 0: Valid bit indicating that this CSR holds the PC (program counter).
     * Clear this bit */
    return error_pc & ~(1);
}

#endif // SOC_ASYNCHRONOUS_BUS_ERROR_MODE

/* ---------------------------------------------------- Debugging ------------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

// --------------- Breakpoints/Watchpoints -----------------

FORCE_INLINE_ATTR void rv_utils_set_breakpoint(int bp_num, uint32_t bp_addr)
{
    /* The code below sets breakpoint which will trigger `Breakpoint` exception
     * instead transferring control to debugger. */
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
                         ((size == 1) ? TDATA1_MATCH_EXACT : TDATA1_MATCH_NAPOT) |
                         (on_read  ? TDATA1_LOAD  : 0) |
                         (on_write ? TDATA1_STORE : 0));
    /* From RISC-V Debug Specification:
     * tdata1(mcontrol) match = 0 : Exact byte match
     *
     * tdata1(mcontrol) match = 1 : NAPOT (Naturally Aligned Power-Of-Two):
     * Matches when the top M bits of any compare value match the top M bits of tdata2.
     * M is XLEN − 1 minus the index of the least-significant bit containing 0 in tdata2.
     * Note: Expecting that size is number power of 2 (numbers should be in the range of 1 ~ 31)
     *
     * Examples for understanding how to calculate match pattern to tdata2:
     *
     * nnnn...nnnnn 1-byte  Exact byte match
     * nnnn...nnnn0 2-byte  NAPOT range
     * nnnn...nnn01 4-byte  NAPOT range
     * nnnn...nn011 8-byte  NAPOT range
     * nnnn...n0111 16-byte NAPOT range
     * nnnn...01111 32-byte NAPOT range
     * ...
     * n011...11111 2^31 byte NAPOT range
     *  * where n are bits from original address
     */
    uint32_t match_pattern = (wp_addr & ~(size-1)) | ((size-1) >> 1);

    RV_WRITE_CSR(tdata2, match_pattern);
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
#if __riscv_atomic
    uint32_t old_value = 0;
    int error = 0;

    /* Based on sample code for CAS from RISCV specs v2.2, atomic instructions */
    __asm__ __volatile__(
        "cas: lr.w %0, 0(%2)     \n"                        // load 4 bytes from addr (%2) into old_value (%0)
        "     bne  %0, %3, fail  \n"                        // fail if old_value if not equal to compare_value (%3)
        "     sc.w %1, %4, 0(%2) \n"                        // store new_value (%4) into addr,
        "     bnez %1, cas       \n"                        // if we failed to store the new value then retry the operation
        "fail:                   \n"
        : "+r" (old_value), "+r" (error)                    // output parameters
        : "r" (addr), "r" (compare_value), "r" (new_value)  // input parameters
    );
#else
    // For a single core RV target has no atomic CAS instruction, we can achieve atomicity by disabling interrupts
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

#endif //__riscv_atomic
    return (old_value == compare_value);
}

#if SOC_BRANCH_PREDICTOR_SUPPORTED
FORCE_INLINE_ATTR void rv_utils_en_branch_predictor(void)
{
    RV_SET_CSR(MHCR, MHCR_RS|MHCR_BFE|MHCR_BTB);
}

FORCE_INLINE_ATTR void rv_utils_dis_branch_predictor(void)
{
    RV_CLEAR_CSR(MHCR, MHCR_RS|MHCR_BFE|MHCR_BTB);
}
#endif

#ifdef __cplusplus
}
#endif
