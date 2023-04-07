/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

    #include "freertos/xtensa_rtos.h"

    .section .iram1, "ax"

    .global gdbstub_handle_uart_int
    .global esp_gdbstub_int
    .type   esp_gdbstub_int, @function
    .align  4

esp_gdbstub_int:

    /* Allocate exception frame and save minimal context. */
    mov     a0, sp
    addi    sp, sp, -XT_STK_FRMSZ
    s32i    a0, sp, XT_STK_EXIT
    s32i    a0, sp, XT_STK_A0

    #if XCHAL_HAVE_WINDOWED
    s32e    a0, sp, -12                     /* for debug backtrace */
    #endif
    rsr     a0, PS                          /* save interruptee's PS */
    s32i    a0, sp, XT_STK_PS
    rsr     a0, EPC_1                       /* save interruptee's PC */
    s32i    a0, sp, XT_STK_PC
    #if XCHAL_HAVE_WINDOWED
    s32e    a0, sp, -16                     /* for debug backtrace */
    #endif
    s32i    a12, sp, XT_STK_A12             /* _xt_context_save requires A12- */
    s32i    a13, sp, XT_STK_A13             /* A13 to have already been saved */

    /* Save exc cause and vaddr into exception frame */
    rsr     a0, EXCCAUSE
    s32i    a0, sp, XT_STK_EXCCAUSE
    rsr     a0, EXCVADDR
    s32i    a0, sp, XT_STK_EXCVADDR


    /* _xt_context_save seems to save the current a0, but we need the interuptees a0. Fix this. */
    rsr     a0, EXCSAVE_1                   /* save interruptee's a0 */

    s32i    a0, sp, XT_STK_A0

    /* Save context pointer as input parameter */
    rsr  a6, excsave1

    rsr     a3, EPS
    s32i    a3, sp, XT_STK_PS   // store PS to the ps place
    movi    a3, gdbstub_handle_uart_int
    callx0  a3

    l32i    a0, sp, XT_STK_EXIT
    addi    sp, sp, XT_STK_FRMSZ
    ret
