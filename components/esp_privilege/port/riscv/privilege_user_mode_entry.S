/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Entry into U-mode on the calling task, and the return to its M-mode caller */

#include "sdkconfig.h"
#include "riscv/encoding.h"
#include "esp_private/privilege_trap.h"
#include "esp_privilege_abi.h"

    .section .iram1, "ax"

/**
 * @brief Call a function in U-mode and return its value
 *
 * The caller's context is saved at the top of its stack, which becomes the M-mode stack of
 * the task until the run ends.
 *
 * @param a0 U-mode entry point
 * @param a1 First argument passed to the entry point
 * @param a2 Second argument passed to the entry point
 * @param a3 Initial user sp, 16-byte aligned
 * @param a4 satp value of the page tables that the U-mode code runs with
 * @param a5 Run context
 * @return The low word in a0 is the user result. The high word in a1 is zero
 */
    .global esp_privilege_call_user_code
    .type esp_privilege_call_user_code, @function
esp_privilege_call_user_code:
    addi    sp, sp, -RV_UCTX_SIZE
    sw      ra, RV_UCTX_RA(sp)
    sw      gp, RV_UCTX_GP(sp)
    sw      tp, RV_UCTX_TP(sp)
    sw      s0, RV_UCTX_S0(sp)
    sw      s1, RV_UCTX_S1(sp)
    sw      s2, RV_UCTX_S2(sp)
    sw      s3, RV_UCTX_S3(sp)
    sw      s4, RV_UCTX_S4(sp)
    sw      s5, RV_UCTX_S5(sp)
    sw      s6, RV_UCTX_S6(sp)
    sw      s7, RV_UCTX_S7(sp)
    sw      s8, RV_UCTX_S8(sp)
    sw      s9, RV_UCTX_S9(sp)
    sw      s10, RV_UCTX_S10(sp)
    sw      s11, RV_UCTX_S11(sp)
    sw      a4, RV_UCTX_SATP(sp)
    sw      zero, RV_UCTX_FAULT(sp)

    /* Nothing must trap until mret. From now on, sp is the top of the M-mode stack */
    csrci   mstatus, MSTATUS_MIE
    /* The caller flushed translations and instruction fetch on every core after the page tables and
     * the code last changed */
    rv_user_activate_satp a4, t0
    csrw    mepc, a0
    li      t0, MSTATUS_MPP
    csrc    mstatus, t0
    li      t0, MSTATUS_MPIE
    csrs    mstatus, t0

    mv      a0, a1
    mv      a1, a2
    lw      gp, RV_URUN_GP(a5)
    lw      tp, RV_URUN_TP(a5)
    lw      ra, RV_URUN_RETURN(a5)
#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    /* U-mode code runs on its own stack, outside the task's stack bounds */
    ESP_HW_STACK_GUARD_MONITOR_STOP_CUR_CORE t0 t1
#endif /* CONFIG_ESP_SYSTEM_HW_STACK_GUARD */
    /* Do not leak M-mode register values to U-mode. */
    li      t0, 0
    li      t1, 0
    li      t2, 0
    li      s0, 0
    li      s1, 0
    li      a2, 0
    csrw    mscratch, a3
    li      a3, 0
    li      a4, 0
    li      a5, 0
    li      a6, 0
    li      a7, 0
    li      s2, 0
    li      s3, 0
    li      s4, 0
    li      s5, 0
    li      s6, 0
    li      s7, 0
    li      s8, 0
    li      s9, 0
    li      s10, 0
    li      s11, 0
    li      t3, 0
    li      t4, 0
    li      t5, 0
    li      t6, 0
    csrrw   sp, mscratch, sp
    mret
    .size esp_privilege_call_user_code, .-esp_privilege_call_user_code

/**
 * @brief Resume the caller of esp_privilege_call_user_code, in M-mode
 *
 * The ecall handler redirects the task here, with sp pointing to the top of the M-mode stack
 * and a0 holding the value returned by the U-mode code.
 */
    .global esp_privilege_return_to_caller
    .type esp_privilege_return_to_caller, @function
esp_privilege_return_to_caller:
    lw      ra, RV_UCTX_RA(sp)
    lw      gp, RV_UCTX_GP(sp)
    lw      tp, RV_UCTX_TP(sp)
    lw      s0, RV_UCTX_S0(sp)
    lw      s1, RV_UCTX_S1(sp)
    lw      s2, RV_UCTX_S2(sp)
    lw      s3, RV_UCTX_S3(sp)
    lw      s4, RV_UCTX_S4(sp)
    lw      s5, RV_UCTX_S5(sp)
    lw      s6, RV_UCTX_S6(sp)
    lw      s7, RV_UCTX_S7(sp)
    lw      s8, RV_UCTX_S8(sp)
    lw      s9, RV_UCTX_S9(sp)
    lw      s10, RV_UCTX_S10(sp)
    lw      s11, RV_UCTX_S11(sp)
    /* RV32 returns a uint64_t in a0 and a1. a0 holds the value of the U-mode code, a1 is zero */
    lw      a1, RV_UCTX_FAULT(sp)
    addi    sp, sp, RV_UCTX_SIZE
    ret
    .size esp_privilege_return_to_caller, .-esp_privilege_return_to_caller
