/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

	.bss
	.global fsm_counter
fsm_counter:
	.long 0

	.text
	.global entry
entry:
	move r1, fsm_counter
	ld r0, r1, 0
	add r0, r0, 1
	st r0, r1, 0
	jumpr wake_cpu, 5, ge
	halt

wake_cpu:
	wake
	halt
