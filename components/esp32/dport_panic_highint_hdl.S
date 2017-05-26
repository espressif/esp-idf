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
#include <xtensa/simcall.h>
#include "freertos/xtensa_context.h"
#include "esp_panic.h"
#include "sdkconfig.h"
#include "soc/soc.h"
#include "soc/dport_reg.h"

/*

Interrupt , a high-priority interrupt, is used for several things:
- Dport access mediation
- Cache error panic handler
- Interrupt watchdog panic handler

*/

#define L4_INTR_STACK_SIZE  8
#define L4_INTR_A2_OFFSET   0
#define L4_INTR_A3_OFFSET   4
    .data
_l4_intr_stack:
    .space      L4_INTR_STACK_SIZE

    .section .iram1,"ax"
    .global     xt_highint4
    .type       xt_highint4,@function
    .align      4
xt_highint4:

    /* See if we're here for the dport access interrupt */
    rsr     a0, INTERRUPT
    extui   a0, a0, ETS_DPORT_INUM, 1
    bnez    a0, .handle_dport_access_int

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
    extui   a0, a0, ETS_CACHEERR_INUM, 1 /* get cacheerr int bit */
    beqz    a0, 1f
    /* Kill this interrupt; we cannot reset it. */
    rsr     a0, INTENABLE
    movi    a4, ~(1<<ETS_CACHEERR_INUM)
    and     a0, a4, a0
    wsr     a0, INTENABLE
    movi    a0, PANIC_RSN_CACHEERR
    j 9f
1:
#if CONFIG_INT_WDT_CHECK_CPU1
    /* Check if the cause is the app cpu failing to tick.*/
    movi    a0, int_wdt_app_cpu_ticked
    l32i    a0, a0, 0
    bnez    a0, 2f
    /* It is. Modify cause. */
    movi    a0,PANIC_RSN_INTWDT_CPU1
    j 9f
2:
#endif
    /* Set EXCCAUSE to reflect cause of the wdt int trigger */
    movi    a0,PANIC_RSN_INTWDT_CPU0
9:
    /* Found the reason, now save it. */
    s32i    a0, sp, XT_STK_EXCCAUSE

    /* _xt_context_save seems to save the current a0, but we need the interuptees a0. Fix this. */
    rsr     a0, EXCSAVE_4                   /* save interruptee's a0 */

    s32i    a0, sp, XT_STK_A0

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





    .align      4
.handle_dport_access_int:
    /* This section is for dport access register protection */
    /* Allocate exception frame and save minimal context. */
    /* Because the interrupt cause code has protection that only
       allows one cpu to enter in the dport section of the L4 
       interrupt at one time, there's no need to have two 
       _l4_intr_stack for each cpu */

    /* This int is edge-triggered and needs clearing. */
    movi    a0, (1<<ETS_DPORT_INUM)
    wsr     a0, INTCLEAR

    /* Save A2, A3 so we can use those registers */
    movi    a0, _l4_intr_stack
    s32i    a2, a0, L4_INTR_A2_OFFSET
    s32i    a3, a0, L4_INTR_A3_OFFSET

    /* handle dport interrupt */
    /* get CORE_ID */
    getcoreid   a0
    beqz    a0, 2f

    /* current cpu is 1 */
    movi    a0, DPORT_CPU_INTR_FROM_CPU_3_REG
    movi    a2, 0
    s32i    a2, a0, 0   /* clear intr */
    movi    a0, 0       /* other cpu id */
    j       3f
2:
    /* current cpu is 0 */
    movi    a0, DPORT_CPU_INTR_FROM_CPU_2_REG
    movi    a2, 0
    s32i    a2, a0, 0   /* clear intr */
    movi    a0, 1       /* other cpu id */
3:
    /* set and wait flag */
    movi    a2, dport_access_start
    addx4   a2, a0, a2
    movi    a3, 1
    s32i    a3, a2, 0
    memw
    movi    a2, dport_access_end
    addx4   a2, a0, a2
.check_dport_access_end:
    l32i    a3, a2, 0
    beqz    a3, .check_dport_access_end

    /* Done. Restore registers and return. */
    movi    a0, _l4_intr_stack
    l32i    a2, a0, L4_INTR_A2_OFFSET
    l32i    a3, a0, L4_INTR_A3_OFFSET
    rsync                                   /* ensure register restored */

    rsr     a0, EXCSAVE_4                   /* restore a0 */
    rfi     4


/* The linker has no reason to link in this file; all symbols it exports are already defined
   (weakly!) in the default int handler. Define a symbol here so we can use it to have the 
   linker inspect this anyway. */

    .global ld_include_panic_highint_hdl
ld_include_panic_highint_hdl:




