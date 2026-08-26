/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ulp_lp_core_print.h"
#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_critical_section_shared.h"

volatile uint32_t shared_cnt = 0;
ulp_lp_core_spinlock_t lp_spinlock;

int main (void)
{
    /* Initialize the inter-processor spinlock. This must be done on either of HP core and LP core */
    ulp_lp_core_spinlock_init(&lp_spinlock);

    /* Delay 10ms in case of interleaved console output */
    ulp_lp_core_delay_us(10000);

    lp_core_printf("LP CPU's increment starts, shared counter = %d\r\n", shared_cnt);

    for (int i = 0; i < 100000; i++) {
        ulp_lp_core_enter_critical(&lp_spinlock);
        shared_cnt++;
        ulp_lp_core_exit_critical(&lp_spinlock);
    }

    lp_core_printf("LP CPU's increment ends, shared counter = %d\r\n", shared_cnt);
    return 0;
}
