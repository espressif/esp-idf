/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "ulp_math.h"

/* Place in .ulp_math_fast section via the ldgen fragment (ulp_math.lf) */
__attribute__((section(".ulp_math_fast")))
int ulp_math_multiply(int a, int b)
{
    return a * b;
}
