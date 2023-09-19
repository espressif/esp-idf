/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/hp_system_reg.h"

/* IPC_ISR handler */
    .equ SAVE_REGS, 16  /* count of saving regs: a0 - a7, t0 - t6, ra */
    .equ CONTEXT_SIZE, (SAVE_REGS * 4)
    .section    .iram1,"ax"
    .global     esp_ipc_isr_handler
    .type       esp_ipc_isr_handler,@function
esp_ipc_isr_handler:

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

    /* MIE is cleared, so nested interrupts are disabled */

    /* Reset isr interrupt flags */
    li      a1, HP_SYSTEM_CPU_INT_FROM_CPU_2_REG
    csrr    a0, mhartid                            # Get CORE_ID
    beqz    a0, 1f
    li      a1, HP_SYSTEM_CPU_INT_FROM_CPU_3_REG
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

    /* Restore a0 - a7, t0 - t6, ra */
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

    mret
