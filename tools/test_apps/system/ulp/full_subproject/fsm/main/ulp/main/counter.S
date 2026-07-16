/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ULP Example: timer counter

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.

   This file contains assembly code which runs on the ULP. On each timer wakeup,
   the program increments a counter in RTC_SLOW_MEM. Once the counter reaches
   the value set by the main program, the ULP wakes the main CPU from deep sleep.
*/

/* ULP assembly files are passed through C preprocessor first, so include directives
   and C macros may be used in these files
 */
#include "sdkconfig.h"

#ifndef ULP_BUILD_SYSTEM_FSM_EXAMPLE
#error "ULP FSM compile definitions were not passed to the assembly preprocessor"
#endif

	/* Define variables, which go into .bss section (zero-initialized data) */
	.bss
	.global iteration_count
iteration_count:
	.long 0

	.global wakeup_iteration
wakeup_iteration:
	.long 0

	/* Ldgen-only marker section used by ulp_fsm_sections.lf. Keep it
	   non-allocated so it never becomes part of the ULP FSM load image. */
	.section .ulp_fsm_extra_data, ""
ldgen_section_marker:
	.long 0x12345678

	/* Code goes into .text section */
	.text
	.global entry
entry:
	/* Increment iteration_count. */
	move r3, iteration_count
	ld r2, r3, 0
	add r2, r2, 1
	st r2, r3, 0

	/* Compare iteration_count to wakeup_iteration. */
	move r3, wakeup_iteration
	ld r3, r3, 0
	sub r3, r3, r2
	jump wake_up, eq
	halt
