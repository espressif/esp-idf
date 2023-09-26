/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "portmacro.h"
#include "freertos/FreeRTOSConfig.h"
#include "soc/soc_caps.h"

.extern pxCurrentTCBs

#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
#include "esp_private/hw_stack_guard.h"
#endif

    .global port_uxInterruptNesting
    .global port_xSchedulerRunning
    .global xIsrStackTop
    .global pxCurrentTCBs
    .global vTaskSwitchContext
    .global xPortSwitchFlag
#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    .global xIsrStackBottom
    .global port_offset_pxStack
    .global port_offset_pxEndOfStack
    .global esp_hw_stack_guard_monitor_stop
    .global esp_hw_stack_guard_monitor_start
    .global esp_hw_stack_guard_set_bounds
#endif /* CONFIG_ESP_SYSTEM_HW_STACK_GUARD */

    .section .text

/**
 * This function makes the RTOS aware about an ISR entering. It takes the
 * current task stack pointer and places it into the pxCurrentTCBs.
 * It then loads the ISR stack into sp.
 * TODO: ISR nesting code improvements ?
 * In the routines below, let's use a0-a5 registers to let the compiler generate
 * 16-bit instructions.
 */
    .global rtos_int_enter
    .type rtos_int_enter, @function
rtos_int_enter:
#if ( configNUM_CORES > 1 )
    csrr    a5, mhartid                     /* a5 = coreID */
    slli    a5, a5, 2                       /* a5 = coreID * 4 */
    la      a0, port_xSchedulerRunning      /* a0 = &port_xSchedulerRunning */
    add     a0, a0, a5                      /* a0 = &port_xSchedulerRunning[coreID] */
    lw      a0, (a0)                        /* a0 = port_xSchedulerRunning[coreID] */
#else
    lw      a0, port_xSchedulerRunning      /* a0 = port_xSchedulerRunning */
#endif /* ( configNUM_CORES > 1 ) */
    beqz    a0, rtos_int_enter_end          /* if (port_xSchedulerRunning[coreID] == 0) jump to rtos_int_enter_end */

    /* Increment the ISR nesting count */
    la      a0, port_uxInterruptNesting     /* a0 = &port_uxInterruptNesting */
#if ( configNUM_CORES > 1 )
    add     a0, a0, a5                      /* a0 = &port_uxInterruptNesting[coreID] // a5 already contains coreID * 4 */
#endif /* ( configNUM_CORES > 1 ) */
    lw      a1, 0(a0)                       /* a1 = port_uxInterruptNesting[coreID] */
    addi    a2, a1, 1                       /* a2 = a1 + 1 */
    sw      a2, 0(a0)                       /* port_uxInterruptNesting[coreID] = a2 */

    /* If we reached here from another low-priority ISR, i.e, port_uxInterruptNesting[coreID] > 0, then skip stack pushing to TCB */
    bnez    a1, rtos_int_enter_end          /* if (port_uxInterruptNesting[coreID] > 0) jump to rtos_int_enter_end */

#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    /* esp_hw_stack_guard_monitor_stop(); pass the scratch registers */
    ESP_HW_STACK_GUARD_MONITOR_STOP_CUR_CORE a0 a1
#endif /* CONFIG_ESP_SYSTEM_HW_STACK_GUARD */

    /* Save the current sp in pxCurrentTCBs[coreID] and load the ISR stack on to sp */
#if ( configNUM_CORES > 1 )
    la      a0, pxCurrentTCBs               /* a0 = &pxCurrentTCBs */
    add     a0, a0, a5                      /* a0 = &pxCurrentTCBs[coreID] // a5 already contains coreID * 4 */
    lw      a0, (a0)                        /* a0 = pxCurrentTCBs[coreID] */
    sw      sp, 0(a0)                       /* pxCurrentTCBs[coreID] = sp */
    la      a0, xIsrStackTop                /* a0 = &xIsrStackTop */
    add     a0, a0, a5                      /* a0 = &xIsrStackTop[coreID] // a5 already contains coreID * 4 */
    lw      sp, (a0)                        /* sp = xIsrStackTop[coreID] */
#else
    lw      a0, pxCurrentTCBs               /* a0 = pxCurrentTCBs */
    sw      sp, 0(a0)                       /* pxCurrentTCBs[0] = sp */
    lw      sp, xIsrStackTop                /* sp = xIsrStackTop */
#endif /* ( configNUM_CORES > 1 ) */

#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    /* Prepare the parameters for esp_hw_stack_guard_set_bounds(xIsrStackBottom, xIsrStackTop); */
#if ( configNUM_CORES > 1 )
    /* Load the xIsrStack for the current core and set the new bounds */
    la      a0, xIsrStackBottom
    add     a0, a0, a5                      /* a0 = &xIsrStackBottom[coreID] */
    lw      a0, (a0)                        /* a0 = xIsrStackBottom[coreID] */
#else
    lw      a0, xIsrStackBottom
#endif /* ( configNUM_CORES > 1 ) */
    mv      a1, sp
    /* esp_hw_stack_guard_set_bounds(xIsrStackBottom[coreID], xIsrStackTop[coreID]);
     */
    ESP_HW_STACK_GUARD_SET_BOUNDS_CUR_CORE a2
    ESP_HW_STACK_GUARD_MONITOR_START_CUR_CORE a0 a1
#endif /* CONFIG_ESP_SYSTEM_HW_STACK_GUARD */

rtos_int_enter_end:
    ret

/**
 * Restore the stack pointer of the next task to run.
 */
    .global rtos_int_exit
    .type rtos_int_exit, @function
rtos_int_exit:
#if ( configNUM_CORES > 1 )
    csrr    a1, mhartid                     /* a1 = coreID */
    slli    a1, a1, 2                       /* a1 = a1 * 4 */
    la      a0, port_xSchedulerRunning      /* a0 = &port_xSchedulerRunning */
    add     a0, a0, a1                      /* a0 = &port_xSchedulerRunning[coreID] */
    lw      a0, (a0)                        /* a0 = port_xSchedulerRunning[coreID] */
#else
    lw      a0, port_xSchedulerRunning      /* a0 = port_xSchedulerRunning */
#endif /* ( configNUM_CORES > 1 ) */
    beqz    a0, rtos_int_exit_end           /* if (port_uxSchewdulerRunning == 0) jump to rtos_int_exit_end */

    /* Update nesting interrupts counter */
    la      a0, port_uxInterruptNesting     /* a0 = &port_uxInterruptNesting */
#if ( configNUM_CORES > 1 )
    add     a0, a0, a1                      /* a0 = &port_uxInterruptNesting[coreID] // a1 already contains coreID * 4 */
#endif /* ( configNUM_CORES > 1 ) */
    lw      a2, 0(a0)                       /* a2 = port_uxInterruptNesting[coreID] */

    /* Already zero, protect against underflow */
    beqz    a2, isr_skip_decrement          /* if (port_uxInterruptNesting[coreID] == 0) jump to isr_skip_decrement */
    addi    a2, a2, -1                      /* a2 = a2 - 1 */
    sw      a2, 0(a0)                       /* port_uxInterruptNesting[coreID] = a2 */
    /* May still have interrupts pending, skip section below and exit */
    bnez    a2, rtos_int_exit_end

isr_skip_decrement:
    /* If the CPU reached this label, a2 (uxInterruptNesting) is 0 for sure */

    /* Schedule the next task if a yield is pending */
    la      a0, xPortSwitchFlag             /* a0 = &xPortSwitchFlag */
#if ( configNUM_CORES > 1 )
    add     a0, a0, a1                      /* a0 = &xPortSwitchFlag[coreID] // a1 already contains coreID * 4  */
#endif /* ( configNUM_CORES > 1 ) */
    lw      a2, 0(a0)                       /* a2 = xPortSwitchFlag[coreID] */
    beqz    a2, no_switch                   /* if (xPortSwitchFlag[coreID] == 0) jump to no_switch */

    /* Preserve return address and schedule next task. To speed up the process, instead of allocating stack
     * space, let's use a callee-saved register: s0. Since the caller is not using it, let's use it. */
    mv      s0, ra
    call    vTaskSwitchContext
    mv      ra, s0

    /* Clears the switch pending flag */
    la      a0, xPortSwitchFlag             /* a0 = &xPortSwitchFlag */
#if ( configNUM_CORES > 1 )
    /* C routine vTaskSwitchContext may change the temp registers, so we read again */
    csrr    a1, mhartid                     /* a1 = coreID */
    slli    a1, a1, 2                       /* a1 = a1 * 4 */
    add     a0, a0, a1                      /* a0 = &xPortSwitchFlag[coreID]; */
#endif /* ( configNUM_CORES > 1 ) */
    sw      zero, 0(a0)                     /* xPortSwitchFlag[coreID] = 0; */

no_switch:

#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    /* esp_hw_stack_guard_monitor_stop(); pass the scratch registers */
    ESP_HW_STACK_GUARD_MONITOR_STOP_CUR_CORE a0 a1
#endif /* CONFIG_ESP_SYSTEM_HW_STACK_GUARD */


#if ( configNUM_CORES > 1 )
    /* Recover the stack of next task and prepare to exit */
    csrr    a1, mhartid
    slli    a1, a1, 2
    la      a0, pxCurrentTCBs               /* a0 = &pxCurrentTCBs */
    add     a0, a0, a1                      /* a0 = &pxCurrentTCBs[coreID] */
    lw      a0, 0(a0)                       /* a0 = pxCurrentTCBs[coreID] */
    lw      sp, 0(a0)                       /* sp = previous sp */
#else
    /* Recover the stack of next task */
    lw      a0, pxCurrentTCBs
    lw      sp, 0(a0)
#endif /* ( configNUM_CORES > 1 ) */


#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    /* esp_hw_stack_guard_set_bounds(pxCurrentTCBs[0]->pxStack,
     *                               pxCurrentTCBs[0]->pxEndOfStack);
     */
    lw      a1, PORT_OFFSET_PX_END_OF_STACK(a0)
    lw      a0, PORT_OFFSET_PX_STACK(a0)
    ESP_HW_STACK_GUARD_SET_BOUNDS_CUR_CORE a2
    /* esp_hw_stack_guard_monitor_start(); */
    ESP_HW_STACK_GUARD_MONITOR_START_CUR_CORE a0 a1
#endif /* CONFIG_ESP_SYSTEM_HW_STACK_GUARD */

rtos_int_exit_end:
    ret
