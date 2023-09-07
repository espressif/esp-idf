/*
 * SPDX-FileCopyrightText: 2017-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc.h"
#include "soc/interrupt_reg.h"
#include "riscv/rvruntime-frames.h"
#include "soc/soc_caps.h"
#include "sdkconfig.h"
#include "esp_private/vectors_const.h"


    .equ SAVE_REGS, 32
    .equ CONTEXT_SIZE, (SAVE_REGS * 4)
    .equ panic_from_exception, xt_unhandled_exception
    .equ panic_from_isr, panicHandler

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
    csrr t0, mepc
    sw   t0, RV_STK_MEPC(sp)
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

    .global rtos_int_enter
    .global rtos_int_exit
    .global _global_interrupt_handler
#ifdef CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
    .global gdbstub_handle_debug_int
#endif

    .section .exception_vectors.text

    /* Exception handler.*/
    .type _panic_handler, @function
    .global _panic_handler
_panic_handler:
    /* Allocate space on the stack and store general purpose registers */
    save_general_regs RV_STK_FRMSZ

    /* As gp register is not saved by the macro, save it here */
    sw    gp, RV_STK_GP(sp)

    /* Same goes for the SP value before trapping */
    addi  t0, sp, RV_STK_FRMSZ /* restore sp with the value when trap happened */

    /* Save CSRs */
    sw    t0, RV_STK_SP(sp)
    csrr  t0, mepc
    sw    t0, RV_STK_MEPC(sp)
    csrr  t0, mstatus
    sw    t0, RV_STK_MSTATUS(sp)
    csrr  t0, mtvec
    sw    t0, RV_STK_MTVEC(sp)
    csrr  t0, mtval
    sw    t0, RV_STK_MTVAL(sp)
    csrr  t0, mhartid
    sw    t0, RV_STK_MHARTID(sp)

    /* Call panic_from_exception(sp) or panic_from_isr(sp)
     * depending on whether we have a pseudo excause or not.
     * If mcause's highest bit is 1, then an interrupt called this routine,
     * so we have a pseudo excause. Else, it is due to a exception, we don't
     * have an pseudo excause */
    mv    a0, sp
    csrr  a1, mcause

    /* Only keep the interrupt bit and the source cause of the trap */
    li t1, VECTORS_MCAUSE_INTBIT_MASK | VECTORS_MCAUSE_REASON_MASK
    and a1, a1, t1

    /* Branches instructions don't accept immediate values, so use t1 to
     * store our comparator */
    li    t0, 0x80000000
    bgeu  a1, t0, _call_panic_handler
    sw    a1, RV_STK_MCAUSE(sp)
#ifdef CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
    li    t0, 3
    beq   a1, t0, _call_gdbstub_handler
#endif
    /* exception_from_panic never returns */
    jal panic_from_exception
    /* We arrive here if the exception handler has returned. */
    j _return_from_exception

#ifdef CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
_call_gdbstub_handler:
    call  gdbstub_handle_debug_int
    j     _return_from_exception
#endif

_call_panic_handler:
    /* Remove highest bit from mcause (a1) register and save it in the
     * structure */
    not   t0, t0
    and   a1, a1, t0
#if CONFIG_SOC_INT_CLIC_SUPPORTED
    /* When CLIC is supported, external interrupts are shifted by 16, deduct this difference from mcause */
    add a1, a1, -16
#endif // CONFIG_SOC_INT_CLIC_SUPPORTED
    sw    a1, RV_STK_MCAUSE(sp)
    jal panic_from_isr

    /* We arrive here if the exception handler has returned. This means that
     * the exception was handled, and the execution flow should resume.
     * Restore the registers and return from the exception.
     */
_return_from_exception:
    restore_mepc
    /* MTVEC and SP are assumed to be unmodified.
     * MSTATUS, MHARTID, MTVAL are read-only and not restored.
     */
    lw gp,  RV_STK_GP(sp)
    restore_general_regs RV_STK_FRMSZ
    mret
    .size  _panic_handler, .-_panic_handler


    /* This is the interrupt handler.
     * It saves the registers on the stack,
     * prepares for interrupt nesting,
     * re-enables the interrupts,
     * then jumps to the C dispatcher in interrupt.c.
     */
    .global _interrupt_handler
    .type _interrupt_handler, @function
_interrupt_handler:
    /* Start by saving the general purpose registers and the PC value before
     * the interrupt happened. */
    save_general_regs
    save_mepc

    /* Though it is not necessary we save GP and SP here.
     * SP is necessary to help GDB to properly unwind
     * the backtrace of threads preempted by interrupts (OS tick etc.).
     * GP is saved just to have its proper value in GDB. */
    /* As gp register is not saved by the macro, save it here */
    sw    gp, RV_STK_GP(sp)
    /* Same goes for the SP value before trapping */
    addi  t0, sp, CONTEXT_SIZE /* restore sp with the value when interrupt happened */
    /* Save SP */
    sw    t0, RV_STK_SP(sp)

    /* Notify the RTOS that an interrupt ocurred, it will save the current stack pointer
     * in the running TCB, no need to pass it as a parameter */
    call    rtos_int_enter
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
    li      t2, INTC_INT_PRIO_REG(0)
    add     t1, t2, t1       /* t1 = INTC_INT_PRIO_REG + 4 * mcause */
    lw      t2, 0(t1)        /* t2 = INTC_INT_PRIO_REG[mcause] */
    addi    t2, t2, 1        /* t2 = t2 +1 */
    sw      t2, 0(t0)        /* INTERRUPT_CURRENT_CORE_INT_THRESH_REG = t2 */
    fence
#endif // !SOC_INT_HW_NESTED_SUPPORTED

    li      t0, 0x8
    csrrs   t0, mstatus, t0
    /* MIE set. Nested interrupts can now occur */

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

    /* call the C dispatcher */
    mv      a0, sp      /* argument 1, stack pointer */
    mv      a1, s1      /* argument 2, interrupt number (mcause) */
    /* mask off the interrupt flag of mcause */
    li	    t0, VECTORS_MCAUSE_REASON_MASK
    and     a1, a1, t0
    jal     _global_interrupt_handler

    /* After dispatch c handler, disable interrupt to make freertos make context switch */

    li      t0, 0x8
    csrrc   t0, mstatus, t0
    /* MIE cleared. Nested interrupts are disabled */

#if !SOC_INT_HW_NESTED_SUPPORTED
    /* restore the interrupt threshold level */
    li      t0, INTERRUPT_CURRENT_CORE_INT_THRESH_REG
    sw      s3, 0(t0)
    fence
#endif // !SOC_INT_HW_NESTED_SUPPORTED

    /* The RTOS will restore the current TCB stack pointer. This routine will preserve s1 and s2 but alter s0. */
    call    rtos_int_exit

    /* Restore the rest of the registers.
     * In case the target uses the CLIC, it is mandatory to restore `mcause` register since it contains
     * the former CPU priority. When executing `mret`, the hardware will restore the former threshold,
     * from `mcause` to `mintstatus` CSR */
    csrw    mcause, s1
    csrw    mstatus, s2
    restore_mepc
    restore_general_regs

    /* exit, this will also re-enable the interrupts */
    mret
    .size  _interrupt_handler, .-_interrupt_handler
