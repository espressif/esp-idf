/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Shared assembly helpers (macros + assembler-time constants) for the TEE
 * M-mode runtime. Included from esp_tee_vectors_{plic,clic}.S
 */

#pragma once

#include "sdkconfig.h"
#include "riscv/rvruntime-frames.h"

#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
#include "esp_private/hw_stack_guard.h"
#endif

/* Shared assembler-time constants used by the macros */
.equ SAVE_REGS, 32
.equ CONTEXT_SIZE, (SAVE_REGS * 4)
.equ MAGIC, 0x1f

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
    addi sp, sp, \cxt_size
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

/**
 * STACK_GUARD_PRE_SWITCH
 * Stops HW stack-guard monitoring and optionally saves current bounds.
 *
 * Args:
 *   op_reg  – output register for "monitoring enabled" state (must be reused)
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
