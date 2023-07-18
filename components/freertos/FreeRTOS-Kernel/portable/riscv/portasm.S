/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "portmacro.h"
#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
#include "esp_private/hw_stack_guard.h"
#endif

    .global uxInterruptNesting
    .global uxSchedulerRunning
    .global xIsrStackTop
#if CONFIG_IDF_TARGET_ESP32P4
//TODO: IDF-7566
    .global xIsrStackTop1
#endif
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
 * This function makes the RTOS aware about a ISR entering, it takes the
 * current task stack saved, places into the TCB, loads the ISR stack.
 * TODO: ISR nesting code improvements ?
 */

    .global rtos_int_enter
    .type rtos_int_enter, @function
rtos_int_enter:
#if CONFIG_IDF_TARGET_ESP32P4
    /* needs jira for p4 */
    /* preserve the return address */
    mv t1, ra
    mv t2, a0
#endif

//TODO: IDF-7566
#if !CONFIG_IDF_TARGET_ESP32P4
    /* scheduler not enabled, jump directly to ISR handler */
    lw t0, uxSchedulerRunning
    beq t0,zero, rtos_enter_end
#else
    /* scheduler not enabled, jump directly to ISR handler */
    csrr t6, mhartid          /* t6 = coreID */
    slli t6, t6, 2            /* t6 = coreID * 4 */
    la t0, uxSchedulerRunning /* t0 = &uxSchedulerRunning */
    add t0, t0, t6            /* t0 = &uxSchedulerRunning[coreID] */
    lw t0, (t0)               /* t0 = uxSchedulerRunning[coreID] */
    beq t0,zero, rtos_enter_end
#endif

    /* increments the ISR nesting count */
    la t3, uxInterruptNesting
#if CONFIG_IDF_TARGET_ESP32P4
//TODO: IDF-7566
    add t3, t3, t6
#endif
    lw t4, 0x0(t3)
    addi t5,t4,1
    sw  t5, 0x0(t3)

    /* If reached here from another low-prio ISR, skip stack pushing to TCB */
    bne t4,zero, rtos_enter_end

#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    /* esp_hw_stack_guard_monitor_stop(); */
    ESP_HW_STACK_GUARD_MONITOR_STOP_CPU0
#endif /* CONFIG_ESP_SYSTEM_HW_STACK_GUARD */

    /* Save current TCB and load the ISR stack */
//TODO: IDF-7566
#if !CONFIG_IDF_TARGET_ESP32P4
    lw  t0, pxCurrentTCB
    sw  sp, 0x0(t0)
    lw  sp, xIsrStackTop
#else
    la  t0, pxCurrentTCB      /* t0 = &pxCurrentTCB */
    add t0, t0, t6            /* t0 = &pxCurrentTCB[coreID] */
    lw  t0, (t0)              /* t0 = pxCurrentTCB[coreID] */
    sw 	t2, 0x0(t0)
    lw  sp, xIsrStackTop
    csrr t6, mhartid
    beq t6, zero, rtos_enter_end
    lw  sp, xIsrStackTop1
#endif

#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    /* esp_hw_stack_guard_set_bounds(xIsrStack, xIsrStackTop); */
    la      a0, xIsrStack
    mv      a1, sp
    ESP_HW_STACK_GUARD_SET_BOUNDS_CPU0
    ESP_HW_STACK_GUARD_MONITOR_START_CPU0
#endif /* CONFIG_ESP_SYSTEM_HW_STACK_GUARD */

rtos_enter_end:
#if CONFIG_IDF_TARGET_ESP32P4
    /* needs jira for p4 */
    mv  ra, t1
#endif
    ret

/**
 * Recovers the next task to run stack pointer.
 */
    .global rtos_int_exit
    .type rtos_int_exit, @function
rtos_int_exit:
    /* may skip RTOS aware interrupt since scheduler was not started */

//TODO: IDF-7566
#if !CONFIG_IDF_TARGET_ESP32P4
    lw t0, uxSchedulerRunning
#else
    csrr t1, mhartid
    slli t1, t1, 2
    la t0, uxSchedulerRunning      /* t0 = &uxSchedulerRunning */
    add t0, t0, t1                 /* t0 = &uxSchedulerRunning[coreID] */
    lw t0, (t0)
#endif
    beq t0,zero, rtos_exit_end

    /* update nesting interrupts counter */
    la t2, uxInterruptNesting
#if CONFIG_IDF_TARGET_ESP32P4
//TODO: IDF-7566
    add t2, t2, t1
#endif
    lw t3, 0x0(t2)

    /* Already zero, protect against underflow */
    beq t3, zero, isr_skip_decrement
    addi t3,t3, -1
    sw  t3, 0x0(t2)

isr_skip_decrement:

    /* may still have interrupts pending, skip section below and exit */
    bne t3,zero,rtos_exit_end

    /* Schedule the next task if a yield is pending */
    la t0, xPortSwitchFlag
#if CONFIG_IDF_TARGET_ESP32P4
//TODO: IDF-7566
    add t0, t0, t1
#endif
    lw t2, 0x0(t0)
    beq t2, zero, no_switch

    /* preserve return address and schedule next task
       stack pointer for riscv should always be 16 byte aligned */
    addi sp,sp,-16
    sw  ra, 0(sp)
    call vTaskSwitchContext
    lw  ra, 0(sp)
    addi sp, sp, 16

    /* Clears the switch pending flag */
    la t0, xPortSwitchFlag
#if CONFIG_IDF_TARGET_ESP32P4
//TODO: IDF-7566
    /* c routine vTaskSwitchContext may change the temp registers, so we read again */
    csrr t3, mhartid
    slli t3, t3, 2
    add t0, t0, t3
#endif
    mv t2, zero
    sw  t2, 0x0(t0)

no_switch:

#if !CONFIG_IDF_TARGET_ESP32P4   //TODO: IDF-7566
#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    /* esp_hw_stack_guard_monitor_stop(); */
    ESP_HW_STACK_GUARD_MONITOR_STOP_CPU0
#endif /* CONFIG_ESP_SYSTEM_HW_STACK_GUARD */

    /* Recover the stack of next task */
    lw t0, pxCurrentTCB
    lw sp, 0x0(t0)

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

#else
    /* Recover the stack of next task and prepare to exit : */
    la a0, pxCurrentTCB
    /* We may come here from a branch, so we re-cal here */
    csrr t3, mhartid
    slli t3, t3, 2
    add a0, a0, t3    /* a0 = &pxCurrentTCB[coreID] */
    lw a0, (a0)       /* a0 = pxCurrentTCB[coreID] */
    lw a0, 0x0(a0)    /* a0 = previous sp */
#endif  //#if !CONFIG_IDF_TARGET_ESP32P4

rtos_exit_end:
    ret
