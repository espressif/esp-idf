#include "freertos/xtensa_rtos.h"
#include "esp_private/panic_reason.h"
#include "soc/soc.h"

#include "sdkconfig.h"



/*
--------------------------------------------------------------------------------
  Panic handler.
  Should be reached by call0 (preferable) or jump only. If call0, a0 says where
  from. If on simulator, display panic message and abort, else loop indefinitely.
--------------------------------------------------------------------------------
*/

    .section .iram1,"ax"
    .global panicHandler

    .global     _xt_panic
    .type       _xt_panic,@function
    .align      4
    .literal_position
    .align      4

_xt_panic:
    /* Allocate exception frame and save minimal context. */
    mov     a0, sp
    addi    sp, sp, -XT_STK_FRMSZ
    s32i    a0, sp, XT_STK_A1
    #if XCHAL_HAVE_WINDOWED
    s32e    a0, sp, -12                     /* for debug backtrace */
    #endif
    rsr     a0, PS                          /* save interruptee's PS */
    s32i    a0, sp, XT_STK_PS
    rsr     a0, EPC_1                       /* save interruptee's PC */
    s32i    a0, sp, XT_STK_PC
    rsr     a0, EXCSAVE_1                   /* save interruptee's a0 */
    s32i    a0, sp, XT_STK_A0
    #if XCHAL_HAVE_WINDOWED
    s32e    a0, sp, -16                     /* for debug backtrace */
    #endif
    s32i    a12, sp, XT_STK_A12             /* _xt_context_save requires A12- */
    s32i    a13, sp, XT_STK_A13             /* A13 to have already been saved */
    call0   _xt_context_save

    /* Save exc cause and vaddr into exception frame */
    rsr     a0, EXCCAUSE
    s32i    a0, sp, XT_STK_EXCCAUSE
    rsr     a0, EXCVADDR
    s32i    a0, sp, XT_STK_EXCVADDR

    /* Set up PS for C, disable all interrupts except NMI and debug, and clear EXCM. */
    movi    a0, PS_INTLEVEL(XCHAL_DEBUGLEVEL - 2) | PS_UM | PS_WOE
    wsr     a0, PS

    //Call panic handler
    mov     a6,sp
    call4 panicHandler

    ret
