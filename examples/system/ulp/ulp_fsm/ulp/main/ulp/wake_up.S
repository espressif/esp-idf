/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* ULP assembly files are passed through C preprocessor first, so include directives
   and C macros may be used in these files
 */
#include "soc/rtc_cntl_reg.h"
#include "soc/soc_ulp.h"
#include "sdkconfig.h"

	.global wake_up
wake_up:
	/* Check if the system is in sleep mode */
#if CONFIG_IDF_TARGET_ESP32
	READ_RTC_REG(RTC_CNTL_LOW_POWER_ST_REG, 27, 1)
#else
	READ_RTC_FIELD(RTC_CNTL_LOW_POWER_ST_REG, RTC_CNTL_MAIN_STATE_IN_IDLE)
#endif
	move r1, r0
	/* Check if the system can be woken up */
	READ_RTC_FIELD(RTC_CNTL_LOW_POWER_ST_REG, RTC_CNTL_RDY_FOR_WAKEUP)
	/* If the system is in normal mode or if the system is in sleep mode with ready for wakeup set, we can signal the main CPU to wakeup */
	or r0, r0, r1
	jump wake_up, eq

	/* Wake up the SoC, end program */
	wake
	halt
