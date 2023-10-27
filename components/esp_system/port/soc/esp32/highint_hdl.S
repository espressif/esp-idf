/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <xtensa/coreasm.h>
#include <xtensa/corebits.h>
#include <xtensa/config/system.h>
#include "xtensa_context.h"
#include "freertos/xtensa_rtos.h"
#include "esp_private/panic_reason.h"
#include "sdkconfig.h"
#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "soc/dport_reg.h"
#include "soc/timer_group_reg.h"

/*

Interrupt , a high-priority interrupt, is used for several things:
- IPC_ISR handler
- Cache error panic handler
- Interrupt watchdog panic handler

*/

#if CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_5

#define LX_INTR_STACK_SIZE  12
#define LX_INTR_A2_OFFSET   0
#define LX_INTR_A3_OFFSET   4
#define LX_INTR_A4_OFFSET   8
#define EPC_X               EPC_5
#define EXCSAVE_X           EXCSAVE_5
#define RFI_X               5
#define xt_highintx         xt_highint5

#elif CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_4

#define LX_INTR_STACK_SIZE  12
#define LX_INTR_A2_OFFSET   0
#define LX_INTR_A3_OFFSET   4
#define LX_INTR_A4_OFFSET   8
#define EPC_X               EPC_4
#define EXCSAVE_X           EXCSAVE_4
#define RFI_X               4
#define xt_highintx         xt_highint4

#endif /* CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_5 */

/*
--------------------------------------------------------------------------------
  Macro wdt_clr_intr_status - Clear the WDT interrupt status.
  Macro wdt_feed            - Feed the WDT.

  Input  : "dev" - Beginning address of the peripheral registers

  Macro get_int_status_tg1wdt - Get the ETS_TG1_WDT_LEVEL_INTR_SOURCE bit in interrupt status

  output  : "reg" - Store the result into the reg
--------------------------------------------------------------------------------
*/

#define TIMG1_REG_OFFSET(reg)               ((reg) - REG_TIMG_BASE(1))
#define TIMG1_WDTWPROTECT_OFFSET            TIMG1_REG_OFFSET(TIMG_WDTWPROTECT_REG(1))
#define TIMG1_INT_CLR_OFFSET                TIMG1_REG_OFFSET(TIMG_INT_CLR_TIMERS_REG(1))
#define TIMG1_WDT_STG0_HOLD_OFFSET          TIMG1_REG_OFFSET(TIMG_WDTCONFIG2_REG(1))
#define TIMG1_WDT_STG1_HOLD_OFFSET          TIMG1_REG_OFFSET(TIMG_WDTCONFIG3_REG(1))
#define TIMG1_WDT_FEED_OFFSET               TIMG1_REG_OFFSET(TIMG_WDTFEED_REG(1))
#define UART0_DATA_REG                      (0x3FF40078)
#define TIMG_WDT_WKEY_VALUE                 0x50D83AA1
#define ETS_TG1_WDT_LEVEL_INTR_SOURCE       20

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
    movi    a4, _lx_intr_livelock_max
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

    .macro get_int_status_tg1wdt reg
    rsr     \reg, INTERRUPT
    extui   \reg, \reg, ETS_T1_WDT_CACHEERR_INUM, 1
    beqz    \reg, 99f        /* not ETS_T1_WDT_INUM or ETS_CACHEERR_INUM */

    getcoreid   \reg
    bnez    \reg, 98f
    /* core 0 */
    movi    \reg, UART0_DATA_REG
    l32i    \reg, \reg, 0     /* Workaround for DPORT read error, for silicon revision 0~2 (ECO V0 ~ ECO V2). */
    movi    \reg, DPORT_PRO_INTR_STATUS_0_REG
    l32i    \reg, \reg, 0
    extui   \reg, \reg, ETS_TG1_WDT_LEVEL_INTR_SOURCE, 1
    j       99f

98: /* core 1 */
    movi    \reg, UART0_DATA_REG
    l32i    \reg, \reg, 0     /* Workaround for DPORT read error, for silicon revision 0~2 (ECO V0 ~ ECO V2). */
    movi    \reg, DPORT_APP_INTR_STATUS_0_REG
    l32i    \reg, \reg, 0
    extui   \reg, \reg, ETS_TG1_WDT_LEVEL_INTR_SOURCE, 1
99:
    .endm

    .data
_lx_intr_stack:
    .space      LX_INTR_STACK_SIZE*portNUM_PROCESSORS /* This allocates stacks for each individual CPU. */

#if CONFIG_ESP32_ECO3_CACHE_LOCK_FIX && CONFIG_ESP_INT_WDT
    .global _lx_intr_livelock_counter
    .global _lx_intr_livelock_max
    .align  16
_lx_intr_livelock_counter:
    .word   0
_lx_intr_livelock_max:
    .word   0
_lx_intr_livelock_sync:
    .word   0, 0
_lx_intr_livelock_app:
    .word   0
_lx_intr_livelock_pro:
    .word   0
#endif

    .section .iram1,"ax"
    .global     xt_highintx
    .type       xt_highintx,@function
    .align      4
xt_highintx:

#ifndef CONFIG_FREERTOS_UNICORE
    /* See if we're here for the IPC_ISR interrupt */
    rsr     a0, INTERRUPT
    extui   a0, a0, ETS_IPC_ISR_INUM, 1
    beqz    a0, 1f
    /* Jump to `esp_ipc_isr_handler` which is non-returning. We need to use `jx`
     * because on Xtensa, `j` instruction can only refer to a label which
     * is in the range [-131068;+131075]. If the destination is out of scope,
     * linking will fail. So, to make sure we will always be able to jump to
     * that subroutine, retrieve its address and store it in a register. */
    movi    a0, esp_ipc_isr_handler
    jx      a0
1:
#endif /* not CONFIG_FREERTOS_UNICORE */

#if CONFIG_ESP32_ECO3_CACHE_LOCK_FIX && CONFIG_ESP_INT_WDT

#if CONFIG_BTDM_CTRL_HLI
    /* Timer 2 interrupt */
    rsr     a0, INTENABLE
    extui   a0, a0, 16, 1
    beqz    a0, 1f
    rsr     a0, INTERRUPT
    extui   a0, a0, 16, 1
    bnez    a0, .handle_multicore_debug_int
1:
#endif /* CONFIG_BTDM_CTRL_HLI */

    /* ETS_T1_WDT_INUM */
#if CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_5
    get_int_status_tg1wdt a0
#elif CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_4
    /* See if we're here for the tg1 watchdog interrupt */
    rsr     a0, INTERRUPT
    extui   a0, a0, ETS_T1_WDT_INUM, 1
#endif /* CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_5 */
    beqz    a0, 1f

    wsr     a5, depc                        /* use DEPC as temp storage */
    movi    a0, _lx_intr_livelock_counter
    l32i    a0, a0, 0
    movi    a5, _lx_intr_livelock_max
    l32i    a5, a5, 0
    bltu    a0, a5, .handle_livelock_int    /* _lx_intr_livelock_counter < _lx_intr_livelock_max */

    rsr     a5, depc                        /* restore a5 */
#endif

1:  /* ETS_CACHEERR_INUM or ETS_T1_WDT_INUM */
    /* Allocate exception frame and save minimal context. */
    mov     a0, sp
    addi    sp, sp, -XT_STK_FRMSZ
    s32i    a0, sp, XT_STK_A1
    #if XCHAL_HAVE_WINDOWED
    s32e    a0, sp, -12                     /* for debug backtrace */
    #endif
    rsr     a0, PS                          /* save interruptee's PS */
    s32i    a0, sp, XT_STK_PS
    rsr     a0, EPC_X                       /* save interruptee's PC */
    s32i    a0, sp, XT_STK_PC
    rsr     a0, EXCSAVE_X                   /* save interruptee's a0 */
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
#if CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_5
    get_int_status_tg1wdt a0
    bnez    a0, 1f

    /* TODO: Clear the MEMACCESS_ERR interrupt status. */
#elif CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_4
    rsr     a0, INTERRUPT
    extui   a0, a0, ETS_MEMACCESS_ERR_INUM, 1 /* get cacheerr int bit */
    beqz    a0, 1f
#endif /* CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_5 */

    /* Kill this interrupt; we cannot reset it. */
    rsr     a0, INTENABLE
    movi    a4, ~(1<<ETS_MEMACCESS_ERR_INUM)
    and     a0, a4, a0
    wsr     a0, INTENABLE
    movi    a0, PANIC_RSN_CACHEERR
    j 9f
1:
#if CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_5
   /* Clear the WDT interrupt status. */
    wdt_clr_intr_status TIMERG1
#endif /* CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL_5 */
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

    /* Call panic handler */
    mov     a6,sp
    call4   panicHandler

    call0   _xt_context_restore
    l32i    a0, sp, XT_STK_PS               /* retrieve interruptee's PS */
    wsr     a0, PS
    l32i    a0, sp, XT_STK_PC               /* retrieve interruptee's PC */
    wsr     a0, EPC_X
    l32i    a0, sp, XT_STK_A0               /* retrieve interruptee's A0 */
    l32i    sp, sp, XT_STK_A1               /* remove exception frame */
    rsync                                   /* ensure PS and EPC written */

    rsr     a0, EXCSAVE_X                   /* restore a0 */
    rfi     RFI_X


#if CONFIG_ESP32_ECO3_CACHE_LOCK_FIX && CONFIG_ESP_INT_WDT
#if CONFIG_BTDM_CTRL_HLI
#define APB_ITCTRL              (0x3f00)
#define APB_DCRSET              (0x200c)

#define ERI_ADDR(APB)           (0x100000 + (APB))

    .align      4
.handle_multicore_debug_int:

    wsr     a2, depc                        /* temp storage */

    rsr.ccount  a2
    addmi   a2, a2, (CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ*50)
    wsr     a2, CCOMPARE2

    /* Enable Integration Mode */
    movi    a2, ERI_ADDR(APB_ITCTRL)
    rer     a0, a2
    addi    a0, a0, 1
    wer     a0, a2

    /* Enable and emit BreakOut signal */
    movi    a2, ERI_ADDR(APB_DCRSET)
    rer     a0, a2
    movi    a2, 0x1020000
    or      a0, a2, a0
    movi    a2, ERI_ADDR(APB_DCRSET)
    wer     a0, a2

    .rept   4
    nop
    .endr

    /* Enable Normal Mode */
    movi    a2, ERI_ADDR(APB_ITCTRL)
    rer     a0, a2
    movi    a2, ~0x1
    and     a0, a2, a0
    movi    a2, ERI_ADDR(APB_ITCTRL)
    wer     a0, a2

    rsr     a2, depc

    rsr     a0, EXCSAVE_5                   /* restore a0 */
    rfi     5
#endif /* CONFIG_BTDM_CTRL_HLI */

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



    .align      4
.handle_livelock_int:

    getcoreid   a5

    /* Save A2, A3, A4 so we can use those registers */
    movi    a0, LX_INTR_STACK_SIZE
    mull    a5, a5, a0
    movi    a0, _lx_intr_stack
    add     a0, a0, a5
    s32i    a2, a0, LX_INTR_A2_OFFSET
    s32i    a3, a0, LX_INTR_A3_OFFSET
    s32i    a4, a0, LX_INTR_A4_OFFSET

    /* Here, we can use a0, a2, a3, a4, a5 registers */
    getcoreid   a5

    rsil    a0, SOC_DPORT_WORKAROUND_DIS_INTERRUPT_LVL /* disable nested interrupt */

    beqz    a5, 1f
    movi    a2, _lx_intr_livelock_app
    l32i    a3, a2, 0
    addi    a3, a3, 1
    s32i    a3, a2, 0

    /* Dual core synchronization, ensuring that both cores enter interrupts */
1:  movi    a4, 0x1
    movi    a2, _lx_intr_livelock_sync
    addx4   a3, a5, a2
    s32i    a4, a3, 0

1:  movi    a2, _lx_intr_livelock_sync
    movi    a3, 1
    addx4   a3, a3, a2
    l32i    a2, a2, 0
    l32i    a3, a3, 0
    and     a2, a2, a3
    beqz    a2, 1b

    beqz    a5, 1f                          /* Pro cpu (Core 0) jump bypass */

    movi    a2, _lx_intr_livelock_app
    l32i    a2, a2, 0
    bnei    a2, 2, 1f
    movi    a2, _lx_intr_livelock_counter   /* _lx_intr_livelock_counter++ */
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
    movi    a2, _lx_intr_livelock_app
    l32i    a2, a2, 0
    beqi    a2, 2, 8f
    j       3f

2:  movi    a2, _lx_intr_livelock_pro
    l32i    a4, a2, 0
    addi    a4, a4, 1
    s32i    a4, a2, 0

    movi    a2, _lx_intr_livelock_sync
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
    then restore registers and exit highint5/4.
    */
3:  intr_matrix_map DPORT_APP_MAC_INTR_MAP_REG, ETS_TG1_WDT_LEVEL_INTR_SOURCE, 16
    j       9f

    /*
    Here, App cpu (Core 1) has exited isr, Pro cpu (Core 0) help the
    App cpu map tg1 1st stage timeout interrupt clear tg1 interrupt.
    */
4:  intr_matrix_map DPORT_APP_MAC_INTR_MAP_REG, ETS_TG1_WDT_LEVEL_INTR_SOURCE, ETS_T1_WDT_INUM

1:  movi    a2, _lx_intr_livelock_sync
    movi    a4, 1
    addx4   a3, a4, a2
    l32i    a2, a2, 0
    l32i    a3, a3, 0
    and     a2, a2, a3
    beqz    a2, 1b                          /* Wait for App cpu to enter highint5/4 again */

    wdt_clr_intr_status TIMERG1
    j       9f

    /* Feed watchdog */
8:  wdt_feed    TIMERG1

9:  wsr     a0, PS                          /* restore interrupt level */

    movi    a0, 0
    beqz    a5, 1f
    movi    a2, _lx_intr_livelock_app
    l32i    a3, a2, 0
    bnei    a3, 2, 1f
    s32i    a0, a2, 0

1:  bnez    a5, 2f
    movi    a2, _lx_intr_livelock_pro
    s32i    a0, a2, 0
2:  movi    a2, _lx_intr_livelock_sync
    addx4   a2, a5, a2
    s32i    a0, a2, 0

    /* Done. Restore registers and return. */
    movi    a0, LX_INTR_STACK_SIZE
    mull    a5, a5, a0
    movi    a0, _lx_intr_stack
    add     a0, a0, a5
    l32i    a2, a0, LX_INTR_A2_OFFSET
    l32i    a3, a0, LX_INTR_A3_OFFSET
    l32i    a4, a0, LX_INTR_A4_OFFSET
    rsync                                   /* ensure register restored */

    rsr     a5, depc

    rsr     a0, EXCSAVE_X                   /* restore a0 */
    rfi     RFI_X

#endif

/* The linker has no reason to link in this file; all symbols it exports are already defined
   (weakly!) in the default int handler. Define a symbol here so we can use it to have the
   linker inspect this anyway. */

    .global ld_include_highint_hdl
ld_include_highint_hdl:
