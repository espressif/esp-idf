/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdint.h>
#include "ulp_lp_core.h"
#include "ulp_lp_core_utils.h"

/* this variable will be exported as a public symbol, visible from main CPU: */
volatile uint32_t run_counter = 0;

int main (void)
{
    run_counter++;

    /* ulp_lp_core_halt() is called automatically when main exits */
    return 0;
}
