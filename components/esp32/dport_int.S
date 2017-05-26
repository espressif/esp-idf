#include <xtensa/coreasm.h>
#include <xtensa/corebits.h>
#include <xtensa/config/system.h>
#include <xtensa/simcall.h>
#include "esp_panic.h"
#include "sdkconfig.h"
#include "soc/soc.h"
#include "soc/dport_reg.h"
#include "freertos/xtensa_context.h"


#define L5_INTR_STACK_SIZE  8
#define L5_INTR_A2_OFFSET   0
#define L5_INTR_A3_OFFSET   4
    .data
_l5_intr_stack:
    .space      L5_INTR_STACK_SIZE

    .section .iram1,"ax"
    .global     xt_highint5
    .type       _xt_highint5,@function
    .align      4
xt_highint5:

    #ifdef XT_INTEXC_HOOKS
    /* Call interrupt hook if present to (pre)handle interrupts. */
    movi    a0, _xt_intexc_hooks
    l32i    a0, a0, 5<<2
    beqz    a0, 1f
.Ln_xt_highint5_call_hook:
    callx0  a0                              /* must NOT disturb stack! */
1:
    #endif

    /* This section is for access dport register protection */
    /* Allocate exception frame and save minimal context. */
    /* Because the interrupt cause code have protection that only
       allow one cpu enter in L5 interrupt at one time, so 
       there needn't have two _l5_intr_stack for each cpu */

    movi    a0, _l5_intr_stack
    s32i    a2, a0, L5_INTR_A2_OFFSET
    s32i    a3, a0, L5_INTR_A3_OFFSET

    /* Check interrupt */
    rsr     a0, INTERRUPT
    extui   a0, a0, ETS_DPORT_INUM, 1 /* get dport int bit */
    beqz    a0, 1f

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

1:
    movi    a0, _l5_intr_stack
    l32i    a2, a0, L5_INTR_A2_OFFSET
    l32i    a3, a0, L5_INTR_A3_OFFSET
    rsync                                   /* ensure register restored */

    rsr     a0, EXCSAVE_5                   /* restore a0 */
    rfi     5


    .align  4
.L_xt_highint5_exit:
    rsr     a0, EXCSAVE_5                   /* restore a0 */
    rfi     5


/* The linker has no reason to link in this file; all symbols it exports are already defined
   (weakly!) in the default int handler. Define a symbol here so we can use it to have the 
   linker inspect this anyway. */

    .global ld_include_dport_int
ld_include_dport_int:
