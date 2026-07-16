/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "ulp_sum.h"
#include "ulp_math.h"

int sum;
int product;

int main(void)
{
    /* ulp_sum_func internally uses ulp_clamp() from the ulp_clamp
     * managed component (resolved via ulp_sum/idf_component.yml). */
    sum = ulp_sum_func(5, 6);
    product = ulp_math_multiply(3, 7);

    /* ulp_lp_core_halt() is called automatically when main exits */
    return 0;
}
