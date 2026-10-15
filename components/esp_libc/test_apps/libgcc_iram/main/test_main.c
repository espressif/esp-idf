/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdint.h>

static volatile uint64_t s_dividend = 0x100000000ULL;
static volatile uint64_t s_divisor = 3;
static volatile uint64_t s_quotient;

static volatile double s_lhs = 1.25;
static volatile double s_rhs = 2.5;
static volatile double s_sum;

void app_main(void)
{
    /* Keep a 64-bit divide and a software double add in the link. The build
     * checks where the helpers for these calls were placed. */
    s_quotient = s_dividend / s_divisor;
    s_sum = s_lhs + s_rhs;
}
