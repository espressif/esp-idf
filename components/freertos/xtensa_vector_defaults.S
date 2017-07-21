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

#include "xtensa_rtos.h"
#include "esp_panic.h"
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
    movi    a0,PANIC_RSN_DEBUGEXCEPTION
    wsr     a0,EXCCAUSE
    call0   _xt_panic                       /* does not return */
    rsr     a0, EXCSAVE+XCHAL_DEBUGLEVEL
    rfi     XCHAL_DEBUGLEVEL

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

