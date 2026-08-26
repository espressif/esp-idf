/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/* ULP FSM assembly program to increment counter */

#include "soc/rtc_cntl_reg.h"
#include "soc/soc_ulp.h"

	/* Define variables */
	.bss
	.global fsm_counter
fsm_counter:
	.long 0

	/* Code section */
	.text
	.global entry
entry:
	/* Load counter address */
	move r1, fsm_counter

	/* Load current counter value */
	ld r0, r1, 0

	/* Check if counter >= 100 */
	jumpr done, 100, ge

	/* Increment counter */
	add r0, r0, 1

	/* Store incremented value */
	st r0, r1, 0

	/* Halt until next wakeup */
	halt

done:
	/* Counter reached 100, wake up the main processor */
	wake
	/* Halt - task complete */
	halt
