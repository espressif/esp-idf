/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc.h"
#include "rvsleep-frames.h"
#include "soc/soc_caps.h"
#include "sdkconfig.h"

    .section    .data1,"aw"
    .global     rv_core_critical_regs_frame
    .type       rv_core_critical_regs_frame,@object
    .align      4
rv_core_critical_regs_frame:
    .word       0

/*
--------------------------------------------------------------------------------
    This assembly subroutine is used to save the critical registers of the CPU
    core to the internal RAM before sleep, and modify the PMU control flag to
    indicate that the system needs to sleep. When the subroutine returns, it
    will return the memory pointer that saves the context information of the CPU
    critical registers.
--------------------------------------------------------------------------------
*/

    .section    .iram1,"ax"
    .global     rv_core_critical_regs_save
    .type       rv_core_critical_regs_save,@function
    .align      4

rv_core_critical_regs_save:

    /* arrived here in critical section. we need:
       save riscv core critical registers to RvCoreCriticalSleepFrame
     */
    csrw    mscratch, t0        /* use mscratch as temp storage */
    la      t0, rv_core_critical_regs_frame
    lw      t0, 0(t0)           /* t0 pointer to RvCoreCriticalSleepFrame object */

    sw      ra, RV_SLP_CTX_RA(t0)
    sw      sp, RV_SLP_CTX_SP(t0)
    sw      gp, RV_SLP_CTX_GP(t0)
    sw      tp, RV_SLP_CTX_TP(t0)
    sw      t1, RV_SLP_CTX_T1(t0)
    sw      t2, RV_SLP_CTX_T2(t0)
    sw      s0, RV_SLP_CTX_S0(t0)
    sw      s1, RV_SLP_CTX_S1(t0)

    /* a0 is caller saved, so it does not need to be saved, but it should be the
       pointer value of RvCoreCriticalSleepFrame for return.
     */
    mv      a0, t0
    sw      a0, RV_SLP_CTX_A0(t0)
    sw      a1, RV_SLP_CTX_A1(t0)
    sw      a2, RV_SLP_CTX_A2(t0)
    sw      a3, RV_SLP_CTX_A3(t0)
    sw      a4, RV_SLP_CTX_A4(t0)
    sw      a5, RV_SLP_CTX_A5(t0)
    sw      a6, RV_SLP_CTX_A6(t0)
    sw      a7, RV_SLP_CTX_A7(t0)
    sw      s2, RV_SLP_CTX_S2(t0)
    sw      s3, RV_SLP_CTX_S3(t0)
    sw      s4, RV_SLP_CTX_S4(t0)
    sw      s5, RV_SLP_CTX_S5(t0)
    sw      s6, RV_SLP_CTX_S6(t0)
    sw      s7, RV_SLP_CTX_S7(t0)
    sw      s8, RV_SLP_CTX_S8(t0)
    sw      s9, RV_SLP_CTX_S9(t0)
    sw      s10, RV_SLP_CTX_S10(t0)
    sw      s11, RV_SLP_CTX_S11(t0)
    sw      t3, RV_SLP_CTX_T3(t0)
    sw      t4, RV_SLP_CTX_T4(t0)
    sw      t5, RV_SLP_CTX_T5(t0)
    sw      t6, RV_SLP_CTX_T6(t0)

    csrr    t1, mstatus
    sw      t1, RV_SLP_CTX_MSTATUS(t0)
    csrr    t2, mtvec
    sw      t2, RV_SLP_CTX_MTVEC(t0)
    csrr    t3, mcause
    sw      t3, RV_SLP_CTX_MCAUSE(t0)

    csrr    t1, mtval
    sw      t1, RV_SLP_CTX_MTVAL(t0)
    csrr    t2, mie
    sw      t2, RV_SLP_CTX_MIE(t0)
    csrr    t3, mip
    sw      t3, RV_SLP_CTX_MIP(t0)
    csrr    t1, mepc
    sw      t1, RV_SLP_CTX_MEPC(t0)

    /*
    !!! Let idf knows it's going to sleep !!!

    RV_SLP_STK_PMUFUNC field is used to identify whether it is going to sleep or
    has just been awakened.  We use the lowest 2 bits as indication information,
    3 means being awakened, 1 means going to sleep.
    */
    li      t1, ~0x3
    lw      t2, RV_SLP_CTX_PMUFUNC(t0)
    and     t2, t1, t2
    ori     t2, t2, 0x1
    sw      t2, RV_SLP_CTX_PMUFUNC(t0)

    mv      t3, t0
    csrr    t0, mscratch
    lw      t1, RV_SLP_CTX_T1(t3)
    lw      t2, RV_SLP_CTX_T2(t3)
    lw      t3, RV_SLP_CTX_T3(t3)

    ret

    .size   rv_core_critical_regs_save, . - rv_core_critical_regs_save


#define CSR_PCER_U              0x800
#define CSR_PCMR_U              0x801
#define PCER_CYCLES             (1<<0)  /* count clock cycles */
#define PCMR_GLOBAL_EN          (1<<0)  /* enable count */
#define pcer                    CSR_PCER_U
#define pcmr                    CSR_PCMR_U

/*
--------------------------------------------------------------------------------
    This assembly subroutine is used to restore the CPU core critical register
    context before sleep after system wakes up, modify the PMU control
    information, and return the critical register context memory object pointer.
    After the subroutine returns, continue to restore other modules of the
    system.
--------------------------------------------------------------------------------
*/

    .section    .iram1,"ax"
    .global     rv_core_critical_regs_restore
    .weak       rv_core_critical_regs_restore
    .type       rv_core_critical_regs_restore,@function
    .global     _rv_core_critical_regs_restore
    .type       _rv_core_critical_regs_restore,@function
    .align      4

_rv_core_critical_regs_restore: /* export a strong symbol to jump to here, used
                                 * for a static callback */
    nop

rv_core_critical_regs_restore:

    la      t0, rv_core_critical_regs_frame
    lw      t0, 0(t0)           /* t0 pointer to RvCoreCriticalSleepFrame object */
    beqz    t0, .skip_restore   /* make sure we do not jump to zero address */

    /*
    !!! Let idf knows it's sleep awake. !!!

    RV_SLP_STK_PMUFUNC field is used to identify whether it is going to sleep or
    has just been awakened.  We use the lowest 2 bits as indication information,
    3 means being awakened, 1 means going to sleep.
    */
    lw      t1, RV_SLP_CTX_PMUFUNC(t0)
    ori     t1, t1, 0x3
    sw      t1, RV_SLP_CTX_PMUFUNC(t0)

    lw      t2, RV_SLP_CTX_MEPC(t0)
    csrw    mepc, t2
    lw      t3, RV_SLP_CTX_MIP(t0)
    csrw    mip, t3
    lw      t1, RV_SLP_CTX_MIE(t0)
    csrw    mie, t1
    lw      t2, RV_SLP_CTX_MSTATUS(t0)
    csrw    mstatus, t2

    lw      t3, RV_SLP_CTX_MTVEC(t0)
    csrw    mtvec, t3
    lw      t1, RV_SLP_CTX_MCAUSE(t0)
    csrw    mcause, t1
    lw      t2, RV_SLP_CTX_MTVAL(t0)
    csrw    mtval, t2

    lw      t6, RV_SLP_CTX_T6(t0)
    lw      t5, RV_SLP_CTX_T5(t0)
    lw      t4, RV_SLP_CTX_T4(t0)
    lw      t3, RV_SLP_CTX_T3(t0)
    lw      s11, RV_SLP_CTX_S11(t0)
    lw      s10, RV_SLP_CTX_S10(t0)
    lw      s9, RV_SLP_CTX_S9(t0)
    lw      s8, RV_SLP_CTX_S8(t0)
    lw      s7, RV_SLP_CTX_S7(t0)
    lw      s6, RV_SLP_CTX_S6(t0)
    lw      s5, RV_SLP_CTX_S5(t0)
    lw      s4, RV_SLP_CTX_S4(t0)
    lw      s3, RV_SLP_CTX_S3(t0)
    lw      s2, RV_SLP_CTX_S2(t0)
    lw      a7, RV_SLP_CTX_A7(t0)
    lw      a6, RV_SLP_CTX_A6(t0)
    lw      a5, RV_SLP_CTX_A5(t0)
    lw      a4, RV_SLP_CTX_A4(t0)
    lw      a3, RV_SLP_CTX_A3(t0)
    lw      a2, RV_SLP_CTX_A2(t0)
    lw      a1, RV_SLP_CTX_A1(t0)
    lw      a0, RV_SLP_CTX_A0(t0)
    lw      s1, RV_SLP_CTX_S1(t0)
    lw      s0, RV_SLP_CTX_S0(t0)
    lw      t2, RV_SLP_CTX_T2(t0)
    lw      t1, RV_SLP_CTX_T1(t0)
    lw      tp, RV_SLP_CTX_TP(t0)
    lw      gp, RV_SLP_CTX_GP(t0)
    lw      sp, RV_SLP_CTX_SP(t0)
    lw      ra, RV_SLP_CTX_RA(t0)
    lw      t0, RV_SLP_CTX_T0(t0)

.skip_restore:
    ret

    .size   rv_core_critical_regs_restore, . - rv_core_critical_regs_restore
