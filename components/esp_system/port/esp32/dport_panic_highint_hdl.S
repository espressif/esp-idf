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
#include "freertos/xtensa_rtos.h"
#include "esp_private/panic_reason.h"
#include "sdkconfig.h"
#include "soc/soc.h"
#include "soc/dport_reg.h"
#include "soc/timer_group_reg.h"

/*

Interrupt , a high-priority interrupt, is used for several things:
- Dport access mediation
- Cache error panic handler
- Interrupt watchdog panic handler

*/

#define L4_INTR_STACK_SIZE  12
#define L4_INTR_A2_OFFSET   0
#define L4_INTR_A3_OFFSET   4
#define L4_INTR_A4_OFFSET   8
    .data
_l4_intr_stack:
    .space      L4_INTR_STACK_SIZE*portNUM_PROCESSORS /* This allocates stacks for each individual CPU. */

#if CONFIG_ESP32_ECO3_CACHE_LOCK_FIX && CONFIG_ESP_INT_WDT
    .global _l4_intr_livelock_counter
    .global _l4_intr_livelock_max
    .align  16
_l4_intr_livelock_counter:
    .word   0
_l4_intr_livelock_max:
    .word   0
_l4_intr_livelock_sync:
    .word   0, 0
_l4_intr_livelock_app:
    .word   0
_l4_intr_livelock_pro:
    .word   0
#endif

    .section .iram1,"ax"
    .global     xt_highint4
    .type       xt_highint4,@function
    .align      4
xt_highint4:

#ifndef CONFIG_FREERTOS_UNICORE
    /* See if we're here for the dport access interrupt */
    rsr     a0, INTERRUPT
    extui   a0, a0, ETS_DPORT_INUM, 1
    bnez    a0, .handle_dport_access_int
#endif // CONFIG_FREERTOS_UNICORE

#if CONFIG_ESP32_ECO3_CACHE_LOCK_FIX && CONFIG_ESP_INT_WDT
    /* See if we're here for the tg1 watchdog interrupt */
    rsr     a0, INTERRUPT
    extui   a0, a0, ETS_T1_WDT_INUM, 1
    beqz    a0, 1f

    wsr     a5, depc                        /* use DEPC as temp storage */
    movi    a0, _l4_intr_livelock_counter
    l32i    a0, a0, 0
    movi    a5, _l4_intr_livelock_max
    l32i    a5, a5, 0
    bltu    a0, a5, .handle_livelock_int    /* _l4_intr_livelock_counter < _l4_intr_livelock_max */

    rsr     a5, depc                        /* restore a5 */
#endif

    /* Allocate exception frame and save minimal context. */
1:  mov     a0, sp
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
#if CONFIG_ESP_INT_WDT_CHECK_CPU1
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


#if CONFIG_ESP32_ECO3_CACHE_LOCK_FIX && CONFIG_ESP_INT_WDT

/*
--------------------------------------------------------------------------------
  Macro intr_matrix_map - Attach an CPU interrupt to a hardware source.

  Input  : "addr" - Interrupt map configuration base address
  Input  : "src"  - Interrupt source.
  Input  : "inum" - Interrupt number.
--------------------------------------------------------------------------------
*/
    .macro intr_matrix_map  addr src inum
    movi    a2, \src
    slli    a2, a2, 2
    movi    a3, \addr
    add     a3, a3, a2
    movi    a2, \inum
    s32i    a2, a3, 0
    memw
    .endm

/*
--------------------------------------------------------------------------------
  Macro wdt_clr_intr_status - Clear the WDT interrupt status.
  Macro wdt_feed            - Feed the WDT.

  Input  : "dev" - Beginning address of the peripheral registers
--------------------------------------------------------------------------------
*/

#define TIMG1_REG_OFFSET(reg)               ((reg) - REG_TIMG_BASE(1))
#define TIMG1_WDTWPROTECT_OFFSET            TIMG1_REG_OFFSET(TIMG_WDTWPROTECT_REG(1))
#define TIMG1_INT_CLR_OFFSET                TIMG1_REG_OFFSET(TIMG_INT_CLR_TIMERS_REG(1))
#define TIMG1_WDT_STG0_HOLD_OFFSET          TIMG1_REG_OFFSET(TIMG_WDTCONFIG2_REG(1))
#define TIMG1_WDT_STG1_HOLD_OFFSET          TIMG1_REG_OFFSET(TIMG_WDTCONFIG3_REG(1))
#define TIMG1_WDT_FEED_OFFSET               TIMG1_REG_OFFSET(TIMG_WDTFEED_REG(1))

    .macro wdt_clr_intr_status  dev
    movi    a2, \dev
    movi    a3, TIMG_WDT_WKEY_VALUE
    s32i    a3, a2, TIMG1_WDTWPROTECT_OFFSET    /* disable write protect */
    memw
    l32i    a4, a2, TIMG1_INT_CLR_OFFSET
    memw
    movi    a3, 4
    or      a3, a4, a3
    s32i    a3, a2, TIMG1_INT_CLR_OFFSET        /* clear 1st stage timeout interrupt */
    memw
    movi    a3, 0
    s32i    a3, a2, TIMG1_WDTWPROTECT_OFFSET    /* enable write protect */
    memw
    .endm

    .macro wdt_feed dev
    movi    a2, \dev
    movi    a3, TIMG_WDT_WKEY_VALUE
    s32i    a3, a2, TIMG1_WDTWPROTECT_OFFSET    /* disable write protect */
    memw
    movi    a4, _l4_intr_livelock_max
    l32i    a4, a4, 0
    memw
    addi    a4, a4, 1
    movi    a3, (CONFIG_ESP_INT_WDT_TIMEOUT_MS<<1)
    quou    a3, a3, a4
    s32i    a3, a2, TIMG1_WDT_STG0_HOLD_OFFSET  /* set timeout before interrupt */
    memw
    movi    a3, (CONFIG_ESP_INT_WDT_TIMEOUT_MS<<2)
    s32i    a3, a2, TIMG1_WDT_STG1_HOLD_OFFSET  /* set timeout before system reset */
    memw
    movi    a3, 1
    s32i    a3, a2, TIMG1_WDT_FEED_OFFSET       /* feed wdt */
    memw
    movi    a3, 0
    s32i    a3, a2, TIMG1_WDTWPROTECT_OFFSET    /* enable write protect */
    memw
    .endm

    .align      4
.handle_livelock_int:

    getcoreid   a5

    /* Save A2, A3, A4 so we can use those registers */
    movi    a0, L4_INTR_STACK_SIZE
    mull    a5, a5, a0
    movi    a0, _l4_intr_stack
    add     a0, a0, a5
    s32i    a2, a0, L4_INTR_A2_OFFSET
    s32i    a3, a0, L4_INTR_A3_OFFSET
    s32i    a4, a0, L4_INTR_A4_OFFSET

    /* Here, we can use a0, a2, a3, a4, a5 registers */
    getcoreid   a5

    rsil    a0, CONFIG_ESP32_DPORT_DIS_INTERRUPT_LVL /* disable nested interrupt */

    beqz    a5, 1f
    movi    a2, _l4_intr_livelock_app
    l32i    a3, a2, 0
    addi    a3, a3, 1
    s32i    a3, a2, 0

    /* Dual core synchronization, ensuring that both cores enter interrupts */
1:  movi    a4, 0x1
    movi    a2, _l4_intr_livelock_sync
    addx4   a3, a5, a2
    s32i    a4, a3, 0

1:  movi    a2, _l4_intr_livelock_sync
    movi    a3, 1
    addx4   a3, a3, a2
    l32i    a2, a2, 0
    l32i    a3, a3, 0
    and     a2, a2, a3
    beqz    a2, 1b

    beqz    a5, 1f                          /* Pro cpu (Core 0) jump bypass */

    movi    a2, _l4_intr_livelock_app
    l32i    a2, a2, 0
    bnei    a2, 2, 1f
    movi    a2, _l4_intr_livelock_counter   /* _l4_intr_livelock_counter++ */
    l32i    a3, a2, 0
    addi    a3, a3, 1
    s32i    a3, a2, 0

    /*
    The delay time can be calculated by the following formula:
      T = ceil(0.25 + max(t1, t2)) us

      t1 = 80 / f1, t2 = (1 + 14/N) * 20 / f2

      f1: PSRAM access frequency, unit: MHz.
      f2: Flash access frequency, unit: MHz.

      When flash is slow/fast read, N = 1.
      When flash is DOUT/DIO read, N = 2.
      When flash is QOUT/QIO read, N = 4.
    */
1:  rsr.ccount  a2
#if defined(CONFIG_ESPTOOLPY_FLASHMODE_QIO) || defined(CONFIG_ESPTOOLPY_FLASHMODE_QOUT)
# if defined(CONFIG_ESPTOOLPY_FLASHFREQ_80M) && defined(CONFIG_SPIRAM_SPEED_80M)
    movi    a3, 480
# elif defined(CONFIG_ESPTOOLPY_FLASHFREQ_80M) && defined(CONFIG_SPIRAM_SPEED_40M)
    movi    a3, 720
# elif defined(CONFIG_ESPTOOLPY_FLASHFREQ_40M) && defined(CONFIG_SPIRAM_SPEED_40M)
    movi    a3, 720
# elif defined(CONFIG_ESPTOOLPY_FLASHFREQ_26M) && defined(CONFIG_SPIRAM_SPEED_40M)
    movi    a3, 960
# else
    movi    a3, 1200
# endif
#elif defined(CONFIG_ESPTOOLPY_FLASHMODE_DIO) || defined(CONFIG_ESPTOOLPY_FLASHMODE_DOUT)
# if defined(CONFIG_ESPTOOLPY_FLASHFREQ_80M) && defined(CONFIG_SPIRAM_SPEED_80M)
    movi    a3, 720
# elif defined(CONFIG_ESPTOOLPY_FLASHFREQ_80M) && defined(CONFIG_SPIRAM_SPEED_40M)
    movi    a3, 720
# elif defined(CONFIG_ESPTOOLPY_FLASHFREQ_40M) && defined(CONFIG_SPIRAM_SPEED_40M)
    movi    a3, 1200
# elif defined(CONFIG_ESPTOOLPY_FLASHFREQ_26M) && defined(CONFIG_SPIRAM_SPEED_40M)
    movi    a3, 1680
# else
    movi    a3, 2160
# endif
#endif
2:  rsr.ccount  a4                          /* delay_us(N) */
    sub     a4, a4, a2
    bltu    a4, a3, 2b

    beqz    a5, 2f
    movi    a2, _l4_intr_livelock_app
    l32i    a2, a2, 0
    beqi    a2, 2, 8f
    j       3f

2:  movi    a2, _l4_intr_livelock_pro
    l32i    a4, a2, 0
    addi    a4, a4, 1
    s32i    a4, a2, 0

    movi    a2, _l4_intr_livelock_sync
    movi    a3, 1
    addx4   a3, a3, a2
    l32i    a2, a2, 0
    l32i    a3, a3, 0
    and     a2, a2, a3
    beqz    a2, 5f
    j       1b
5:  bgei    a4, 2, 4f
    j       1b

    /*
    Pro cpu (Core 0) jump bypass, continue waiting, App cpu (Core 1)
    can execute to here, unmap itself tg1 1st stage timeout interrupt
    then restore registers and exit highint4.
    */
3:  intr_matrix_map DPORT_APP_MAC_INTR_MAP_REG, ETS_TG1_WDT_LEVEL_INTR_SOURCE, 16
    j       9f

    /*
    Here, App cpu (Core 1) has exited isr, Pro cpu (Core 0) help the
    App cpu map tg1 1st stage timeout interrupt clear tg1 interrupt.
    */
4:  intr_matrix_map DPORT_APP_MAC_INTR_MAP_REG, ETS_TG1_WDT_LEVEL_INTR_SOURCE, ETS_T1_WDT_INUM

1:  movi    a2, _l4_intr_livelock_sync
    movi    a4, 1
    addx4   a3, a4, a2
    l32i    a2, a2, 0
    l32i    a3, a3, 0
    and     a2, a2, a3
    beqz    a2, 1b                          /* Wait for App cpu to enter highint4 again */

    wdt_clr_intr_status TIMERG1
    j       9f

    /* Feed watchdog */
8:  wdt_feed    TIMERG1

9:  wsr     a0, PS                          /* restore iterrupt level */

    movi    a0, 0
    beqz    a5, 1f
    movi    a2, _l4_intr_livelock_app
    l32i    a3, a2, 0
    bnei    a3, 2, 1f
    s32i    a0, a2, 0

1:  bnez    a5, 2f
    movi    a2, _l4_intr_livelock_pro
    s32i    a0, a2, 0
2:  movi    a2, _l4_intr_livelock_sync
    addx4   a2, a5, a2
    s32i    a0, a2, 0

    /* Done. Restore registers and return. */
    movi    a0, L4_INTR_STACK_SIZE
    mull    a5, a5, a0
    movi    a0, _l4_intr_stack
    add     a0, a0, a5
    l32i    a2, a0, L4_INTR_A2_OFFSET
    l32i    a3, a0, L4_INTR_A3_OFFSET
    l32i    a4, a0, L4_INTR_A4_OFFSET
    rsync                                   /* ensure register restored */

    rsr     a5, depc

    rsr     a0, EXCSAVE_4                   /* restore a0 */
    rfi     4

#endif


#ifndef CONFIG_FREERTOS_UNICORE

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

    /* Save A2, A3, A4 so we can use those registers */
    movi    a0, _l4_intr_stack
    s32i    a2, a0, L4_INTR_A2_OFFSET
    s32i    a3, a0, L4_INTR_A3_OFFSET
    s32i    a4, a0, L4_INTR_A4_OFFSET

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
    rsil    a4, CONFIG_ESP32_DPORT_DIS_INTERRUPT_LVL /* disable nested iterrupt */
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

    wsr     a4, PS                                   /* restore iterrupt level */
    /* Done. Restore registers and return. */
    movi    a0, _l4_intr_stack
    l32i    a2, a0, L4_INTR_A2_OFFSET
    l32i    a3, a0, L4_INTR_A3_OFFSET
    l32i    a4, a0, L4_INTR_A4_OFFSET
    rsync                                   /* ensure register restored */

    rsr     a0, EXCSAVE_4                   /* restore a0 */
    rfi     4

#endif // CONFIG_FREERTOS_UNICORE

/* The linker has no reason to link in this file; all symbols it exports are already defined
   (weakly!) in the default int handler. Define a symbol here so we can use it to have the
   linker inspect this anyway. */

    .global ld_include_panic_highint_hdl
ld_include_panic_highint_hdl:




