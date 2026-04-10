/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "soc/clic_reg.h"

#include "riscv/encoding.h"
#include "riscv/rvruntime-frames.h"
#include "esp_private/vectors_const.h"

#include "esp_tee.h"
#include "esp_tee_intr_defs.h"
#include "sdkconfig.h"

#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
#include "esp_private/hw_stack_guard.h"
#endif

    .equ SAVE_REGS, 32
    .equ CONTEXT_SIZE, (SAVE_REGS * 4)
    .equ panic_from_exception, tee_panic_from_exc
    .equ panic_from_isr, tee_panic_from_isr
    .equ MAGIC, 0x1f
    .equ RTNVAL, 0xc0de
    .equ ECALL_U_MODE, 0x8
    .equ ECALL_M_MODE, 0xb
    .equ CSR_UINTTHRESH, 0x047
    .equ CSR_MINTTHRESH, 0x347

    .global esp_tee_global_interrupt_handler
    .global esp_tee_service_dispatcher
    .global _tee_s_entry
    .global syscall_enter_tee
    .global syscall_exit_tee

    .section .data
    .balign 4

    .global _ns_sp
_ns_sp:
    .word 0

    .global _s_sp
_s_sp:
    .word 0

    .global _s_intr_thresh
_s_intr_thresh:
    .word 0

    .global _ns_sp_min
_ns_sp_min:
    .word 0

    .global _ns_sp_max
_ns_sp_max:
    .word 0

/**
 * STACK_GUARD_PRE_SWITCH
 * Stops HW stack-guard monitoring and optionally saves current bounds.
 *
 * Args:
 *   op_reg  – output register for “monitoring enabled” state (must be reused)
 *   to_save – 1=save bounds to memory, 0=skip saving
 *   sp_min  – symbol to store lower bound (if to_save=1)
 *   sp_max  – symbol to store upper bound (if to_save=1)
 *
 * Clobbers: t0, t1, t2, op_reg
 */
.macro STACK_GUARD_PRE_SWITCH op_reg, to_save, sp_min, sp_max
#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    /* Query if monitoring is enabled: result goes into \op_reg */
    ESP_HW_STACK_GUARD_MONITOR_QUERY_CUR_CORE t2 \op_reg
    beqz    \op_reg, 1f

    .if \to_save
        /* Save current REE/U-mode stack bounds */
        ESP_HW_STACK_GUARD_GET_BOUNDS_CUR_CORE t2 t0 t1
        la t2, \sp_min
        sw t0, 0(t2)
        la t2, \sp_max
        sw t1, 0(t2)
    .endif

    /* Stop monitoring */
    ESP_HW_STACK_GUARD_MONITOR_STOP_CUR_CORE t0 t1
    fence
1:
#endif
.endm

/**
 * STACK_GUARD_POST_SWITCH
 * Restores or applies new bounds after switching stacks, then restarts monitoring.
 *
 * Args:
 *   op_reg     – saved monitoring state from PRE_SWITCH
 *   to_restore – 1=restore from memory, 0=set static bounds
 *   sp_min     – saved bound (restore) or static lower bound (S-mode)
 *   sp_max     – saved bound (restore) or static upper bound (S-mode)
 *
 * Clobbers: t0, t1, t2
 */
.macro STACK_GUARD_POST_SWITCH op_reg, to_restore, sp_min, sp_max
#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    /* Check if monitoring was enabled (using saved state from op_reg) */
    beqz    \op_reg, 1f

    .if \to_restore
        /* Restore saved REE/U-mode bounds from memory */
        la  t2, \sp_min
        lw  t0, 0(t2)
        la  t2, \sp_max
        lw  t1, 0(t2)
    .else
        /* Use new TEE/S-mode stack bounds (static symbols) */
        la  t0, \sp_min
        la  t1, \sp_max
    .endif

    /* Apply bounds to hardware stack guard */
    ESP_HW_STACK_GUARD_SET_BOUNDS_CUR_CORE t2 t0 t1
    /* Restart monitoring */
    ESP_HW_STACK_GUARD_MONITOR_START_CUR_CORE t0 t1
1:
#endif
.endm

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
    csrr  t0, mcause
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
    .global _tee_exception_handler
    .global _tee_panic_handler
    .type _tee_panic_handler, @function
_tee_exception_handler:
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
    beq     t0, t1, _machine_ecall

    /* Check whether the exception is an U-mode ecall */
    li      t1, ECALL_U_MODE
    beq     t0, t1, _user_ecall

    /* Restore t0, t1 from the stack */
    lw      t0, 0(sp)
    lw      t1, 4(sp)
    addi    sp, sp, 16

_tee_panic_handler:
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

    /* Keep mcause in s0, only the exception code and interrupt bit are relevant */
    csrr  s0, mcause
    li    t1, VECTORS_MCAUSE_INTBIT_MASK | VECTORS_MCAUSE_REASON_MASK
    and   s0, s0, t1

    /* Call panic_from_exception(sp) or panic_from_isr(sp)
     * depending on whether we have a pseudo excause or not.
     * If mcause's highest bit is 1, then an interrupt called this routine,
     * so we have a pseudo excause. Else, it is due to a exception, we don't
     * have an pseudo excause */
    mv      a0, sp
    mv      a1, s0
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
#if CONFIG_SOC_INT_CLIC_SUPPORTED
    /* When CLIC is supported, external interrupts are shifted by 16, deduct this difference from mcause */
    add   a1, a1, -16
#endif // CONFIG_SOC_INT_CLIC_SUPPORTED
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

    .size  _tee_exception_handler, .-_tee_exception_handler

    /* ECALL handler. */
    .type _ecall_handler, @function
_ecall_handler:
    /* M-mode ecall handler */
_machine_ecall:
    /* Enable the U-mode delegation of all interrupts */
    li      t0, INTMTX_SIG_IDX_ASSERT_IN_SEC_REG
    sw      zero, 0(t0)
    fence
    /* Verify the above */
_1:
    lw      t1, 0(t0)
    bnez    t1, _1

    /* Set the privilege mode to transition to after mret to U-mode  */
    li      t0, MSTATUS_MPP
    csrc    mstatus, t0

    /* Check whether this is the first M-mode ecall (see esp_tee_init) and skip context restoration */
    lui     t0, ESP_TEE_M2U_SWITCH_MAGIC
    beq     a1, t0, _skip_ctx_restore

    /* Check if we need to restore the MINTTHRESH register */
    la      t0, _s_intr_thresh
    lw      t1, 0(t0)
    beqz    t1, _skip_thresh_restore     # if saved threshold == 0, skip

    /* Restore CSR_MINTTHRESH */
    sw      zero, 0(t0)
    csrw    CSR_MINTTHRESH, t1

_skip_thresh_restore:
    STACK_GUARD_PRE_SWITCH t3 0

    /* Switching back to the saved REE stack */
    la      t0, _ns_sp
    lw      sp, 0(t0)

    STACK_GUARD_POST_SWITCH t3 1 _ns_sp_min _ns_sp_max
    fence

    call syscall_exit_tee

    /* Backup the A0 register
     * This point is reached after an ecall is triggered after executing the secure service.
     * The A0 register contains the return value of the corresponding service.
     * After restoring the entire register context, we assign A0 the value back to the return value. */
    csrw    mscratch, a0
    restore_general_regs
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
    csrrw   t1, mscratch, zero
    beq     t0, t1, _rtn_from_ns_int

    /* Restore t0, t1 from the stack */
    lw      t0, 0(sp)
    lw      t1, 4(sp)
    addi    sp, sp, 16

    /* This point is reached when a secure service call is issued from the REE */
    /* Save register context and mepc */
    save_general_regs
    save_mepc

    # Check if REE is in a critical section
    csrr    t0, CSR_UINTTHRESH      # t0 = current UINTTHRESH
    beqz    t0, _process_ecall      # if threshold == 0 -> continue

    # Save current MINTTHRESH and mirror UINTTHRESH into MINTTHRESH
    csrr    t1, CSR_MINTTHRESH      # t1 = current MINTTHRESH
    la      t2, _s_intr_thresh
    sw      t1, 0(t2)               # save old MINTTHRESH
    csrw    CSR_MINTTHRESH, t0      # set MINTTHRESH = UINTTHRESH

_process_ecall:
    /* Save the U-mode (i.e. REE) stack pointer */
    la      t0, _ns_sp
    sw      sp, 0(t0)

    STACK_GUARD_PRE_SWITCH t3 1 _ns_sp_min _ns_sp_max

    /* Switch to the M-mode (i.e. TEE) stack */
    la      sp, _tee_stack

    STACK_GUARD_POST_SWITCH t3 0 _tee_stack_bottom _tee_stack

    /* Load the TEE entry point (see _tee_s_entry) in the mepc */
    la      t0, _tee_s_entry
    csrw    mepc, t0

    /* Disable the U-mode delegation of all interrupts */
    li      t0, INTMTX_SIG_IDX_ASSERT_IN_SEC_REG
    li      t1, TEE_PASS_INUM + CLIC_EXT_INTR_NUM_OFFSET
    sw      t1, 0(t0)
    fence
    /* Verify the above */
_2:
    lw      t2, 0(t0)
    bne     t2, t1, _2

    /* Set the privilege mode to transition to after mret to M-mode  */
    li      t0, MSTATUS_MPP
    csrs    mstatus, t0

    call syscall_enter_tee

    mret

    /* This point is reached after servicing a U-mode interrupt occurred
     * while executing a secure service */
_rtn_from_ns_int:
    /* Disable the U-mode interrupt delegation */
    li      t0, INTMTX_SIG_IDX_ASSERT_IN_SEC_REG
    li      t1, TEE_PASS_INUM + CLIC_EXT_INTR_NUM_OFFSET
    sw      t1, 0(t0)
    fence
    /* Verify the above */
_3:
    lw      t2, 0(t0)
    bne     t2, t1, _3

    STACK_GUARD_PRE_SWITCH t3 0

    /* Restore the secure stack pointer */
    la      t0, _s_sp
    lw      sp, 0(t0)

    STACK_GUARD_POST_SWITCH t3 0 _tee_stack_bottom _tee_stack

    /* Set the privilege mode to transition to after mret to M-mode */
    li      t0, MSTATUS_MPP
    csrs    mstatus, t0

    call syscall_enter_tee

    /* Restore register context and resume the secure service */
    restore_mepc
    restore_general_regs

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
    save_general_regs
    save_mepc

    /* Though it is not necessary we save GP and SP here.
     * SP is necessary to help GDB to properly unwind
     * the backtrace of threads preempted by interrupts (OS tick etc.).
     * GP is saved just to have its proper value in GDB. */
    /* As gp register is not saved by the macro, save it here */
    sw      gp, RV_STK_GP(sp)
    /* Same goes for the SP value before trapping */
    addi    t0, sp, CONTEXT_SIZE /* restore sp with the value when interrupt happened */
    /* Save SP */
    sw      t0, RV_STK_SP(sp)

    /*
     * NOTE: Since this is a delegated U-mode interrupt routed through M-mode, the U-mode CSRs
     * (like ucause, uintstatus, and uintthresh) are not automatically populated by hardware.
     *
     * To correctly simulate the U-mode interrupt context, we must explicitly update:
     * - ucause: to reflect the interrupt source (including EXCCODE, INTBIT, and PIE).
     * - uintthresh: to the level of the interrupt being serviced.
     *
     * uintstatus is read-only and will be zero in this scenario.
     * However, CLIC uses max(uintstatus.uil, uintthresh.th) to determine active interrupt level.
     * So, setting uintthresh to the current interrupt level ensures that only higher-priority
     * U-mode interrupts can preempt while this one is being handled.
     */
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
    addi    t6, t1, CLIC_EXT_INTR_NUM_OFFSET

    /* Update ucause */
    csrr    t0, ucause
    li      t1, VECTORS_MCAUSE_REASON_MASK
    not     t1, t1
    and     t0, t0, t1
    or      t0, t0, t6
    li      t1, VECTORS_MCAUSE_INTBIT_MASK
    or      t0, t0, t1
    csrw    ucause, t0

    /* Update the new the U-mode interrupt threshold */
    li      t1, DR_REG_CLIC_CTRL_BASE
    slli    t2, t6, 2
    add     t1, t1, t2
    lw      t2, 0(t1)
    fence
    srli    t2, t2, CLIC_INT_CTL_S
    csrw    CSR_UINTTHRESH, t2

    /* Enable the U-mode interrupt delegation */
    li      t0, INTMTX_SIG_IDX_ASSERT_IN_SEC_REG
    sw      zero, 0(t0)
    fence
    /* Verify the above */
_4:
    lw      t1, 0(t0)
    bnez    t1, _4

    /* For U-mode interrupts, we use mret to switch to U-mode after executing the below steps - */
    /* Disable the U-mode global interrupts */
    csrci    ustatus, USTATUS_UIE

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

    /* Save the current secure stack pointer */
    la     t0, _s_sp
    sw     sp, 0(t0)

    STACK_GUARD_PRE_SWITCH t3 0

    /* Switch to the saved U-mode stack */
    la     t1, _ns_sp
    lw     sp, 0(t1)
    fence

    /* Set a flag to identify the next U2M switch would be after handling a U-mode interrupt */
    lui     t0, RTNVAL
    csrw    mscratch, t0

    STACK_GUARD_POST_SWITCH t3 1 _ns_sp_min _ns_sp_max

    call syscall_exit_tee

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

    /* Repeat for the other status register */
    li      t0, INTMTX_STATUS_REG_1
    lw      t1, 0(t0)
    li      t2, TEE_SECURE_INT_APM_MASK_1
    and     t1, t1, t2
    bnez    t1, _save_reg_ctx

    /* Continue normal execution */
    j       _continue

_save_reg_ctx:
    /* Save CSR context here */
    save_mcsr
    /* NOTE: With ESP-TEE, since APM violations trigger a panic, it's safe to use the mscratch
     * register to pass on the stack pointer to the APM violation handler */
    csrw    mscratch, sp
    j       _intr_hdlr_exec

_continue:
    /* Before doing anything preserve the stack pointer */
    mv      s11, sp

    /* NOTE: Cannot use the stack-guard macros here because they expect
     * static symbols, not register-based bounds. */
#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    /* Check if stack monitoring is enabled */
    ESP_HW_STACK_GUARD_MONITOR_QUERY_CUR_CORE t2 t3
    beqz    t3, 1f

    /* Save the REE stack guard bounds in s9/s10 (callee-saved registers) */
    ESP_HW_STACK_GUARD_GET_BOUNDS_CUR_CORE t2 s9 s10
    /* Stop the stack monitoring */
    ESP_HW_STACK_GUARD_MONITOR_STOP_CUR_CORE t0 t1
    fence
1:
#endif

    /* Switch to the TEE interrupt stack */
    la      sp, _tee_intr_stack

    STACK_GUARD_POST_SWITCH t3 0 _tee_intr_stack_bottom _tee_intr_stack
    fence

    /* If this is a non-nested interrupt, SP now points to the interrupt stack */

    /* Before dispatch c handler, restore interrupt to enable nested intr */
    csrr    s1, mcause
    csrr    s2, mstatus

    /* TODO: [IDF-9972] Nested interrupts are not supported yet */
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
    li      t0, VECTORS_MCAUSE_REASON_MASK
    and     a1, a1, t0

    jal     esp_tee_global_interrupt_handler

    /* TODO: [IDF-9972] Nested interrupts are not supported yet */
    # csrci   mstatus, MSTATUS_MIE
    # /* MIE cleared. Nested interrupts are disabled */

    /* restore the rest of the registers */
    csrw    mcause, s1
    csrw    mstatus, s2

    STACK_GUARD_PRE_SWITCH t3 0

    /* Restoring the stack pointer */
    mv      sp, s11

    /* NOTE: Cannot use the stack-guard macros here because they expect
     * static symbols, not register-based bounds. */
#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    /* Check if stack monitoring was enabled (using saved state from t3) */
    beqz    t3, 2f  # Skip if monitoring was not enabled

    /* Restore the REE stack bounds from s9/s10 (callee-saved registers) */
    ESP_HW_STACK_GUARD_SET_BOUNDS_CUR_CORE t2 s9 s10
    /* Resume stack monitoring */
    ESP_HW_STACK_GUARD_MONITOR_START_CUR_CORE t0 t1
2:
#endif

    restore_mepc
    restore_general_regs RV_STK_FRMSZ
    /* exit, this will also re-enable the interrupts */
    mret

    .size  _tee_s_intr_handler, .-_tee_s_intr_handler

    .section .text, "ax"

    .balign  4
    .type _tee_s_entry, @function
_tee_s_entry:
    /* Jump to the secure service dispatcher */
    jal     esp_tee_service_dispatcher

    /* Fire an M-ecall */
    mv      a1, zero
    ecall
    fence

    .size  _tee_s_entry, .-_tee_s_entry
