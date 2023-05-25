/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "ulp_lp_core_utils.h"

volatile uint32_t counter;
volatile uint32_t counter_wakeup_limit;

int main (void)
{
    counter++;

    if (counter_wakeup_limit && (counter > counter_wakeup_limit)) {
        counter = 0;

        ulp_lp_core_wakeup_main_processor();
    }

    return 0;
}
