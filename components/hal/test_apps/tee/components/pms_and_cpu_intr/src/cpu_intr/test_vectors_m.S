/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "soc/interrupt_reg.h"
#include "soc/interrupt_matrix_reg.h"

#include "riscv/encoding.h"
#include "riscv/rvruntime-frames.h"
#include "esp_private/panic_reason.h"
#include "esp_private/vectors_const.h"

#include "test_cpu_intr_params.h"
#include "sdkconfig.h"

    .equ SAVE_REGS, 32
    .equ CONTEXT_SIZE, (SAVE_REGS * 4)
    .equ ECALL_U_MODE, 0x8
    .equ ECALL_M_MODE, 0xb
    .equ INTR_M2U_MAGIC,  0x1f
    .equ INTR_U2M_RTNVAL, 0xc0de
    .equ INTMTX_SEC_STATUS_REG, INTERRUPT_CORE0_SECURE_STATUS_REG
    .equ INTMTX_SIG_IDX_ASSERT_IN_SEC_REG, INTERRUPT_CORE0_SIG_IDX_ASSERT_IN_SEC_REG
    .equ VECTORS_XCAUSE_XPIE_MASK, 0x08000000
    .equ panic_from_excp, test_panicHandler

    .global _test_uintr_handler

    .section .data

    .align 4
    .global _m_sp
_m_sp:
    .word 0

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

.macro store_magic_general_regs
    lui     ra, INTR_M2U_MAGIC
    lui     tp, INTR_M2U_MAGIC
    lui     t0, INTR_M2U_MAGIC
    lui     t1, INTR_M2U_MAGIC
    lui     t2, INTR_M2U_MAGIC
    lui     s0, INTR_M2U_MAGIC
    lui     s1, INTR_M2U_MAGIC
    lui     a0, INTR_M2U_MAGIC
    lui     a1, INTR_M2U_MAGIC
    lui     a2, INTR_M2U_MAGIC
    lui     a3, INTR_M2U_MAGIC
    lui     a4, INTR_M2U_MAGIC
    lui     a5, INTR_M2U_MAGIC
    lui     a6, INTR_M2U_MAGIC
    lui     a7, INTR_M2U_MAGIC
    lui     s2, INTR_M2U_MAGIC
    lui     s3, INTR_M2U_MAGIC
    lui     s4, INTR_M2U_MAGIC
    lui     s5, INTR_M2U_MAGIC
    lui     s6, INTR_M2U_MAGIC
    lui     s7, INTR_M2U_MAGIC
    lui     s8, INTR_M2U_MAGIC
    lui     s9, INTR_M2U_MAGIC
    lui     s10, INTR_M2U_MAGIC
    lui     s11, INTR_M2U_MAGIC
    lui     t3, INTR_M2U_MAGIC
    lui     t4, INTR_M2U_MAGIC
    lui     t5, INTR_M2U_MAGIC
    lui     t6, INTR_M2U_MAGIC
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
    .global _test_mtvt_table
    .type _test_mtvt_table, @function
_test_mtvt_table:
    /* First 16 (CLIC_EXT_INTR_NUM_OFFSET) entries are system interrupts */
    .rept CLIC_EXT_INTR_NUM_OFFSET
    .word _test_panic_handler
    .endr
    /* Next 31 entries are free interrupts (indices 16 to 46) */
    .rept 31
    .word _test_mintr_handler
    .endr
    /* Entry 47 is special - used for U-mode interrupt delegation */
    .word _test_uintr_deleg
    .size _test_mtvt_table, .-_test_mtvt_table

    /*
     * Non-hardware vectored interrupt entry. MTVEC CSR points here.
     *
     * On targets that use CLIC as their interrupt controller, when an exception occurs, the CPU
     * jumps to the address stored in MTVEC[31:6] << 6. The CPU will also jump to this location
     * if an interrupt is configured as non-vectored (CLIC_INT_ATTR.shv = 0).
     *
     */
    .balign 0x40
    .global _test_mtvec_table
    .type _test_mtvec_table, @function
_test_mtvec_table:
    /* Backup t0, t1 on the stack before using it */
    addi    sp, sp, -16
    sw      t0, 0(sp)
    sw      t1, 4(sp)

    /* Check whether the exception is an M-mode ecall */
    csrr    t0, mcause
    li      t1, VECTORS_MCAUSE_REASON_MASK
    and     t0, t0, t1
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
_test_panic_handler:
    /* Save the register and CSR context */
    save_general_regs RV_STK_FRMSZ
    sw      gp, RV_STK_GP(sp)
    addi    t0, sp, RV_STK_FRMSZ
    sw      t0, RV_STK_SP(sp)
    save_mepc
    save_mcsr

    /* Executing the panic handler */
    li      t0, 0xDEADC0DE
    csrw    mscratch, t0
    mv      a0, sp
    csrr    a1, mcause
    li      t0, VECTORS_MCAUSE_REASON_MASK
    and     a1, a1, t0
    jal     panic_from_excp

    /* We arrive here if the exception handler has returned. */
    restore_mepc
    restore_general_regs RV_STK_FRMSZ
    mret

    .size  _test_mtvec_table, .-_test_mtvec_table

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

    /* Restore t0, t1 from the stack */
    lw      t1, 4(sp)
    lw      t0, 0(sp)
    addi    sp, sp, 16
    mret

    /* U-mode ecall handler */
    /* Currently in U-mode and switch to M-mode */
_test_user_ecall:
    /* Check whether we are returning after servicing an U-mode interrupt */
    lui     t0, INTR_U2M_RTNVAL
    csrrw   t1, mscratch, zero
    beq     t0, t1, _rtn_from_u_intr

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

    /* This point is reached after servicing a U-mode interrupt occurred in M-mode */
_rtn_from_u_intr:
    /* Disable the U-mode delegation of all interrupts */
    li      t0, INTMTX_SIG_IDX_ASSERT_IN_SEC_REG
    li      t1, TEST_INTR_NUM_PASS_IN_SEC + CLIC_EXT_INTR_NUM_OFFSET
    sw      t1, 0(t0)
    fence
    /* Verify the interrupt remap */
_1:
    lw      t2, 0(t0)
    bne     t2, t1, _1

    /* Restore the secure stack pointer */
    la      t0, _m_sp
    lw      sp, 0(t0)

    /* Set the privilege mode to transition to after mret to M-mode */
    li      t0, MSTATUS_MPP
    csrs    mstatus, t0

    /* Restore register context and resume the secure service */
    restore_mepc
    restore_general_regs

    mret

    .size  _test_ecall_handler, .-_test_ecall_handler

    /* Interrupt handler */
    .type _test_mintr_handler, @function
_test_mintr_handler:
    /* Start by saving the general purpose registers and the PC value before
     * the interrupt happened. */
    save_general_regs
    save_mepc

    /* Save GP and SP here. */
    sw      gp, RV_STK_GP(sp)
    addi    a0, sp, CONTEXT_SIZE
    sw      a0, RV_STK_SP(sp)

    /* Save mcause */
    csrr    s1, mcause
    csrr    s2, mstatus

    /* Enable nested interrupts */
    csrsi   mstatus, MSTATUS_MIE

    /* call the C dispatcher */
    mv      a0, sp      /* argument 1, stack pointer */
    mv      a1, s1      /* argument 2, interrupt number (mcause) */
    /* mask off the interrupt flag of mcause */
    li      t0, VECTORS_MCAUSE_REASON_MASK
    and     a1, a1, t0
    jal     _test_global_interrupt_handler

    /* Disable nested interrupts */
    csrci   mstatus, MSTATUS_MIE

    /* Restore the rest of the registers. */
    csrw    mcause, s1
    csrw    mstatus, s2

    restore_mepc
    restore_general_regs
    /* exit, this will also re-enable the interrupts */
    mret

    .size  _test_mintr_handler, .-_test_mintr_handler

    /* U-mode to M-mode switch */
    .balign 4
    .global _test_u2m_switch
    .type   _test_u2m_switch, @function
_test_u2m_switch:
    ecall
    fence
    ret

    .size  _test_u2m_switch, .-_test_u2m_switch

    /* U-mode interrupt delegation */
    .global _test_uintr_deleg
    .type _test_uintr_deleg, @function
_test_uintr_deleg:
    /* Start by saving the general purpose registers and the PC value before
     * the interrupt happened. */
    save_general_regs
    save_mepc

    /* Save GP and SP here. */
    sw      gp, RV_STK_GP(sp)
    addi    a0, sp, CONTEXT_SIZE
    sw      a0, RV_STK_SP(sp)

    /* Pass the interrupt ID to be serviced to U-mode */
    li      t2, INTMTX_SEC_STATUS_REG
    lw      t0, 0(t2)
    li      t1, 0

_find_intr_loop:
    and     t2, t0, 1
    bnez    t2, _found_intr
    srai    t0, t0, 1
    addi    t1, t1, 1
    bnez    t0, _find_intr_loop

    /* should not reach here */
    li      t1, -1
    unimp
_found_intr:
    addi    t0, t1, CLIC_EXT_INTR_NUM_OFFSET
    csrr    t1, ucause
    or      t1, t1, t0
    li      t2, (VECTORS_MCAUSE_INTBIT_MASK | VECTORS_XCAUSE_XPIE_MASK)
    or      t1, t1, t2
    csrw    ucause, t1

    /* Enable the U-mode interrupt delegation */
    li      t0, INTMTX_SIG_IDX_ASSERT_IN_SEC_REG
    li      t1, 0x00
    sw      t1, 0(t0)
    fence
    /* Verify the interrupt remap */
_2:
    lw      t2, 0(t0)
    bne     t2, t1, _2

    /* For U-mode interrupts, we use mret to switch to U-mode after executing the below steps - */
    /* Disable the U-mode global interrupts */
    csrci   ustatus, USTATUS_UIE

    /* Configure `uepc` with the U-mode ecall handler (see _test_u2m_switch) so that we can
     * return to M-mode after handling the interrupt */
    la      t0, _test_u2m_switch
    csrw    uepc, t0

    /* Set the program counter to the U-mode global interrupt handler (see _test_uintr_handler) */
    la      t0, _test_uintr_handler
    csrw    mepc, t0

    /* Set the privilege mode to transition to after mret to U-mode */
    li      t1, MSTATUS_MPP
    csrc    mstatus, t1

    /* Save the current secure stack pointer */
    la      t0, _m_sp
    sw      sp, 0(t0)

    /* Set a flag to identify the next U2M switch would be after handling a U-mode interrupt */
    lui     t0, INTR_U2M_RTNVAL
    csrw    mscratch, t0

    /* Place magic bytes in all the general registers */
    store_magic_general_regs

    mret

    .size  _test_uintr_deleg, .-_test_uintr_deleg

    .option pop
