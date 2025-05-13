/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc.h"
#include "soc/soc_caps.h"

#include "riscv/encoding.h"
#include "riscv/rvruntime-frames.h"
#include "esp_private/vectors_const.h"

#include "esp_tee.h"
#include "esp_tee_intr_defs.h"
#include "sdkconfig.h"

    .equ SAVE_REGS, 32
    .equ CONTEXT_SIZE, (SAVE_REGS * 4)
    .equ panic_from_exception, tee_panic_from_exc
    .equ panic_from_isr, tee_panic_from_isr
    .equ MAGIC, 0x1f
    .equ RTNVAL, 0xc0de
    .equ ECALL_U_MODE, 0x8
    .equ ECALL_M_MODE, 0xb
    .equ TEE_INTR_DELEG_MASK, 0xffffbfff

    .global esp_tee_global_interrupt_handler
    .global esp_tee_service_dispatcher

    .section .data
    .align 4
    .global _ns_sp
_ns_sp:
    .word 0

    .section .data
    .align 4
    .global _s_sp
_s_sp:
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

.macro store_magic_general_regs
    lui     ra, MAGIC
    lui     tp, MAGIC
    lui     t0, MAGIC
    lui     t1, MAGIC
    lui     t2, MAGIC
    lui     s0, MAGIC
    lui     s1, MAGIC
    lui     a0, MAGIC
    lui     a1, MAGIC
    lui     a2, MAGIC
    lui     a3, MAGIC
    lui     a4, MAGIC
    lui     a5, MAGIC
    lui     a6, MAGIC
    lui     a7, MAGIC
    lui     s2, MAGIC
    lui     s3, MAGIC
    lui     s4, MAGIC
    lui     s5, MAGIC
    lui     s6, MAGIC
    lui     s7, MAGIC
    lui     s8, MAGIC
    lui     s9, MAGIC
    lui     s10, MAGIC
    lui     s11, MAGIC
    lui     t3, MAGIC
    lui     t4, MAGIC
    lui     t5, MAGIC
    lui     t6, MAGIC
.endm

    .section .exception_vectors.text, "ax"

    /* Exception handler. */
    .global _panic_handler
    .type _panic_handler, @function
_panic_handler:
    /* Backup t0 on the stack before using it */
    addi    sp, sp, -16
    sw      t0, 0(sp)

    /* Read mcause */
    csrr    t0, mcause
    li      t1, VECTORS_MCAUSE_INTBIT_MASK | VECTORS_MCAUSE_REASON_MASK
    and     t0, t0, t1

    /* Check whether the exception is an M-mode ecall */
    li      t1, ECALL_M_MODE
    beq     t0, t1, _machine_ecall

    /* Check whether the exception is an U-mode ecall */
    li      t1, ECALL_U_MODE
    beq     t0, t1, _user_ecall

    /* Restore t0 from the stack */
    lw      t0, 0(sp)
    addi    sp, sp, 16

    /* Not an ecall, proceed to the panic handler */
    /* Allocate space on the stack and store general purpose registers */
    save_general_regs RV_STK_FRMSZ

    /* As gp register is not saved by the macro, save it here */
    sw      gp, RV_STK_GP(sp)

    /* Same goes for the SP value before trapping */
    addi    t0, sp, RV_STK_FRMSZ /* restore sp with the value when trap happened */

    /* Save CSRs */
    sw      t0, RV_STK_SP(sp)
    save_mepc
    save_mcsr

    /* Call panic_from_exception(sp) or panic_from_isr(sp)
     * depending on whether we have a pseudo excause or not.
     * If mcause's highest bit is 1, then an interrupt called this routine,
     * so we have a pseudo excause. Else, it is due to a exception, we don't
     * have an pseudo excause */
    mv      a0, sp
    csrr    a1, mcause
    /* Branches instructions don't accept immediates values, so use t1 to
     * store our comparator */
    li      t0, 0x80000000
    bgeu    a1, t0, _call_panic_handler
    sw      a1, RV_STK_MCAUSE(sp)
    /* exception_from_panic never returns */
    jal     panic_from_exception
    /* We arrive here if the exception handler has returned. */
    j       _return_from_exception

_call_panic_handler:
    /* Remove highest bit from mcause (a1) register and save it in the
     * structure */
    not     t0, t0
    and     a1, a1, t0
    sw      a1, RV_STK_MCAUSE(sp)
    jal     panic_from_isr

    /* We arrive here if the exception handler has returned. This means that
     * the exception was handled, and the execution flow should resume.
     * Restore the registers and return from the exception.
     */
_return_from_exception:
    restore_mepc
    /* MTVEC and SP are assumed to be unmodified.
     * MSTATUS, MHARTID, MTVAL are read-only and not restored. */
    lw      gp, RV_STK_GP(sp)
    restore_general_regs RV_STK_FRMSZ
    mret

    .size  _panic_handler, .-_panic_handler

    /* ECALL handler. */
    .type _ecall_handler, @function
_ecall_handler:
    /* M-mode ecall handler */
_machine_ecall:
    /* Set the privilege mode to transition to after mret to U-mode  */
    li      t0, MSTATUS_MPP
    csrc    mstatus, t0

    /* Check whether this is the first M-mode ecall (see esp_tee_init) and skip context restoration */
    lui     t0, ESP_TEE_M2U_SWITCH_MAGIC
    beq     a1, t0, _skip_ctx_restore

    /* Switching back to the saved REE stack */
    la      t0, _ns_sp
    lw      sp, 0(t0)
    fence

    /* Backup the A0 register
     * This point is reached after an ecall is triggered after executing the secure service.
     * The A0 register contains the return value of the corresponding service.
     * After restoring the entire register context, we assign A0 the value back to the return value. */
    csrw    mscratch, a0
    restore_general_regs RV_STK_FRMSZ
    csrrw   a0, mscratch, zero

_skip_ctx_restore:
    /* Copy the ra register to mepc which contains the user app entry point (i.e. call_start_cpu0) */
    csrw    mepc, ra

    /* Jump to the REE */
    mret

    /* U-mode ecall handler */
_user_ecall:
    /* Check whether we are returning after servicing an U-mode interrupt */
    lui     t0, RTNVAL
    csrr    t1, mscratch
    beq     t0, t1, _rtn_from_ns_int
    csrwi   mscratch, 0

    /* Restore t0 from the stack */
    lw      t0, 0(sp)
    addi    sp, sp, 16

    /* This point is reached when a secure service call is issued from the REE */
    /* Save register context and mepc */
    save_general_regs RV_STK_FRMSZ
    save_mepc

    /* Save the U-mode (i.e. REE) stack pointer */
    la      t0, _ns_sp
    sw      sp, 0(t0)

    /* Switch to the M-mode (i.e. TEE) stack */
    la      sp, _tee_stack

    /* Disable the U-mode delegation of all interrupts */
    csrwi   mideleg, 0

    /* Enable interrupts */
    csrsi   mstatus, MSTATUS_MIE

    /* Jump to the secure service dispatcher */
    jal     esp_tee_service_dispatcher

    /* Enable the U-mode delegation of all interrupts (except the TEE secure interrupt) */
    li      t0, TEE_INTR_DELEG_MASK
    csrs    mideleg, t0

    /* Fire an M-ecall */
    mv      a1, zero
    ecall

    /* This point is reached after servicing a U-mode interrupt occurred
     * while executing a secure service */
_rtn_from_ns_int:
    /* Disable the U-mode interrupt delegation */
    csrwi    mideleg, 0

    /* Restore the secure stack pointer */
    la      t0, _s_sp
    lw      sp, 0(t0)

    /* Clear the flag set marking the completion of interrupt service */
    csrwi    mscratch, 0

    /* Set the privilege mode to transition to after mret to M-mode */
    li      t0, MSTATUS_MPP
    csrs    mstatus, t0

    /* Restore register context and resume the secure service */
    restore_mepc
    restore_general_regs RV_STK_FRMSZ

    mret

    .size  _ecall_handler, .-_ecall_handler

    /* This is the interrupt handler for the U-mode interrupts.
     * It saves the registers on the stack, re-enables the interrupt delegation,
     * then jumps to the U-mode global interrupt handler, */
    .global _tee_ns_intr_handler
    .type _tee_ns_intr_handler, @function
_tee_ns_intr_handler:
    /* Start by saving the general purpose registers and the PC value before
     * the interrupt happened. */
    save_general_regs RV_STK_FRMSZ
    save_mepc

    /* Though it is not necessary we save GP and SP here.
     * SP is necessary to help GDB to properly unwind
     * the backtrace of threads preempted by interrupts (OS tick etc.).
     * GP is saved just to have its proper value in GDB. */
    /* As gp register is not saved by the macro, save it here */
    sw      gp, RV_STK_GP(sp)
    /* Same goes for the SP value before trapping */
    addi    t0, sp, RV_STK_FRMSZ /* restore sp with the value when interrupt happened */
    /* Save SP */
    sw      t0, RV_STK_SP(sp)

    /* For U-mode interrupts, we use mret to switch to U-mode after executing the below steps - */
    /* Disable the U-mode global interrupts */
    csrci    ustatus, USTATUS_UIE

    /* Pass the interrupt ID to be serviced to U-mode */
    csrr    t0, mcause
    csrw    ucause, t0

    /* Configure `uepc` with the U-mode ecall handler (see u2m_switch) so that we can
     * return to M-mode after handling the interrupt */
    la      t0, esp_tee_app_config
    lw      t1, ESP_TEE_CFG_OFFS_NS_ENTRY_ADDR(t0)
    csrw    uepc, t1

    /* Set the program counter to the U-mode global interrupt handler (see _interrupt_handler) */
    lw      t1, ESP_TEE_CFG_OFFS_NS_INTR_HANDLER(t0)
    csrw    mepc, t1

    /* Set the privilege mode to transition to after mret to U-mode */
    li      t1, MSTATUS_MPP
    csrc    mstatus, t1

    /* Save the current secure stack pointer and switch to the U-mode interrupt stack
     * saved while entering the secure service call routine (see `sec_world_entry`) */
    la     t0, _s_sp
    sw     sp, 0(t0)
    la     t1, _ns_sp
    lw     sp, 0(t1)

    /* Set a flag to identify the next U2M switch would be after handling a U-mode interrupt */
    lui     t0, RTNVAL
    csrw    mscratch, t0

    /* Enable the U-mode interrupt delegation (except for the TEE secure interrupt) */
    li      t0, TEE_INTR_DELEG_MASK
    csrs    mideleg, t0

    /* Place magic bytes in all the general registers */
    store_magic_general_regs

    mret

    .size  _tee_ns_intr_handler, .-_tee_ns_intr_handler

    /* This is the interrupt handler for the M-mode interrupts.
     * It saves the registers on the stack, prepares for interrupt nesting,
     * re-enables the interrupts, then jumps to the C dispatcher in esp_tee_intr.c. */
    .global _tee_s_intr_handler
    .type _tee_s_intr_handler, @function
_tee_s_intr_handler:
    /* Start by saving the general purpose registers and the PC value before
     * the interrupt happened. */
    save_general_regs RV_STK_FRMSZ
    save_mepc

    /* Though it is not necessary we save GP and SP here.
     * SP is necessary to help GDB to properly unwind
     * the backtrace of threads preempted by interrupts (OS tick etc.).
     * GP is saved just to have its proper value in GDB. */
    /* As gp register is not saved by the macro, save it here */
    sw      gp, RV_STK_GP(sp)
    /* Same goes for the SP value before trapping */
    addi    t0, sp, RV_STK_FRMSZ /* restore sp with the value when interrupt happened */
    /* Save SP */
    sw      t0, RV_STK_SP(sp)

    /* Check if the interrupt source is related to an APM exception */
    /* Define the addresses of the registers */
    li      t0, INTMTX_STATUS_REG_0
    /* Load the values from the registers */
    lw      t1, 0(t0)
    /* Define the masks */
    li      t2, TEE_SECURE_INT_APM_MASK_0
    /* Apply the masks */
    and     t1, t1, t2
    /* Check if any of the masked bits are set */
    bnez    t1, _save_reg_ctx

#if CONFIG_IDF_TARGET_ESP32C6
    /* Repeat for the other status register */
    li      t0, INTMTX_STATUS_REG_1
    lw      t1, 0(t0)
    li      t2, TEE_SECURE_INT_APM_MASK_1
    and     t1, t1, t2
    bnez    t1, _save_reg_ctx
#endif

    /* Continue normal execution */
    j       _continue

_save_reg_ctx:
    /* Save CSR context here */
    save_mcsr
    csrr    t0, mcause
    sw      t0, RV_STK_MCAUSE(sp)
    /* NOTE: With ESP-TEE, since APM violations trigger a panic, it's safe to use the mscratch
     * register to pass on the stack pointer to the APM violation handler */
    csrw    mscratch, sp
    j       _intr_hdlr_exec

_continue:
    /* Before doing anything preserve the stack pointer */
    mv      s11, sp
    /* Switch to the TEE interrupt stack */
    la      sp, _tee_intr_stack
    /* If this is a non-nested interrupt, SP now points to the interrupt stack */

    /* Before dispatch c handler, restore interrupt to enable nested intr */
    csrr    s1, mcause
    csrr    s2, mstatus

    /* TODO: [IDF-9972] Nested interrupts are not supported yet */
    # /* Save the interrupt threshold level */
    # li      t0, PLIC_MXINT_THRESH_REG /*INTERRUPT_CORE0_CPU_INT_THRESH_REG*/
    # lw      s3, 0(t0)

    # /* Increase interrupt threshold level */
    # li      t2, 0x7fffffff
    # and     t1, s1, t2       /* t1 = mcause & mask */
    # slli    t1, t1, 2        /* t1 = mcause * 4 */
    # li      t2, PLIC_MXINT0_PRI_REG /*INTC_INT_PRIO_REG(0)*/
    # add     t1, t2, t1       /* t1 = INTC_INT_PRIO_REG + 4 * mcause */
    # lw      t2, 0(t1)        /* t2 = INTC_INT_PRIO_REG[mcause] */
    # addi    t2, t2, 1        /* t2 = t2 +1 */
    # sw      t2, 0(t0)        /* INTERRUPT_CORE0_CPU_INT_THRESH_REG = t2 */
    # fence

    # csrsi   mstatus, MSTATUS_MIE
    # /* MIE set. Nested interrupts can now occur */

    #ifdef CONFIG_PM_TRACE
    li      a0, 0       /* = ESP_PM_TRACE_IDLE */
    #if SOC_CPU_CORES_NUM == 1
    li      a1, 0       /* No need to check core ID on single core hardware */
    #else
    csrr    a1, mhartid
    #endif
    la      t0, esp_pm_trace_exit
    jalr    t0          /* absolute jump, avoid the 1 MiB range constraint */
    #endif

    #ifdef CONFIG_PM_ENABLE
    la      t0, esp_pm_impl_isr_hook
    jalr    t0          /* absolute jump, avoid the 1 MiB range constraint */
    #endif

_intr_hdlr_exec:
    /* call the C dispatcher */
    mv      a0, sp      /* argument 1, stack pointer */
    mv      a1, s1      /* argument 2, interrupt number (mcause) */
    /* mask off the interrupt flag of mcause */
    li      t0, 0x7fffffff
    and     a1, a1, t0

    jal     esp_tee_global_interrupt_handler

    /* TODO: [IDF-9972] Nested interrupts are not supported yet */
    # csrci   mstatus, MSTATUS_MIE
    # /* MIE cleared. Nested interrupts are disabled */

    # /* restore the interrupt threshold level */
    # li      t0, PLIC_MXINT_THRESH_REG /*INTERRUPT_CORE0_CPU_INT_THRESH_REG*/
    # sw      s3, 0(t0)
    # fence

    /* restore the rest of the registers */
    csrw    mcause, s1
    csrw    mstatus, s2

    /* Restoring the stack pointer */
    mv      sp, s11

    restore_mepc
    restore_general_regs RV_STK_FRMSZ
    /* exit, this will also re-enable the interrupts */
    mret

    .size  _tee_s_intr_handler, .-_tee_s_intr_handler
