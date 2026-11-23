/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/system_reg.h"
#include "esp_private/privilege_trap.h"

/* When the IPC ISR interrupts U-mode code, the callback runs on the M-mode stack of the task, with the M-mode gp and
 * tp of the task. */

/* IPC_ISR handler */
    .equ SAVE_REGS, 16  /* count of saving regs: a0 - a7, t0 - t6, ra */
#if CONFIG_RISCV_USER_MODE_TRAPS
    .equ GP_OFFSET, (SAVE_REGS * 4)
    .equ TP_OFFSET, (GP_OFFSET + 4)
    .equ USER_SP_OFFSET, (TP_OFFSET + 4)
    /* The RISC-V calling convention requires a 16-byte aligned stack */
    .equ CONTEXT_SIZE, ((USER_SP_OFFSET + 4 + 15) & -16)
#else /* CONFIG_RISCV_USER_MODE_TRAPS */
    .equ CONTEXT_SIZE, (SAVE_REGS * 4)
#endif /* CONFIG_RISCV_USER_MODE_TRAPS */

.macro restore_regs
    lw      a0, 0(sp)
    lw      a1, 4(sp)
    lw      a2, 8(sp)
    lw      a3, 12(sp)
    lw      a4, 16(sp)
    lw      a5, 20(sp)
    lw      a6, 24(sp)
    lw      a7, 28(sp)
    lw      t0, 32(sp)
    lw      t1, 36(sp)
    lw      t2, 40(sp)
    lw      t3, 44(sp)
    lw      t4, 48(sp)
    lw      t5, 52(sp)
    lw      t6, 56(sp)
    lw      ra, 60(sp)
    addi    sp, sp, (CONTEXT_SIZE)
.endm

    .section    .iram1,"ax"
    .global     esp_ipc_isr_handler
    .type       esp_ipc_isr_handler,@function
esp_ipc_isr_handler:
#if CONFIG_RISCV_USER_MODE_TRAPS
    rv_user_trap_enter
#endif /* CONFIG_RISCV_USER_MODE_TRAPS */

    /* save a0 - a7, t0 - t6, ra */
    addi    sp, sp, -(CONTEXT_SIZE)
    sw      a0, 0(sp)
    sw      a1, 4(sp)
    sw      a2, 8(sp)
    sw      a3, 12(sp)
    sw      a4, 16(sp)
    sw      a5, 20(sp)
    sw      a6, 24(sp)
    sw      a7, 28(sp)
    sw      t0, 32(sp)
    sw      t1, 36(sp)
    sw      t2, 40(sp)
    sw      t3, 44(sp)
    sw      t4, 48(sp)
    sw      t5, 52(sp)
    sw      t6, 56(sp)
    sw      ra, 60(sp)
#if CONFIG_RISCV_USER_MODE_TRAPS
    rv_user_branch_if_user t0, esp_ipc_isr_from_user
esp_ipc_isr_run:
#endif /* CONFIG_RISCV_USER_MODE_TRAPS */

    /* MIE is cleared, so nested interrupts are disabled */

    call    esp_ipc_isr_record_interrupted_context

    /* Reset isr interrupt flags */
    li      a1, SYSTEM_CPU_INTR_FROM_CPU_2_REG
    csrr    a0, mhartid                            # Get CORE_ID
    beqz    a0, 1f
    li      a1, SYSTEM_CPU_INTR_FROM_CPU_3_REG
1:
    sw      zero, (a1)

    /* Set the start flag */
    la      a0, esp_ipc_isr_start_fl
    sw      a0, 0(a0)

    /* Call the esp_ipc_func(void* esp_ipc_func_arg) */
    lw      a1, (esp_ipc_func)
    lw      a0, (esp_ipc_func_arg)
    jalr    a1

    /* Set the end flag */
    la      a0, esp_ipc_isr_end_fl
    sw      a0, 0(a0)

#if CONFIG_RISCV_USER_MODE_TRAPS
    /* The callback must not take an exception that returns, so that mepc, mcause and MPP still hold
     * the values of the IPC interrupt */
    rv_user_branch_if_user t0, esp_ipc_isr_return_to_user
#endif /* CONFIG_RISCV_USER_MODE_TRAPS */
    /* Restore a0 - a7, t0 - t6, ra */
    restore_regs
    mret

#if CONFIG_RISCV_USER_MODE_TRAPS
esp_ipc_isr_from_user:
    sw      gp, GP_OFFSET(sp)
    sw      tp, TP_OFFSET(sp)
    rv_user_trap_from_user t0, CONTEXT_SIZE
    sw      t0, USER_SP_OFFSET(sp)
    j       esp_ipc_isr_run

esp_ipc_isr_return_to_user:
    lw      t0, USER_SP_OFFSET(sp)
    rv_user_trap_set_user_sp t0, CONTEXT_SIZE
    lw      gp, GP_OFFSET(sp)
    lw      tp, TP_OFFSET(sp)
    restore_regs
    rv_user_trap_exit_to_user
#endif /* CONFIG_RISCV_USER_MODE_TRAPS */
