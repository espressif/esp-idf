/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ulp_lp_core_utils.h"

void do_crash(void)
{
    volatile int *p = (int *)0x0;
    // if ubsan is enabled (-fsanitize=undefined) line below will cause ubsan check failure
    // and finally app will be stopped in abort()
    *p = 32;
    // if ubsan is disabled app will be stopped in abort() call below
    abort();
}

void do_things(int max)
{
    while (1) {
        for (int i = 0; i < max; i++) {
            ulp_lp_core_delay_us(100000);
            if (i > 0)
                do_crash();
        }
    }
}

int main (void)
{
    do_things(1000000000);
    return 0;
}
