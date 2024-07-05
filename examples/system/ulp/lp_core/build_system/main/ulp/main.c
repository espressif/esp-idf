/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "lib_src.h"

int sum;

int main (void)
{
    sum = lib_test_func_sum(5, 6);
    /* ulp_lp_core_halt() is called automatically when main exits */
    return 0;
}
