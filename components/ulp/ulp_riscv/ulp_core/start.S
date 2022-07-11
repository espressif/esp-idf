/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
	.section .text.vectors
	.global irq_vector
	.global reset_vector

/* The reset vector, jumps to startup code */
reset_vector:
	j __start

/* Interrupt handler */
.balign 16
irq_vector:
	ret

	.section .text

__start:
	/* setup the stack pointer */
	la sp, __stack_top
	call ulp_riscv_rescue_from_monitor
	call main
	call ulp_riscv_halt
loop:
	j loop
