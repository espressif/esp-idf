/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "portmacro.h"
#include "freertos/FreeRTOSConfig.h"
#include "soc/soc_caps.h"

#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
#include "esp_private/hw_stack_guard.h"
#endif

    .global port_uxInterruptNesting
    .global port_xSchedulerRunning
    .global xIsrStackTop
    .global pxCurrentTCB
    .global vTaskSwitchContext
    .global xPortSwitchFlag
#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    .global xIsrStack
    .global port_offset_pxStack
    .global port_offset_pxEndOfStack
    .global esp_hw_stack_guard_monitor_stop
    .global esp_hw_stack_guard_monitor_start
    .global esp_hw_stack_guard_set_bounds
#endif /* CONFIG_ESP_SYSTEM_HW_STACK_GUARD */

    .section .text

/**
 * This function makes the RTOS aware about an ISR entering. It takes the
 * current task stack pointer and places it into the pxCurrentTCB.
 * It then loads the ISR stack into sp.
 * TODO: ISR nesting code improvements ?
 */

    .global rtos_int_enter
    .type rtos_int_enter, @function
rtos_int_enter:
#if CONFIG_IDF_TARGET_ESP32P4
    //TODO: IDF-7861
    /* preserve the return address */
    mv      t1, ra
    mv      t2, a0
#endif

    /* If the scheduler is not enabled, jump directly to the ISR handler */
#if ( configNUM_CORES > 1 )
    csrr    t6, mhartid                     /* t6 = coreID */
    slli    t6, t6, 2                       /* t6 = coreID * 4 */
    la      t0, port_xSchedulerRunning      /* t0 = &port_xSchedulerRunning */
    add     t0, t0, t6                      /* t0 = &port_xSchedulerRunning[coreID] */
    lw      t0, (t0)                        /* t0 = port_xSchedulerRunning[coreID] */
#else
    lw      t0, port_xSchedulerRunning      /* t0 = port_xSchedulerRunning */
#endif /* (configNUM_CORES > 1) */
    beq     t0, zero, rtos_int_enter_end    /* if (port_xSchedulerRunning[coreID] == 0) jump to rtos_int_enter_end */

    /* Increment the ISR nesting count */
    la      t3, port_uxInterruptNesting     /* t3 = &port_usInterruptNesting */
#if ( configNUM_CORES > 1 )
    add     t3, t3, t6                      /* t3 = &port_uxInterruptNesting[coreID] // t6 already contains coreID * 4 */
#endif /* ( configNUM_CORES > 1 ) */
    lw      t4, 0x0(t3)                     /* t4 = port_uxInterruptNesting[coreID] */
    addi    t5, t4, 1                       /* t5 = t4 + 1 */
    sw      t5, 0x0(t3)                     /* port_uxInterruptNesting[coreID] = t5 */

    /* If we reached here from another low-prio ISR, i.e, port_uxInterruptNesting[coreID] > 0, then skip stack pushing to TCB */
    bne     t4, zero, rtos_int_enter_end    /* if (port_uxInterruptNesting[coreID] > 0) jump to rtos_int_enter_end */

#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    /* esp_hw_stack_guard_monitor_stop(); */
    ESP_HW_STACK_GUARD_MONITOR_STOP_CPU0
#endif /* CONFIG_ESP_SYSTEM_HW_STACK_GUARD */

    /* Save the current sp in pxCurrentTCB[coreID] and load the ISR stack on to sp */
#if ( configNUM_CORES > 1 )
    la      t0, pxCurrentTCB                /* t0 = &pxCurrentTCB */
    add     t0, t0, t6                      /* t0 = &pxCurrentTCB[coreID] // t6 already contains coreID * 4 */
    lw      t0, (t0)                        /* t0 = pxCurrentTCB[coreID] */
    sw      sp, 0x0(t0)                     /* pxCurrentTCB[coreID] = sp */
    la      t0, xIsrStackTop                /* t0 = &xIsrStackTop */
    add     t0, t0, t6                      /* t0 = &xIsrStackTop[coreID] // t6 already contains coreID * 4 */
    lw      sp, 0x0(t0)                     /* sp = xIsrStackTop[coreID] */
#else
    lw      t0, pxCurrentTCB                /* t0 = pxCurrentTCB */
    sw      sp, 0x0(t0)                     /* pxCurrentTCB = sp */
    lw      sp, xIsrStackTop                /* sp = xIsrStackTop */
#endif /* ( configNUM_CORES > 1 ) */

#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    /* esp_hw_stack_guard_set_bounds(xIsrStack, xIsrStackTop); */
    la      a0, xIsrStack
    mv      a1, sp
    ESP_HW_STACK_GUARD_SET_BOUNDS_CPU0
    ESP_HW_STACK_GUARD_MONITOR_START_CPU0
#endif /* CONFIG_ESP_SYSTEM_HW_STACK_GUARD */

rtos_int_enter_end:
#if CONFIG_IDF_TARGET_ESP32P4
    //TODO: IDF-7861
    mv      ra, t1
#endif
    ret

/**
 * Restore the stack pointer of the next task to run.
 */
    .global rtos_int_exit
    .type rtos_int_exit, @function
rtos_int_exit:

    /* Skip if the scheduler was not started */
#if ( configNUM_CORES > 1 )
    csrr    t1, mhartid                     /* t1 = coreID */
    slli    t1, t1, 2                       /* t1 = t1 * 4 */
    la      t0, port_xSchedulerRunning      /* t0 = &port_xSchedulerRunning */
    add     t0, t0, t1                      /* t0 = &port_xSchedulerRunning[coreID] */
    lw      t0, (t0)                        /* t0 = port_xSchedulerRunning[coreID] */
#else
    lw      t0, port_xSchedulerRunning      /* t0 = port_xSchedulerRunning */
#endif /* ( configNUM_CORES > 1 ) */
    beq     t0, zero, rtos_int_exit_end     /* if (port_uxSchewdulerRunning == 0) jump to rtos_int_exit_end */

    /* Decrement interrupt nesting counter */
    la      t2, port_uxInterruptNesting     /* t2 = &port_uxInterruptNesting */
#if ( configNUM_CORES > 1 )
    add     t2, t2, t1                      /* t2 = &port_uxInterruptNesting[coreID] // t1 already contains coreID * 4 */
#endif
    lw      t3, 0x0(t2)                     /* t3 = port_uxInterruptNesting[coreID] */

    /* If the interrupt nesting counter is already zero, then protect against underflow */
    beq     t3, zero, isr_skip_decrement    /* if (port_uxInterruptNesting[coreID] == 0) jump to isr_skip_decrement */
    addi    t3, t3, -1                      /* t3 = t3 - 1 */
    sw      t3, 0x0(t2)                     /* port_uxInterruptNesting[coreID] = t3 */

isr_skip_decrement:

    /* We may still have interrupts pending. Skip the section below and exit */
    bne     t3, zero, rtos_int_exit_end     /* (if port_uxInterruptNesting[coreID] > 0) jump to rtos_int_exit_end */

    /* Schedule the next task if an yield is pending */
    la      t0, xPortSwitchFlag             /* t0 = &xPortSwitchFlag */
#if ( configNUM_CORES > 1 )
    add     t0, t0, t1                      /* t0 = &xPortSwitchFlag[coreID] // t1 already contains coreID * 4 */
#endif /* ( configNUM_CORES > 1 ) */
    lw      t2, 0x0(t0)                     /* t2 = xPortSwitchFlag[coreID] */
    beq     t2, zero, no_switch             /* if (xPortSwitchFlag[coreID] == 0) jump to no_switch */

    /* Save the return address on the stack and create space on the stack for the c-routine call to schedule
     * the next task. Stack pointer for RISC-V should always be 16 byte aligned. After the switch, restore
     * the return address and sp.
     */
    addi    sp, sp, -16                     /* sp = sp - 16 */
    sw      ra, 0(sp)                       /* sp = ra */
    call    vTaskSwitchContext              /* vTaskSwitchContext() */
    lw      ra, 0(sp)                       /* ra = sp */
    addi    sp, sp, 16                      /* sp = sp + 16 */

    /* Clear the switch pending flag */
    la      t0, xPortSwitchFlag             /* t0 = &xPortSwitchFlag */
#if ( configNUM_CORES > 1 )
    /* c routine vTaskSwitchContext may change the temp registers, so we read again */
    csrr    t3, mhartid                     /* t3 = coreID */
    slli    t3, t3, 2                       /* t3 = t3 * 4 */
    add     t0, t0, t3                      /* t0 = &xPortSwitchFlag[coreID] */
#endif /* ( configNUM_CORES > 1 ) */
    mv      t2, zero                        /* t2 = 0 */
    sw      t2, 0x0(t0)                     /* xPortSwitchFlag[coreID] = t2 */

no_switch:

#if SOC_INT_CLIC_SUPPORTED
    /* Recover the stack of next task and prepare to exit */
    la      a0, pxCurrentTCB                /* a0 = &pxCurrentTCB */
#if ( configNUM_CORES > 1 )
    csrr    t3, mhartid                     /* t3 = coreID */
    slli    t3, t3, 2                       /* t3 = t3 * 4 */
    add     a0, a0, t3                      /* a0 = &pxCurrentTCB[coreID] */
#endif /* ( configNUM_CORES > 1 ) */
    lw      a0, (a0)                        /* a0 = pxCurrentTCB[coreID] */
    lw      a0, 0x0(a0)                     /* a0 = previous sp */
#else
#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    /* esp_hw_stack_guard_monitor_stop(); */
    ESP_HW_STACK_GUARD_MONITOR_STOP_CPU0
#endif /* CONFIG_ESP_SYSTEM_HW_STACK_GUARD */

    /* Recover the stack of next task */
    lw      t0, pxCurrentTCB
    lw      sp, 0x0(t0)

#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    /* esp_hw_stack_guard_set_bounds(pxCurrentTCB[0]->pxStack,
     *                               pxCurrentTCB[0]->pxEndOfStack);
     */
    lw      a0, PORT_OFFSET_PX_STACK(t0)
    lw      a1, PORT_OFFSET_PX_END_OF_STACK(t0)
    ESP_HW_STACK_GUARD_SET_BOUNDS_CPU0
    /* esp_hw_stack_guard_monitor_start(); */
    ESP_HW_STACK_GUARD_MONITOR_START_CPU0
#endif /* CONFIG_ESP_SYSTEM_HW_STACK_GUARD */
#endif /* SOC_INT_CLIC_SUPPORTED */

rtos_int_exit_end:
    ret
