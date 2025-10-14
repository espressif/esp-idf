/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "soc/interrupt_reg.h"

#include "riscv/encoding.h"
#include "riscv/rvruntime-frames.h"
#include "esp_private/panic_reason.h"
#include "esp_private/vectors_const.h"

#include "sdkconfig.h"

    .equ SAVE_REGS, 32
    .equ CONTEXT_SIZE, (SAVE_REGS * 4)
    .equ panic_from_excp, test_panicHandler
    .equ ECALL_U_MODE, 0x8
    .equ ECALL_M_MODE, 0xb

/* Macro which first allocates space on the stack to save general
 * purpose registers, and then save them. GP register is excluded.
 * The default size allocated on the stack is CONTEXT_SIZE, but it
 * can be overridden. */
.macro save_general_regs cxt_size=CONTEXT_SIZE
    addi sp, sp, -\cxt_size
    sw   ra, RV_STK_RA(sp)
    sw   tp, RV_STK_TP(sp)
    sw   t0, RV_STK_T0(sp)
    sw   t1, RV_STK_T1(sp)
    sw   t2, RV_STK_T2(sp)
    sw   s0, RV_STK_S0(sp)
    sw   s1, RV_STK_S1(sp)
    sw   a0, RV_STK_A0(sp)
    sw   a1, RV_STK_A1(sp)
    sw   a2, RV_STK_A2(sp)
    sw   a3, RV_STK_A3(sp)
    sw   a4, RV_STK_A4(sp)
    sw   a5, RV_STK_A5(sp)
    sw   a6, RV_STK_A6(sp)
    sw   a7, RV_STK_A7(sp)
    sw   s2, RV_STK_S2(sp)
    sw   s3, RV_STK_S3(sp)
    sw   s4, RV_STK_S4(sp)
    sw   s5, RV_STK_S5(sp)
    sw   s6, RV_STK_S6(sp)
    sw   s7, RV_STK_S7(sp)
    sw   s8, RV_STK_S8(sp)
    sw   s9, RV_STK_S9(sp)
    sw   s10, RV_STK_S10(sp)
    sw   s11, RV_STK_S11(sp)
    sw   t3, RV_STK_T3(sp)
    sw   t4, RV_STK_T4(sp)
    sw   t5, RV_STK_T5(sp)
    sw   t6, RV_STK_T6(sp)
.endm

.macro save_mepc
    csrr    t0, mepc
    sw      t0, RV_STK_MEPC(sp)
.endm

.macro save_mcsr
    csrr  t0, mstatus
    sw    t0, RV_STK_MSTATUS(sp)
    csrr  t0, mtvec
    sw    t0, RV_STK_MTVEC(sp)
    csrr  t0, mcause
    sw    t0, RV_STK_MCAUSE(sp)
    csrr  t0, mtval
    sw    t0, RV_STK_MTVAL(sp)
    csrr  t0, mhartid
    sw    t0, RV_STK_MHARTID(sp)
.endm

/* Restore the general purpose registers (excluding gp) from the context on
 * the stack. The context is then deallocated. The default size is CONTEXT_SIZE
 * but it can be overridden. */
.macro restore_general_regs cxt_size=CONTEXT_SIZE
    lw   ra, RV_STK_RA(sp)
    lw   tp, RV_STK_TP(sp)
    lw   t0, RV_STK_T0(sp)
    lw   t1, RV_STK_T1(sp)
    lw   t2, RV_STK_T2(sp)
    lw   s0, RV_STK_S0(sp)
    lw   s1, RV_STK_S1(sp)
    lw   a0, RV_STK_A0(sp)
    lw   a1, RV_STK_A1(sp)
    lw   a2, RV_STK_A2(sp)
    lw   a3, RV_STK_A3(sp)
    lw   a4, RV_STK_A4(sp)
    lw   a5, RV_STK_A5(sp)
    lw   a6, RV_STK_A6(sp)
    lw   a7, RV_STK_A7(sp)
    lw   s2, RV_STK_S2(sp)
    lw   s3, RV_STK_S3(sp)
    lw   s4, RV_STK_S4(sp)
    lw   s5, RV_STK_S5(sp)
    lw   s6, RV_STK_S6(sp)
    lw   s7, RV_STK_S7(sp)
    lw   s8, RV_STK_S8(sp)
    lw   s9, RV_STK_S9(sp)
    lw   s10, RV_STK_S10(sp)
    lw   s11, RV_STK_S11(sp)
    lw   t3, RV_STK_T3(sp)
    lw   t4, RV_STK_T4(sp)
    lw   t5, RV_STK_T5(sp)
    lw   t6, RV_STK_T6(sp)
    addi sp,sp, \cxt_size
.endm

.macro restore_mepc
    lw      t0, RV_STK_MEPC(sp)
    csrw    mepc, t0
.endm

.macro restore_mcsr
    lw    t0, RV_STK_MSTATUS(sp)
    csrw  mstatus, t0
    lw    t0, RV_STK_MTVEC(sp)
    csrw  mtvec, t0
    lw    t0, RV_STK_MCAUSE(sp)
    csrw  mcause, t0
    lw    t0, RV_STK_MTVAL(sp)
    csrw  mtval, t0
    lw    t0, RV_STK_MHARTID(sp)
    csrw  mhartid, t0
.endm

    .section .iram1, "ax"
    /* This is the vector table. MTVEC points here.
     *
     * Use 4-byte instructions here. 1 instruction = 1 entry of the table.
     * The CPU jumps to MTVEC (i.e. the first entry) in case of an exception,
     * and (MTVEC & 0xfffffffc) + (mcause & 0x7fffffff) * 4, in case of an interrupt.
     *
     * Note: for the PULP, we need to place this on a 256-byte boundary, as PULP
     * only uses the 24 MSBs of the MTVEC, i.e. (MTVEC & 0xffffff00).
     */
    .balign 0x100
    .option push
    .option norvc
    .global _test_vector_table
    .type _test_vector_table, @function
_test_vector_table:
    j _test_panic_handler       /* exception handler, entry 0 */
#if !SOC_INT_CLIC_SUPPORTED
    .rept 31
    j _test_interrupt_handler   /* 31 identical entries, all pointing to the interrupt handler */
    .endr
#endif
	.size _test_vector_table, .-_test_vector_table

    .option pop

    /* Exception handler. */
    .global _test_panic_handler
    .type _test_panic_handler, @function
_test_panic_handler:
    /* Backup t0, t1 on the stack before using it */
    addi    sp, sp, -16
    sw      t0, 0(sp)
    sw      t1, 4(sp)

    /* Read mcause */
    csrr    t0, mcause
    li      t1, VECTORS_MCAUSE_REASON_MASK
    and     t0, t0, t1

    /* Check whether the exception is an M-mode ecall */
    li      t1, ECALL_M_MODE
    beq     t0, t1, _test_machine_ecall

    /* Check whether the exception is an U-mode ecall */
    li      t1, ECALL_U_MODE
    beq     t0, t1, _test_user_ecall

    /* Restore t0, t1 from the stack */
    lw      t1, 4(sp)
    lw      t0, 0(sp)
    addi    sp, sp, 16

    /* Not an exception raised by the ecall instruction */
    save_general_regs RV_STK_FRMSZ
    sw      gp, RV_STK_GP(sp)
    addi    t0, sp, RV_STK_FRMSZ
    sw      t0, RV_STK_SP(sp)
    save_mepc
    save_mcsr

    /* Executing the panic handler */
    mv      a0, sp
    csrr    a1, mcause
    li      t0, VECTORS_MCAUSE_REASON_MASK
    and     a1, a1, t0
    jal     panic_from_excp

    /* We arrive here if the exception handler has returned. */
    restore_mepc
    restore_general_regs RV_STK_FRMSZ
    mret

    .size  _test_panic_handler, .-_test_panic_handler

    /* ECALL handler. */
    .type _test_ecall_handler, @function
_test_ecall_handler:
    /* M-mode ecall handler */
    /* Currently in M-mode and switch to U-mode */
_test_machine_ecall:
    csrr    t0, mepc
    addi    t0, t0, 4
    csrw    mepc, t0
    li      t1, MSTATUS_MPP
    csrc    mstatus, t1
#if !SOC_CPU_HAS_CSR_PC
    csrsi   mcounteren, 0x7
#endif

    /* Restore t0, t1 from the stack */
    lw      t1, 4(sp)
    lw      t0, 0(sp)
    addi    sp, sp, 16
    mret

    /* U-mode ecall handler */
    /* Currently in U-mode and switch to M-mode */
_test_user_ecall:
    csrr    t0, mepc
    addi    t0, t0, 4
    csrw    mepc, t0
    li      t1, MSTATUS_MPP
    csrs    mstatus, t1

    /* Restore t0, t1 from the stack */
    lw      t1, 4(sp)
    lw      t0, 0(sp)
    addi    sp, sp, 16
    mret

    .size  _test_ecall_handler, .-_test_ecall_handler

    .global rtos_int_enter
    .global rtos_int_exit
    .global _test_global_interrupt_handler
    /* Interrupt handler */
	.type _test_interrupt_handler, @function
_test_interrupt_handler:
    /* Start by saving the general purpose registers and the PC value before
     * the interrupt happened. */
    save_general_regs
	save_mepc

    /* Save GP and SP here. */
    sw      gp, RV_STK_GP(sp)
    addi    a0, sp, CONTEXT_SIZE
    sw      a0, RV_STK_SP(sp)

    /* Notify the RTOS that an interrupt occurred, it will save the current stack pointer
     * in the running TCB, no need to pass it as a parameter
     * Returns an abstract context in a0, needs to be passed to `rtos_int_exit` */
    call    rtos_int_enter
    mv      s4, a0
    /* If this is a non-nested interrupt, SP now points to the interrupt stack */

    /* Before dispatch c handler, restore interrupt to enable nested intr */
    csrr    s1, mcause
    csrr    s2, mstatus

#if !SOC_INT_HW_NESTED_SUPPORTED
    /* Save the interrupt threshold level */
    li      t0, INTERRUPT_CURRENT_CORE_INT_THRESH_REG
    lw      s3, 0(t0)

    /* Increase interrupt threshold level */
    li      t2, VECTORS_MCAUSE_REASON_MASK
    and     t1, s1, t2       /* t1 = mcause & mask */
    slli    t1, t1, 2        /* t1 = mcause * 4 */
    li      t2, INTERRUPT_PRIO_REG(0)
    add     t1, t2, t1       /* t1 = INTERRUPT_PRIO_REG + 4 * mcause */
    lw      t2, 0(t1)        /* t2 = INTERRUPT_PRIO_REG[mcause] */
    addi    t2, t2, 1        /* t2 = t2 +1 */
    sw      t2, 0(t0)        /* INTERRUPT_CURRENT_CORE_INT_THRESH_REG = t2 */
    fence
#endif

    csrsi  mstatus, MSTATUS_MIE
    /* MIE set. Nested interrupts can now occur */

    /* call the C dispatcher */
    mv      a0, sp      /* argument 1, stack pointer */
    mv      a1, s1      /* argument 2, interrupt number (mcause) */
    /* mask off the interrupt flag of mcause */
    li      t0, VECTORS_MCAUSE_REASON_MASK
    and     a1, a1, t0
    jal     _test_global_interrupt_handler

    /* After dispatch c handler, disable interrupt to make freertos make context switch */

    csrci  mstatus, MSTATUS_MIE
    /* MIE cleared. Nested interrupts are disabled */

#if !SOC_INT_HW_NESTED_SUPPORTED
    /* restore the interrupt threshold level */
    li      t0, INTERRUPT_CURRENT_CORE_INT_THRESH_REG
    sw      s3, 0(t0)
    fence
#endif

    /* The RTOS will restore the current TCB stack pointer. This routine will preserve s1 and s2.
     * Returns the new `mstatus` value. */
    mv      a0, s2      /* a0 = mstatus */
    mv      a1, s4      /* a1 = abstract context returned by `rtos_int_enter` */
    call    rtos_int_exit

    /* Restore the rest of the registers. */
    csrw    mcause, s1
    csrw    mstatus, a0
    restore_mepc
    restore_general_regs
    /* exit, this will also re-enable the interrupts */
    mret
    .size  _test_interrupt_handler, .-_test_interrupt_handler
