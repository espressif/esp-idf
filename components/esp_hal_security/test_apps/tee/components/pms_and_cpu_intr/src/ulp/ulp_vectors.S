/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "riscv/encoding.h"
#include "riscv/rvruntime-frames.h"
#include "esp_private/panic_reason.h"

    .equ SAVE_REGS, 32
    .equ CONTEXT_SIZE, (SAVE_REGS * 4)
    .global	test_panicHandler

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

    .section .text, "ax"

    .balign 0x100, 0xff
    .option push
    .option norvc
    .global _test_vector_table
    .type _test_vector_table, @function
_test_vector_table:
    j _test_panic_handler       /* exception handler, entry 0 */
    .rept 31
    j _test_interrupt_handler   /* 31 identical entries, all pointing to the interrupt handler */
    .endr

    .size _test_vector_table, .-_test_vector_table

    .option pop

    .balign	0x4, 0xff
    .global _test_panic_handler
    .type _test_panic_handler, @function
_test_panic_handler:
    save_general_regs RV_STK_FRMSZ
    sw      gp, RV_STK_GP(sp)
    addi    t0, sp, RV_STK_FRMSZ
    sw      t0, RV_STK_SP(sp)
    save_mepc
    save_mcsr

    /* Executing the panic handler */
    mv      a0, sp
    csrr    a1, mcause
    jal     test_panicHandler

    /* We arrive here if the exception handler has returned. */
    restore_mepc
    restore_general_regs RV_STK_FRMSZ

    csrw    mepc, ra
    mret

	.size  _test_panic_handler, .-_test_panic_handler

    .balign	0x4, 0xff
	.global _test_interrupt_handler
	.type _test_interrupt_handler, @function
_test_interrupt_handler:
	mret
	.size  _test_interrupt_handler, .-_test_interrupt_handler
