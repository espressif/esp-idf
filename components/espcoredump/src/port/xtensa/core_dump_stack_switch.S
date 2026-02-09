/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Assembly implementation for switching to a new stack during coredump.
 *
 * This implementation properly handles window spilling without lowering the
 * interrupt level, making it safe to use from high-priority interrupt contexts
 * (e.g., interrupt watchdog at level 4).
 */

#include <xtensa/coreasm.h>
#include <xtensa/corebits.h>
#include <xtensa/xtruntime.h>
#include "xt_asm_utils.h"
#include "xt_instr_macros.h"  /* For SET_STACK macro */
#include "sdkconfig.h"

    .extern esp_core_dump_write_elf_and_check
    .extern esp_core_dump_report_stack_usage
    .extern esp_core_dump_report_backup_stack
    .extern esp_core_dump_report_restore_stack

/**
 * @brief Stack context structure layout (offsets in bytes)
 *
 * This structure defines the layout of s_stack_context used for saving/restoring
 * the original stack context when switching to the coredump stack.
 */
    .equ    STACK_CTX_SP,          0
    .equ    STACK_CTX_A0,          4
    .equ    STACK_CTX_PS,          8
    .equ    STACK_CTX_WINDOWBASE,  12
    .equ    STACK_CTX_WINDOWSTART, 16
    .equ    STACK_CTX_SIZE,        20

    .section .bss
    .align  4
    .global s_stack_context
    .type   s_stack_context,@object
s_stack_context:
    .space  STACK_CTX_SIZE
    .size   s_stack_context, STACK_CTX_SIZE

    .text

/**
 * @brief Execute core dump write on a new stack
 *
 * This function spills register windows and switches to new stack.
 * Calls C functions and restores original stack and context.
 */
#if CONFIG_ESP_COREDUMP_STACK_SIZE > 0

    .global     esp_core_dump_port_write_on_new_stack
    .type       esp_core_dump_port_write_on_new_stack,@function
    .align      4
esp_core_dump_port_write_on_new_stack:

#ifndef __XTENSA_CALL0_ABI__
    entry   a1, 32

    /* Save necessary parameters on stack before window spilling */
    s32i    a0, a1, 0            /* Save return address */
    s32i    a2, a1, 4            /* Save coredump_sp parameter */

    /* Prepare for window spilling:
     * - Clear PS.EXCM and set PS.WOE to enable window overflow exceptions
     * - Raise INTLEVEL to at least XCHAL_EXCM_LEVEL (to maintain interrupt masking)
     * - Save/restore EPC1 (clobbered by window overflow exceptions)
     * - Only use a0-a3 here (a4-a15 may hold live data from previous windows)
     */
    rsr     a2, XT_REG_PS              /* Save PS to be restored after SPILL_ALL_WINDOWS */
    movi    a0, PS_INTLEVEL_MASK
    and     a3, a2, a0                 /* Extract current INTLEVEL */
    bgeui   a3, XCHAL_EXCM_LEVEL, 1f   /* Ensure INTLEVEL >= XCHAL_EXCM_LEVEL */
    movi    a3, XCHAL_EXCM_LEVEL
1:
    movi    a0, PS_UM | PS_WOE         /* Clear EXCM, enable WOE */
    or      a3, a3, a0
    wsr     a3, XT_REG_PS
    rsr     a0, EPC1                   /* Save EPC1 to be restored after SPILL_ALL_WINDOWS */

    SPILL_ALL_WINDOWS

    /* Restore PS and EPC1 */
    wsr     a2, XT_REG_PS
    rsync
    wsr     a0, EPC1

    /* Restore return address and save context to s_stack_context */
    mov     a6, a1              /* a6 = SP */
    l32i    a7, a1, 0           /* a7 = return address (from stack) */
    l32i    a12, a1, 4          /* a12 = coredump_sp (from stack) */
    rsr     a8, XT_REG_PS
    rsr     a9, XT_REG_WINDOWBASE
    rsr     a10, XT_REG_WINDOWSTART
    movi    a11, s_stack_context
    s32i    a6, a11, STACK_CTX_SP          /* Save SP */
    s32i    a7, a11, STACK_CTX_A0          /* Save A0 */
    s32i    a8, a11, STACK_CTX_PS          /* Save PS */
    s32i    a9, a11, STACK_CTX_WINDOWBASE  /* Save WindowBase */
    s32i    a10, a11, STACK_CTX_WINDOWSTART /* Save WindowStart */

    /* Switch to coredump stack (use saved coredump_sp from stack) */
    mov     a6, a12             /* a6 = coredump_sp */
    SET_STACK a6, a7, a8

    /* Report backup stack */
    l32i    a10, a11, STACK_CTX_SP  /* a10 = old SP */
    movi    a14, esp_core_dump_report_backup_stack
    callx8  a14

    /* Write ELF core dump */
    movi    a14, esp_core_dump_write_elf_and_check
    callx8  a14

    /* Report stack usage */
    mov     a10, a12            /* a10 = coredump_sp */
    movi    a14, esp_core_dump_report_stack_usage
    callx8  a14

    /* Report restore stack */
    l32i    a10, a11, STACK_CTX_SP  /* a10 = old SP */
    movi    a14, esp_core_dump_report_restore_stack
    callx8  a14

    /* Load original context from s_stack_context */
    movi    a11, s_stack_context
    l32i    a2, a11, STACK_CTX_SP          /* Load SP */
    l32i    a3, a11, STACK_CTX_A0          /* Load A0 */
    l32i    a4, a11, STACK_CTX_PS          /* Load PS */
    l32i    a5, a11, STACK_CTX_WINDOWBASE  /* Load WindowBase */
    l32i    a6, a11, STACK_CTX_WINDOWSTART /* Load WindowStart */

    /* Write PS with WOE disabled */
    movi    a8, ~PS_WOE_MASK
    and     a7, a4, a8
    wsr     a7, XT_REG_PS
    rsync

    /* Restore WindowBase */
    wsr     a5, XT_REG_WINDOWBASE
    rsync

    /* Restore WindowStart */
    wsr     a6, XT_REG_WINDOWSTART
    rsync

    /* Restore SP */
    mov     a1, a2

    /* Restore A0 */
    mov     a0, a3

    /* Restore PS (final with original WOE setting) */
    wsr     a4, XT_REG_PS
    rsync

    retw

#else
    #error "this code is written for Window ABI"
#endif

#endif // CONFIG_ESP_COREDUMP_STACK_SIZE > 0
