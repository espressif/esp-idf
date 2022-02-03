// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include <xtensa/coreasm.h>
#include <xtensa/corebits.h>
#include <xtensa/config/system.h>
#include "freertos/xtensa_context.h"
#include "esp_private/panic_reason.h"
#include "sdkconfig.h"
#include "soc/soc.h"
#include "soc/dport_reg.h"

/*

Interrupt , a high-priority interrupt, is used for several things:
- Cache error panic handler
- Interrupt watchdog panic handler

*/

    .section .iram1,"ax"
    .global     xt_highint4
    .type       xt_highint4,@function
    .align      4
xt_highint4:

    /* Allocate exception frame and save minimal context. */
    mov     a0, sp
    addi    sp, sp, -XT_STK_FRMSZ
    s32i    a0, sp, XT_STK_A1
    #if XCHAL_HAVE_WINDOWED
    s32e    a0, sp, -12                     /* for debug backtrace */
    #endif
    rsr     a0, PS                          /* save interruptee's PS */
    s32i    a0, sp, XT_STK_PS
    rsr     a0, EPC_4                       /* save interruptee's PC */
    s32i    a0, sp, XT_STK_PC
    rsr     a0, EXCSAVE_4                   /* save interruptee's a0 */
    s32i    a0, sp, XT_STK_A0
    #if XCHAL_HAVE_WINDOWED
    s32e    a0, sp, -16                     /* for debug backtrace */
    #endif
    s32i    a12, sp, XT_STK_A12             /* _xt_context_save requires A12- */
    s32i    a13, sp, XT_STK_A13             /* A13 to have already been saved */
    call0   _xt_context_save

    /* Save vaddr into exception frame */
    rsr     a0, EXCVADDR
    s32i    a0, sp, XT_STK_EXCVADDR

    /* Figure out reason, save into EXCCAUSE reg */

    rsr     a0, INTERRUPT
    extui   a0, a0, ETS_MEMACCESS_ERR_INUM, 1 /* get cacheerr int bit */
    beqz    a0, 1f
    /* Kill this interrupt; we cannot reset it. */
    rsr     a0, INTENABLE
    movi    a4, ~(1<<ETS_MEMACCESS_ERR_INUM)
    and     a0, a4, a0
    wsr     a0, INTENABLE
    movi    a0, PANIC_RSN_CACHEERR
    j 9f
1:
    /* Set EXCCAUSE to reflect cause of the wdt int trigger */
    movi    a0,PANIC_RSN_INTWDT_CPU0
9:
    /* Found the reason, now save it. */
    s32i    a0, sp, XT_STK_EXCCAUSE

    /* Set up PS for C, disable all interrupts except NMI and debug, and clear EXCM. */
    movi    a0, PS_INTLEVEL(5) | PS_UM | PS_WOE
    wsr     a0, PS

    //Call panic handler
    mov     a6,sp
    call4   panicHandler

    call0   _xt_context_restore
    l32i    a0, sp, XT_STK_PS               /* retrieve interruptee's PS */
    wsr     a0, PS
    l32i    a0, sp, XT_STK_PC               /* retrieve interruptee's PC */
    wsr     a0, EPC_4
    l32i    a0, sp, XT_STK_A0               /* retrieve interruptee's A0 */
    l32i    sp, sp, XT_STK_A1               /* remove exception frame */
    rsync                                   /* ensure PS and EPC written */

    rsr     a0, EXCSAVE_4                   /* restore a0 */
    rfi     4

/* The linker has no reason to link in this file; all symbols it exports are already defined
   (weakly!) in the default int handler. Define a symbol here so we can use it to have the
   linker inspect this anyway. */

    .global ld_include_highint_hdl
ld_include_highint_hdl:
