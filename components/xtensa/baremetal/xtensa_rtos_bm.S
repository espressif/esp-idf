/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "xtensa_rtos.h"


/*
*******************************************************************************
*                                            _bmxt_int_enter
*                                       void _bmxt_int_enter(void)
*
* Implements the Xtensa RTOS porting layer's XT_RTOS_INT_ENTER function for
* Bare Metal. Currently just stubs that do nothing.
*
*******************************************************************************
*/
    .globl  _bmxt_int_enter
    .type   _bmxt_int_enter,@function
    .align  4
_bmxt_int_enter:

    ret


/*
*******************************************************************************
*                                            _bmxt_int_exit
*                                       void _bmxt_int_exit(void)
*
* Implements the Xtensa RTOS porting layer's XT_RTOS_INT_EXIT function for
* Bare Metal. Currently just stubs that do nothing.
*
*******************************************************************************
*/
    .globl  _bmxt_int_exit
    .type   _bmxt_int_exit,@function
    .align  4
_bmxt_int_exit:

    ret


/*
**********************************************************************************************************
*                                           _bmxt_timer_int
*                                      void _bmxt_timer_int(void)
*
* Implements the Xtensa RTOS porting layer's XT_RTOS_TIMER_INT function for Bare Metal.
* Called every timer interrupt. Currently just stubs that do nothing.
*
**********************************************************************************************************
*/
    .globl  _bmxt_timer_int
    .type   _bmxt_timer_int,@function
    .align  4
_bmxt_timer_int:

    ENTRY(16)
    RET(16)


/*
**********************************************************************************************************
*                                        _bmxt_task_coproc_state
*                                   void _bmxt_task_coproc_state(void)
*
* Implements the Xtensa RTOS porting layer's XT_RTOS_CP_STATE function for Bare Metal.
*
* Currently just stubs that do nothing.
*
**********************************************************************************************************
*/
#if XCHAL_CP_NUM > 0

    .globl  _bmxt_task_coproc_state
    .type   _bmxt_task_coproc_state,@function
    .align  4
_bmxt_task_coproc_state:

    ret

#endif /* XCHAL_CP_NUM > 0 */
