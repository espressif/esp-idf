/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

	.section	.init.vector,"ax"

	.global _vector_table
	.type _vector_table, @function
_vector_table:
	.option push
	.option norvc

	.rept 30
	j _panic_handler
	.endr
	j _interrupt_handler // All interrupts are routed to mtvec + 4*30, i.e. the 31st entry
	j _panic_handler

	.option pop
	.size _vector_table, .-_vector_table
