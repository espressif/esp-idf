/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

	.section	.init.vector,"ax"
	/* This is the vector table. It is currently empty, but will be populated
	 * with exception and interrupt handlers when this is supported
	*/

	.align	0x4, 0xff
	.global _vector_table
	.type _vector_table, @function
_vector_table:
	.option push
	.option norvc

	.rept 32
	nop
	.endr

	.option pop
	.size _vector_table, .-_vector_table
