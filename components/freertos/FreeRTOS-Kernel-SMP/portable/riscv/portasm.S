/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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
    .global pxCurrentTCBs
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
    /* scheduler not enabled, jump directly to ISR handler */
    lw t0, uxSchedulerRunning
    beq t0,zero, rtos_enter_end

    /* increments the ISR nesting count */
	la t3, uxInterruptNesting
	lw t4, 0x0(t3)
	addi t5,t4,1
	sw  t5, 0x0(t3)

    /* If reached here from another low-prio ISR, skip stack pushing to TCB */
	bne t4,zero, rtos_enter_end

#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    /* esp_hw_stack_guard_monitor_stop(); pass the scratch registers */
    ESP_HW_STACK_GUARD_MONITOR_STOP_CUR_CORE a0 a1
#endif /* CONFIG_ESP_SYSTEM_HW_STACK_GUARD */

    /* Save current TCB and load the ISR stack */
    lw  t0, pxCurrentTCBs
    sw  sp, 0x0(t0)
    lw  sp, xIsrStackTop

#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    /* esp_hw_stack_guard_set_bounds(xIsrStack, xIsrStackTop); */
    la      a0, xIsrStack
    mv      a1, sp
    ESP_HW_STACK_GUARD_SET_BOUNDS_CUR_CORE a2
    /* esp_hw_stack_guard_monitor_start(); */
    ESP_HW_STACK_GUARD_MONITOR_START_CUR_CORE a0 a1
#endif /* CONFIG_ESP_SYSTEM_HW_STACK_GUARD */

rtos_enter_end:
    ret

/**
 * @brief Restore the stack pointer of the next task to run.
 *
 * @param a0 Former mstatus
 *
 * @returns New mstatus
 */
    .global rtos_int_exit
    .type rtos_int_exit, @function
rtos_int_exit:
    /* To speed up this routine and because this current routine is only meant to be called from the interrupt
     * handler, let's use callee-saved registers instead of stack space. Registers `s3-s11` are not used by
     * the caller */
    mv s11, a0
    /* may skip RTOS aware interrupt since scheduler was not started */
    lw t0, uxSchedulerRunning
    beq t0,zero, rtos_exit_end

    /* update nesting interrupts counter */
    la t2, uxInterruptNesting
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
    lw t2, 0x0(t0)
    beq t2, zero, no_switch

    /* preserve return address and schedule next task
       stack pointer for riscv should always be 16 byte aligned */
    addi sp,sp,-16
    sw  ra, 0(sp)
    /* vTaskSwitchContext(xCoreID) now expects xCoreID as an argument, so the assembly calls below have been modified. xCoreID is hard-wired to 0 for single-core risc-v. */
    li a0, 0
    call vTaskSwitchContext
    lw  ra, 0(sp)
    addi sp, sp, 16

    /* Clears the switch pending flag */
    la t0, xPortSwitchFlag
    mv t2, zero
    sw  t2, 0x0(t0)

no_switch:

#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    /* esp_hw_stack_guard_monitor_stop(); */
    ESP_HW_STACK_GUARD_MONITOR_STOP_CUR_CORE a0 a1
#endif /* CONFIG_ESP_SYSTEM_HW_STACK_GUARD */

    /* Recover the stack of next task */
    lw t0, pxCurrentTCBs
    lw sp, 0x0(t0)

#if CONFIG_ESP_SYSTEM_HW_STACK_GUARD
    /* esp_hw_stack_guard_set_bounds(pxCurrentTCBs[0]->pxStack,
     *                               pxCurrentTCBs[0]->pxEndOfStack);
     */
    lw      a0, PORT_OFFSET_PX_STACK(t0)
    lw      a1, PORT_OFFSET_PX_END_OF_STACK(t0)
    ESP_HW_STACK_GUARD_SET_BOUNDS_CUR_CORE a2
    /* esp_hw_stack_guard_monitor_start(); */
    ESP_HW_STACK_GUARD_MONITOR_START_CUR_CORE a0 a1
#endif /* CONFIG_ESP_SYSTEM_HW_STACK_GUARD */

rtos_exit_end:
    mv a0, s11                         /* a0 = new mstatus */
    ret
