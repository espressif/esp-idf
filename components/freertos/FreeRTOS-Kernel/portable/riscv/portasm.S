/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

    .global uxInterruptNesting
    .global uxSchedulerRunning
    .global xIsrStackTop
    .global pxCurrentTCB
    .global vTaskSwitchContext
    .global xPortSwitchFlag

    .section .text

/**
 * This function makes the RTOS aware about a ISR entering, it takes the
 * current task stack saved, places into the TCB, loads the ISR stack
 * the interrupted stack must be passed in a0. It needs to receive the
 * ISR nesting code improvements
 */

    .global rtos_int_enter
    .type rtos_int_enter, @function
rtos_int_enter:
    /* preserve the return address */
    mv t1, ra
    mv t2, a0

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

    /* Save current TCB and load the ISR stack */
    lw  t0, pxCurrentTCB
    sw 	t2, 0x0(t0)
    lw  sp, xIsrStackTop

rtos_enter_end:
    mv  ra, t1
    ret

/**
 * Recovers the next task to run stack pointer and place it into
 * a0, then the interrupt handler can restore the context of
 * the next task
 */
    .global rtos_int_exit
    .type rtos_int_exit, @function
rtos_int_exit:
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
    call vTaskSwitchContext
    lw  ra, 0(sp)
    addi sp, sp, 16

    /* Clears the switch pending flag */
    la t0, xPortSwitchFlag
    mv t2, zero
    sw  t2, 0x0(t0)

no_switch:
    /* Recover the stack of next task and prepare to exit : */
    lw a0, pxCurrentTCB
    lw a0, 0x0(a0)

rtos_exit_end:
    ret
