/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <xtensa/coreasm.h>
#include <xtensa/corebits.h>
#include <xtensa/config/system.h>
#include "xtensa_context.h"

    .section    .iram1,"ax"
    .global     _xt_panic_gdbstub
    .type       _xt_panic_gdbstub,@function
    .align      4
    .literal_position
    .align      4

_xt_panic_gdbstub:
    /* Allocate exception frame and save minimal context. */
    addi    sp, sp, -XT_STK_FRMSZ
    s32i    a0, sp, XT_STK_EXIT
    s32i    a0, sp, XT_STK_A0

    rsr     a0, PS                          /* save interruptee's PS */
    s32i    a0, sp, XT_STK_PS
    rsr     a0, EPC_1                       /* save interruptee's PC */
    s32i    a0, sp, XT_STK_PC
    call0   _xt_context_save                /* Save full context*/
    addi     a7, sp, XT_STK_FRMSZ
    s32i    a7, sp, XT_STK_A1
    s32i    a12, sp, XT_STK_A12
    s32i    a13, sp, XT_STK_A13

    /* Save exc cause and vaddr into exception frame */
    rsr     a0, EXCCAUSE
    s32i    a0, sp, XT_STK_EXCCAUSE
    rsr     a0, EXCVADDR
    s32i    a0, sp, XT_STK_EXCVADDR

    /* _xt_context_save seems to save the current a0, but we need the interuptees a0. Fix this. */
    rsr     a0, EXCSAVE_1                   /* save interruptee's a0 */

    /* Set up PS for C, disable all interrupts except NMI and debug, and clear EXCM. */
    movi    a0, PS_INTLEVEL(5) | PS_UM | PS_WOE
    wsr     a0, PS

    //Call gdbstub handler

    rsr     a0,(EPC + XCHAL_DEBUGLEVEL)
    s32i    a0, sp, XT_STK_PC
    mov     a6, sp
    rsr     a9, EPS_6
    s32i    a9, sp, XT_STK_PS   // store PS to the ps place
    movi    a11, gdbstub_handle_debug_int
    callx4  a11                          /* Call interrupt handler */
    l32i    a0, sp, XT_STK_PC
    wsr     a0,(EPC + XCHAL_DEBUGLEVEL)
    call0   _xt_context_restore         /* Restore full context*/
    l32i    a12, sp, XT_STK_A12
    l32i    a13, sp, XT_STK_A13
    l32i    a0, sp, XT_STK_EXIT         /* Restore return point*/
    addi    sp, sp, XT_STK_FRMSZ        /* Restore SP*/
    rfi     6       // Return from high-level interrupt
