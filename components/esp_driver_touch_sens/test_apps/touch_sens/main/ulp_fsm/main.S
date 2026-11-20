/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/rtc_io_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/soc_ulp.h"
#include "test_helper.h"

#define WRITE_TOUCH_PAD_FIELD_2(chan, value) WRITE_RTC_FIELD(RTC_IO_TOUCH_PAD##chan##_REG, RTC_IO_TOUCH_PAD##chan##_DAC, value)
#define WRITE_TOUCH_PAD_FIELD(chan, value) WRITE_TOUCH_PAD_FIELD_2(chan, value)

#define ULP_FSM_CLK_CYCLES_PER_US 8
#define TOUCH_SIMULATE_HOLD_CYCLES (TEST_TOUCH_SIMULATE_HOLD_TIME_US * ULP_FSM_CLK_CYCLES_PER_US)

    .bss
run_count:
    .long 0

    .text
    .global entry
entry:
    move r1, run_count
    ld r0, r1, 0
    add r0, r0, 0
    jump first_run, eq

    /* Simulate touch event */
    WRITE_TOUCH_PAD_FIELD(TEST_TOUCH_WAKEUP_CHANNEL, 4)

    .rept TOUCH_SIMULATE_HOLD_CYCLES / 0xffff
        wait 0xffff
    .endr

    .if TOUCH_SIMULATE_HOLD_CYCLES % 0xffff
        wait TOUCH_SIMULATE_HOLD_CYCLES % 0xffff
    .endif

    WRITE_TOUCH_PAD_FIELD(TEST_TOUCH_WAKEUP_CHANNEL, 7)

    /* Disable the ULP timer */
    WRITE_RTC_FIELD(RTC_CNTL_STATE0_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN, 0)
    halt

first_run:
    add r0, r0, 1
    st r0, r1, 0
    halt
