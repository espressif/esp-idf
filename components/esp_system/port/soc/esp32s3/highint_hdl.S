/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <xtensa/coreasm.h>
#include <xtensa/corebits.h>
#include <xtensa/config/system.h>
#include "xtensa_context.h"
#include "esp_private/panic_reason.h"
#include "sdkconfig.h"
#include "soc/soc.h"
#include "soc/dport_reg.h"
#include "soc/soc_caps.h"

/*

Interrupt , a high-priority interrupt, is used for several things:
- IPC_ISR handler
- Cache error panic handler
- Interrupt watchdog panic handler

*/

#define L4_INTR_STACK_SIZE  12
#define L4_INTR_A2_OFFSET   0
#define L4_INTR_A3_OFFSET   4
#define L4_INTR_A4_OFFSET   8
.data
_l4_intr_stack:
    .space      L4_INTR_STACK_SIZE*SOC_CPU_CORES_NUM /* This allocates stacks for each individual CPU. */
    .section .iram1,"ax"
    .global     xt_highint4
    .type       xt_highint4,@function
    .align      4
xt_highint4:

#ifndef CONFIG_FREERTOS_UNICORE
    /* See if we're here for the IPC_ISR interrupt */
    rsr     a0, INTERRUPT
    extui   a0, a0, ETS_IPC_ISR_INUM, 1
    bnez    a0, jump_to_esp_ipc_isr_handler
#endif // not CONFIG_FREERTOS_UNICORE

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
#if CONFIG_ESP_INT_WDT_CHECK_CPU1
    /* Check if the cause is the app cpu failing to tick.*/
    movi    a0, int_wdt_cpu1_ticked
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

#ifdef CONFIG_ESP_IPC_ISR_ENABLE
jump_to_esp_ipc_isr_handler:
    /* Address of `esp_ipc_isr_handler_address` will always be in `movi` range
     * as it is defined right above. */
    movi    a0, esp_ipc_isr_handler
    jx      a0
#endif

/* The linker has no reason to link in this file; all symbols it exports are already defined
   (weakly!) in the default int handler. Define a symbol here so we can use it to have the
   linker inspect this anyway. */

    .global ld_include_highint_hdl
ld_include_highint_hdl:
