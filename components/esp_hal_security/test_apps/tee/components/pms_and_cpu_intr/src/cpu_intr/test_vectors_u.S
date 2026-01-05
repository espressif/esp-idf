/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "soc/clic_reg.h"
#include "soc/interrupt_reg.h"

#include "riscv/encoding.h"
#include "riscv/rvruntime-frames.h"
#include "esp_private/panic_reason.h"
#include "esp_private/vectors_const.h"

#include "sdkconfig.h"

    .equ SAVE_REGS, 32
    .equ CONTEXT_SIZE, (SAVE_REGS * 4)
    .equ UINTTHRESH_CSR,  0x047
    .equ RV_INTR_NUM, (CLIC_EXT_INTR_NUM_OFFSET + SOC_CPU_INTR_NUM)

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

.macro save_uepc
    csrr    t0, uepc
    sw      t0, RV_STK_MEPC(sp)
.endm

.macro save_ucsr
    csrr  t0, ustatus
    sw    t0, RV_STK_MSTATUS(sp)
    csrr  t0, utvec
    sw    t0, RV_STK_MTVEC(sp)
    csrr  t0, ucause
    sw    t0, RV_STK_MCAUSE(sp)
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

.macro restore_uepc
    lw      t0, RV_STK_MEPC(sp)
    csrw    uepc, t0
.endm

.macro restore_ucsr
    lw    t0, RV_STK_MSTATUS(sp)
    csrw  ustatus, t0
    lw    t0, RV_STK_MTVEC(sp)
    csrw  utvec, t0
    lw    t0, RV_STK_MCAUSE(sp)
    csrw  ucause, t0
.endm

    .section .iram1, "ax"

    /* Prevent the compiler from generating 2-byte instruction in the vector tables */
    .option push
    .option norvc

    /**
     * Vectored interrupt table. MTVT CSR points here.
     *
     * If an interrupt occurs and is configured as (hardware) vectored, the CPU will jump to
     * MTVT[31:0] + 4 * interrupt_id
     *
     */
    .balign 0x40
    .global _test_utvt_table
    .type _test_utvt_table, @function
_test_utvt_table:
    .rept RV_INTR_NUM
    .word _test_uintr_handler
    .endr
    .size _test_utvt_table, .-_test_utvt_table

    /*
     * Non-hardware vectored interrupt entry. MTVEC CSR points here.
     *
     * On targets that use CLIC as their interrupt controller, when an exception occurs, the CPU
     * jumps to the address stored in MTVEC[31:6] << 6. The CPU will also jump to this location
     * if an interrupt is configured as non-vectored (CLIC_INT_ATTR.shv = 0).
     *
     */
    .balign 0x40
    .global _test_utvec_table
    .type _test_utvec_table, @function
_test_utvec_table:
    /* Do nothing */
    mret

    .size  _test_utvec_table, .-_test_utvec_table

    /* Interrupt handler */
    .global _test_uintr_handler
    .type _test_uintr_handler, @function
_test_uintr_handler:
    /* Start by saving the general purpose registers and the PC value before
     * the interrupt happened. */
    save_general_regs
    save_uepc

    /* Save GP and SP here. */
    sw      gp, RV_STK_GP(sp)
    addi    a0, sp, CONTEXT_SIZE
    sw      a0, RV_STK_SP(sp)

    /* Save ucause */
    csrr    s1, ucause
    csrr    s2, ustatus

    /* Enable nested interrupts */
    csrsi   ustatus, USTATUS_UIE

    /* call the C dispatcher */
    mv      a0, sp      /* argument 1, stack pointer */
    mv      a1, s1      /* argument 2, interrupt number (mcause) */
    /* mask off the interrupt flag of mcause */
    li      t0, VECTORS_MCAUSE_REASON_MASK
    and     a1, a1, t0
    jal     _test_global_interrupt_handler

    /* Disable nested interrupts */
    csrci   ustatus, USTATUS_UIE

    /* Restore the rest of the registers. */
    csrw    ucause, s1
    csrw    ustatus, s2

    restore_uepc
    restore_general_regs

    /* exit, this will also re-enable the interrupts */
    uret

    .size  _test_uintr_handler, .-_test_uintr_handler

    .option pop
