/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_io_reg.h"
#include "soc/soc_ulp.h"

	.bss

	.global jumps_pass
jumps_pass:
	.long 0

	.global jumps_fail
jumps_fail:
	.long 0

	.text
	.global test_jumps
test_jumps:

	/* tests for LT (less than) condition */
	stage_rst           /* cnt = 0 */
	jumps test_fail, 0, LT /* 0 < 0: false, should not jump */
	jumps 1f, 1, LT     /* 0 < 1: true, should jump */
	jump test_fail
1:
	stage_inc 2         /* cnt = 2 */
	jumps 1f, 3, LT     /* 2 < 1: true */
	jump test_fail
1:
	jumps test_fail, 1, LT  /* 2 < 1: false */
	jumps test_fail, 2, LT  /* 2 < 2: false */

	/* tests for LE (less or equal) condition */
	stage_rst           /* cnt = 0 */
	jumps 1f, 0, LE     /* 0 <= 0: true */
	jump test_fail
1:
	jumps 1f, 1, LE     /* 0 <= 1: true */
	jump test_fail
1:
	stage_inc 2         /* cnt = 2 */
	jumps test_fail, 1, LE  /* 2 <= 1: false */

	/* tests for EQ (equal) condition */
	stage_rst           /* cnt = 0 */
	jumps 1f, 0, EQ     /* 0 = 0: true */
	jump test_fail
1:
	jumps test_fail, 1, EQ  /* 0 = 1: false */

	stage_inc 1         /* cnt = 1 */
	jumps test_fail, 0, EQ  /* 1 = 0: false */
	jumps test_fail, 2, EQ  /* 1 = 2: false */
	jumps 1f, 1, EQ         /* 1 = 1: true */
1:

	/* tests for GE (greater or equal) condition */
	stage_rst           /* cnt = 0 */
	jumps 1f, 0, GE     /* 0 >= 0: true */
	jump test_fail
1:
	jumps test_fail, 1, GE  /* 0 >= 1: false */

	stage_inc 1         /* cnt = 1 */
	jumps 1f, 0, GE     /* 1 >= 0: true */
	jump test_fail
1:
	jumps 1f, 1, GE     /* 1 >= 1: true */
	jump test_fail
1:
	jumps test_fail, 2, GE  /* 1 >= 2: false */

	/* tests for GT (greater than) condition */
	stage_rst           /* cnt = 0 */
	jumps test_fail, 0, GT  /* 0 > 0: false */
	jumps test_fail, 1, GE  /* 0 > 1: false */

	stage_inc 1         /* cnt = 1 */
	jumps 1f, 0, GT     /* 1 > 0: true */
	jump test_fail
1:
	jumps test_fail, 1, GT  /* 1 > 1: false */
	jumps test_fail, 2, GT  /* 1 > 2: false */

	jump test_pass

test_fail:
	move r0, jumps_fail
	move r1, 1
	st r1, r0, 0
	jump done

test_pass:
	move r0, jumps_pass
	move r1, 1
	st r1, r0, 0
	jump done

	.global done
done:
	wake
	halt
