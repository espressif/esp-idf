// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "soc/soc.h"
#include "soc/interrupt_reg.h"


	.equ SAVE_REGS, 32
	.equ CONTEXT_SIZE, (SAVE_REGS * 4)
	.equ PANIC_REGS, 38
	.equ PANIC_REGS_SIZE, (PANIC_REGS * 4)

.altmacro

.macro lwsp a, b
	lw \a, ((\b)*4)(sp)
.endm

.macro swsp a, b
	sw \a, ((\b)*4)(sp)
.endm


.macro save_regs
	addi    sp, sp, -CONTEXT_SIZE
	swsp 	ra, 1
	swsp 	a0, 2
	swsp 	a1, 3
	swsp 	a2, 4
	swsp 	a3, 5
	swsp 	a4, 6
	swsp 	a5, 7
	swsp 	a6, 8
	swsp 	a7, 9
	swsp 	t0, 10
	swsp 	t1, 11
	swsp 	t2, 12
	swsp 	t3, 13
	swsp 	t4, 14
	swsp 	t5, 15
	swsp 	t6, 16
	//swsp 	sp, 17
	//swsp 	gp, 18
	swsp 	tp, 19
	swsp 	s0, 20
	swsp 	s1, 21
	swsp 	s2, 22
	swsp 	s3, 23
	swsp 	s4, 24
	swsp 	s5, 25
	swsp 	s6, 26
	swsp 	s7, 27
	swsp 	s8, 28
	swsp 	s9, 29
	swsp 	s10, 30
	swsp 	s11, 31
.endm

.macro save_mepc
	csrr	t0, mepc
	swsp	t0, 0
.endm

.macro restore_regs
	lwsp 	ra, 1
	lwsp 	a0, 2
	lwsp 	a1, 3
	lwsp 	a2, 4
	lwsp 	a3, 5
	lwsp 	a4, 6
	lwsp 	a5, 7
	lwsp 	a6, 8
	lwsp 	a7, 9
	lwsp 	t0, 10
	lwsp 	t1, 11
	lwsp 	t2, 12
	lwsp 	t3, 13
	lwsp 	t4, 14
	lwsp 	t5, 15
	lwsp 	t6, 16
	//lwsp 	sp, 17
	//lwsp 	gp, 18
	lwsp 	tp, 19
	lwsp 	s0, 20
	lwsp 	s1, 21
	lwsp 	s2, 22
	lwsp 	s3, 23
	lwsp 	s4, 24
	lwsp 	s5, 25
	lwsp 	s6, 26
	lwsp 	s7, 27
	lwsp 	s8, 28
	lwsp 	s9, 29
	lwsp 	s10, 30
	lwsp 	s11, 31
	addi sp, sp, CONTEXT_SIZE
.endm

.macro restore_mepc
	lwsp	t0, 0
	csrw	mepc, t0
.endm

	.global vPortYieldFromISR
	.global uxInterruptNesting
	.global uxSchedulerRunning
	.global xIsrStackTop
	.global pxCurrentTCB
	.global _global_interrupt_handler

	.section .exception_vectors.text
	/* This is the vector table. MTVEC points here.
	 *
	 * Use 4-byte intructions here. 1 instruction = 1 entry of the table.
	 * The CPU jumps to MTVEC (i.e. the first entry) in case of an exception,
	 * and (MTVEC & 0xfffffffc) + (mcause & 0x7fffffff) * 4, in case of an interrupt.
	 *
	 * Note: for our CPU, we need to place this on a 256-byte boundary, as CPU
	 * only uses the 24 MSBs of the MTVEC, i.e. (MTVEC & 0xffffff00).
	 */

	.balign 0x100
	.global _vector_table
	.type _vector_table, @function
_vector_table:
	.option push
	.option norvc
	j _panic_handler		/* exception handler, entry 0 */
	.rept 31
	j _interrupt_handler		/* 31 identical entries, all pointing to the interrupt handler */
	.endr
	.option pop
	.size _vector_table, .-_vector_table

	/* Exception handler.*/
	.global panicHandler
	.type _panic_handler, @function
_panic_handler:
	addi sp, sp, -PANIC_REGS_SIZE
	swsp x0, 0
	swsp x1, 1
	/* x2 is sp - it will be placed on stack later (after we can use the other registers for computation) */
	swsp x3, 3
	swsp x4, 4
	swsp x5, 5
	swsp x6, 6
	swsp x7, 7
	swsp x8, 8
	swsp x9, 9
	swsp x10, 10
	swsp x11, 11
	swsp x12, 12
	swsp x13, 13
	swsp x14, 14
	swsp x15, 15
	swsp x16, 16
	swsp x17, 17
	swsp x18, 18
	swsp x19, 19
	swsp x20, 20
	swsp x21, 21
	swsp x22, 22
	swsp x23, 23
	swsp x24, 24
	swsp x25, 25
	swsp x26, 26
	swsp x27, 27
	swsp x28, 28
	swsp x29, 29
	swsp x30, 30
	swsp x31, 31
	/* "Undo" the modification already done to the sp (x2) by the panic handler */
	addi a1, x2, PANIC_REGS_SIZE
	swsp a1, 2
	csrr a1, mcause
	swsp a1, 32
	csrr a1, mepc
	swsp a1, 33
	csrr a1, mhartid
	swsp a1, 34
	csrr a1, mstatus
	swsp a1, 35
	csrr a1, mtval
	swsp a1, 36
	csrr a1, mtvec
	swsp a1, 37
	mv a0, sp
	csrr a1, mcause
	jal zero, panicHandler
    /*  panicHandler never returns */
	.size  _panic_handler, .-_panic_handler

	/* This is the interrupt handler.
	 * It saves the registers on the stack,
	 * prepares for interrupt nesting,
	 * re-enables the interrupts,
	 * then jumps to the C dispatcher in interrupt.c.
	 */
	.global _interrupt_handler
	.type _interrupt_handler, @function
_interrupt_handler:
	/* entry */
	save_regs
	save_mepc

	/* scheduler not enabled, jump directly to ISR handler */
	lw t0, uxSchedulerRunning
	beq t0,zero,already_on_handler

	/* increments the ISR nesting count */
	/* This will work properly when we have nested interrupts */
	la t0, uxInterruptNesting
	lw t1, 0x0(t0)
	addi t2,t1,1
	sw  t2, 0x0(t0)

	/* If reached here from another low-prio ISR, skip stack pushing to TCB */
	bne t1,zero, already_on_handler

	/* Otherwise, save current sp, and use the isr stack from here */
	lw  t0, pxCurrentTCB
	sw 	sp, 0x0(t0)
	lw  sp, xIsrStackTop

already_on_handler:
	/* Before dispatch c handler, restore interrupt to enable nested intr */
	csrr s1, mcause
	csrr s2, mstatus

	/* Save the interrupt threshold level */
	la t0, INTERRUPT_CORE0_CPU_INT_THRESH_REG
	lw s3, 0(t0)

	/* Increase interrupt threshold level */
	la t2, 0x7fffffff
	and t1, s1, t2		/* t1 = mcause & mask */
	slli t1, t1, 2 		/* t1 = mcause * 4 */
	la t2, INTC_INT_PRIO_REG(0)
	add t1, t2, t1		/* t1 = INTC_INT_PRIO_REG + 4 * mcause */
	lw t2, 0(t1)		/* t2 = INTC_INT_PRIO_REG[mcause] */
	addi t2, t2, 1		/* t2 = t2 +1 */
	sw t2, 0(t0)		/* INTERRUPT_CORE0_CPU_INT_THRESH_REG = t2 */
	fence

	la t0, 0x8
	csrrs t0, mstatus, t0

	/* call the C dispatcher */
	mv      a0, sp		/* argument 1, stack pointer */
	csrr    a1, mcause	/* argument 2, interrupt number */
	/* mask off the interrupt flag of mcause */
	la	    t0, 0x7fffffff
	and     a1, a1, t0
	jal     _global_interrupt_handler

	/* After dispatch c handler, disable interrupt to make freertos make context switch */

	la t0, 0x8
	csrrc t0, mstatus, t0

	/* restore the interrupt threshold level */
	la t0, INTERRUPT_CORE0_CPU_INT_THRESH_REG
	sw s3, 0(t0)
	fence

	/* may skip RTOS aware interrupt since scheduler was not started */
	lw t1, uxSchedulerRunning
	beq t1,zero, isr_exit

	/* update nesting interrupts counter */
	la t0, uxInterruptNesting
	lw t1, 0x0(t0)

	/* Already zero, protect againts underflow */
	beq t1, zero, isr_skip_decrement
	addi t1,t1, -1
	sw  t1, 0x0(t0)

isr_skip_decrement:
	/* may still have interrupts pending, skip section below and exit */
	bne t1,zero,isr_exit

	/* handlered all the ISRs and scheduled the next task, take its stack */
	/* load on sp, then exit. */
	lw sp, pxCurrentTCB
	lw sp, 0x0(sp)

isr_exit:
	/* restore the rest of the registers */
	csrw mcause, s1
	csrw mstatus, s2
	restore_mepc
	restore_regs

	/* exit, this will also re-enable the interrupts */
	mret
	.size  _interrupt_handler, .-_interrupt_handler
