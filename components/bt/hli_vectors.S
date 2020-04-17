// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
// All rights reserved.

#include <xtensa/coreasm.h>
#include <xtensa/corebits.h>
#include <xtensa/config/system.h>
#include "freertos/xtensa_context.h"
#include "sdkconfig.h"
#include "soc/soc.h"

#define L5_INTR_STACK_SIZE  4096
    .data
_l5_intr_stack:
    .space      L5_INTR_STACK_SIZE

    .section .iram1,"ax"
    .global     xt_highint5
    .type       xt_highint5,@function
    .align      4

xt_highint5:
    /* Allocate exception frame and save minimal context. */
    mov     a0, sp
    addi    sp, sp, -XT_STK_FRMSZ
    s32i    a0, sp, XT_STK_A1
    #if XCHAL_HAVE_WINDOWED
    s32e    a0, sp, -12                     /* for debug backtrace */
    #endif
    rsr     a0, EPS_5                       /* save interruptee's PS */
    s32i    a0, sp, XT_STK_PS
    rsr     a0, EPC_5                       /* save interruptee's PC */
    s32i    a0, sp, XT_STK_PC
    #if XCHAL_HAVE_WINDOWED
    s32e    a0, sp, -16                     /* for debug backtrace */
    #endif
    s32i    a12, sp, XT_STK_A12             /* _xt_context_save requires A12- */
    s32i    a13, sp, XT_STK_A13             /* A13 to have already been saved */
    call0   _xt_context_save

_highint5_stack_switch:
    mov     a7, sp
    movi    sp, _l5_intr_stack + L5_INTR_STACK_SIZE - 16
    s32e    a7, sp, -12
    movi    a0, _Level5Vector+6            /* For GDB: pretend we came from the interrupt vector */
    s32e    a0, sp, -16
    movi    a0, _highint5_stack_switch     /* For GDB: cosmetics, for the frame where stack switch happened */

    /* Set up PS for C, disable all interrupts except NMI and debug, and clear EXCM. */
    movi    a6, PS_INTLEVEL(5) | PS_UM | PS_WOE
    wsr     a6, PS
    rsync

    /* Call C handler */
    mov     a6, sp
    call4   hli_c_handler

    l32e    sp, sp, -12                     /* switch back to the original stack */

    call0   _xt_context_restore
    l32i    a0, sp, XT_STK_PS               /* retrieve interruptee's PS */
    wsr     a0, EPS_5
    l32i    a0, sp, XT_STK_PC               /* retrieve interruptee's PC */
    wsr     a0, EPC_5
    l32i    a0, sp, XT_STK_A0               /* retrieve interruptee's A0 */
    l32i    sp, sp, XT_STK_A1               /* remove exception frame */
    rsync                                   /* ensure PS and EPC written */

    rsr     a0, EXCSAVE_5                   /* restore a0 */
    rfi     5

/* The linker has no reason to link in this file; all symbols it exports are already defined
   (weakly!) in the default int handler. Define a symbol here so we can use it to have the
   linker inspect this anyway. */

    .global ld_include_hli_vectors_bt
ld_include_hli_vectors_bt:
