/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "xtensa_rtos.h"
#include "esp_private/panic_reason.h"
#include "sdkconfig.h"
#include "soc/soc.h"

/*
This file contains the default handlers for the high interrupt levels as well as some specialized exceptions.
The default behaviour is to just exit the interrupt or call the panic handler on the exceptions
*/


#if XCHAL_HAVE_DEBUG
    .global    xt_debugexception
    .weak xt_debugexception
    .set xt_debugexception, _xt_debugexception
    .section .iram1,"ax"
    .type       _xt_debugexception,@function
    .align      4

_xt_debugexception:
#if (CONFIG_ESP32_ECO3_CACHE_LOCK_FIX && CONFIG_BTDM_CTRL_HLI)
    s32i    a0, sp, XT_STK_EXIT

#define XT_DEBUGCAUSE_DI        (5)
    getcoreid   a0
#if (CONFIG_BTDM_CTRL_PINNED_TO_CORE == PRO_CPU_NUM)
    beqz    a0, 1f
#else
    bnez    a0, 1f
#endif

    rsr     a0, DEBUGCAUSE
    extui   a0, a0, XT_DEBUGCAUSE_DI, 1
    bnez    a0, _xt_debug_di_exc
1:
#endif //(CONFIG_ESP32_ECO3_CACHE_LOCK_FIX && CONFIG_BTDM_CTRL_HLI)

    movi    a0,PANIC_RSN_DEBUGEXCEPTION
    wsr     a0,EXCCAUSE
    /* _xt_panic assumes a level 1 exception. As we're
       crashing anyhow, copy EPC & EXCSAVE from DEBUGLEVEL
       to level 1. */
    rsr     a0,(EPC + XCHAL_DEBUGLEVEL)
    wsr     a0,EPC_1
    rsr     a0,(EXCSAVE + XCHAL_DEBUGLEVEL)
    wsr     a0,EXCSAVE_1
#if CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
    J   _xt_panic_gdbstub                   /* For gdbstub we make jump */
#else
    call0   _xt_panic                       /* does not return */
#endif // CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
    rfi     XCHAL_DEBUGLEVEL

#if (CONFIG_ESP32_ECO3_CACHE_LOCK_FIX && CONFIG_BTDM_CTRL_HLI)
    .align  4
_xt_debug_di_exc:

    /*
    The delay time can be calculated by the following formula:
      T = ceil(0.25 + max(t1, t2)) us

      t1 = 80 / f1, t2 = (1 + 14/N) * 20 / f2

      f1: PSRAM access frequency, unit: MHz.
      f2: Flash access frequency, unit: MHz.

      When flash is slow/fast read, N = 1.
      When flash is DOUT/DIO read, N = 2.
      When flash is QOUT/QIO read, N = 4.

      And after testing, when CPU frequency is 240 MHz, it will take 1us to loop 27 times.
    */
#if defined(CONFIG_ESPTOOLPY_FLASHMODE_QIO) || defined(CONFIG_ESPTOOLPY_FLASHMODE_QOUT)

# if defined(CONFIG_ESPTOOLPY_FLASHFREQ_80M) && defined(CONFIG_SPIRAM_SPEED_80M)
    movi    a0, 54
# elif defined(CONFIG_ESPTOOLPY_FLASHFREQ_80M) && defined(CONFIG_SPIRAM_SPEED_40M)
    movi    a0, 81
# elif defined(CONFIG_ESPTOOLPY_FLASHFREQ_40M) && defined(CONFIG_SPIRAM_SPEED_40M)
    movi    a0, 81
# elif defined(CONFIG_ESPTOOLPY_FLASHFREQ_26M) && defined(CONFIG_SPIRAM_SPEED_40M)
    movi    a0, 108
# else
    movi    a0, 135
# endif

#elif defined(CONFIG_ESPTOOLPY_FLASHMODE_DIO) || defined(CONFIG_ESPTOOLPY_FLASHMODE_DOUT)

# if defined(CONFIG_ESPTOOLPY_FLASHFREQ_80M) && defined(CONFIG_SPIRAM_SPEED_80M)
    movi    a0, 81
# elif defined(CONFIG_ESPTOOLPY_FLASHFREQ_80M) && defined(CONFIG_SPIRAM_SPEED_40M)
    movi    a0, 81
# elif defined(CONFIG_ESPTOOLPY_FLASHFREQ_40M) && defined(CONFIG_SPIRAM_SPEED_40M)
    movi    a0, 135
# elif defined(CONFIG_ESPTOOLPY_FLASHFREQ_26M) && defined(CONFIG_SPIRAM_SPEED_40M)
    movi    a0, 189
# else
    movi    a0, 243
# endif

#else
    movi    a0, 243
#endif

1:  addi    a0, a0, -1         /* delay_us(N) */
    .rept   4
    nop
    .endr
    bnez    a0, 1b

    rsr     a0, EXCSAVE+XCHAL_DEBUGLEVEL
    rfi     XCHAL_DEBUGLEVEL
#endif //(CONFIG_ESP32_ECO3_CACHE_LOCK_FIX && CONFIG_BTDM_CTRL_HLI)
#endif /* Debug exception */


#if XCHAL_NUM_INTLEVELS >=2 && XCHAL_EXCM_LEVEL <2 && XCHAL_DEBUGLEVEL !=2
    .global    xt_highint2
    .weak xt_highint2
    .set xt_highint2, _xt_highint2
    .section .iram1,"ax"
    .type       _xt_highint2,@function
    .align      4
_xt_highint2:

    /* Default handler does nothing; just returns */
    .align  4
.L_xt_highint2_exit:
    rsr     a0, EXCSAVE_2                   /* restore a0 */
    rfi     2

#endif  /* Level 2 */

#if XCHAL_NUM_INTLEVELS >=3 && XCHAL_EXCM_LEVEL <3 && XCHAL_DEBUGLEVEL !=3

    .global    xt_highint3
    .weak xt_highint3
    .set xt_highint3, _xt_highint3
    .section .iram1,"ax"
    .type       _xt_highint3,@function
    .align      4
_xt_highint3:

    /* Default handler does nothing; just returns */

    .align  4
.L_xt_highint3_exit:
    rsr     a0, EXCSAVE_3                   /* restore a0 */
    rfi     3

#endif  /* Level 3 */

#if XCHAL_NUM_INTLEVELS >=4 && XCHAL_EXCM_LEVEL <4 && XCHAL_DEBUGLEVEL !=4

    .global    xt_highint4
    .weak xt_highint4
    .set xt_highint4, _xt_highint4
    .section .iram1,"ax"
    .type       _xt_highint4,@function
    .align      4
_xt_highint4:

    /* Default handler does nothing; just returns */

    .align  4
.L_xt_highint4_exit:
    rsr     a0, EXCSAVE_4                   /* restore a0 */
    rfi     4

#endif  /* Level 4 */

#if XCHAL_NUM_INTLEVELS >=5 && XCHAL_EXCM_LEVEL <5 && XCHAL_DEBUGLEVEL !=5

    .global    xt_highint5
    .weak xt_highint5
    .set xt_highint5, _xt_highint5
    .section .iram1,"ax"
    .type       _xt_highint5,@function
    .align      4
_xt_highint5:

    /* Default handler does nothing; just returns */

    .align  4
.L_xt_highint5_exit:
    rsr     a0, EXCSAVE_5                   /* restore a0 */
    rfi     5


#endif  /* Level 5 */

#if XCHAL_NUM_INTLEVELS >=6 && XCHAL_EXCM_LEVEL <6 && XCHAL_DEBUGLEVEL !=6

    .global    _xt_highint6
    .global    xt_highint6
    .weak xt_highint6
    .set xt_highint6, _xt_highint6
    .section .iram1,"ax"
    .type       _xt_highint6,@function
    .align      4
_xt_highint6:

    /* Default handler does nothing; just returns */

    .align  4
.L_xt_highint6_exit:
    rsr     a0, EXCSAVE_6                   /* restore a0 */
    rfi     6

#endif  /* Level 6 */

#if XCHAL_HAVE_NMI

    .global    _xt_nmi
    .global    xt_nmi
    .weak xt_nmi
    .set xt_nmi, _xt_nmi
    .section .iram1,"ax"
    .type       _xt_nmi,@function
    .align      4
_xt_nmi:

    /* Default handler does nothing; just returns */

    .align  4
.L_xt_nmi_exit:
    rsr     a0, EXCSAVE + XCHAL_NMILEVEL    /* restore a0 */
    rfi     XCHAL_NMILEVEL

#endif  /* NMI */
