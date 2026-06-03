/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "ulp_lp_core_utils.h"
#include "hal/lp_core_ll.h"

int main(void)
{
    // Wait for 1 second to ensure the HP Core enters deep sleep
    ulp_lp_core_delay_us(1000000);

    ulp_lp_core_wakeup_main_processor();
    /* Disable wake-up source and put lp core to sleep */
    lp_core_ll_set_wakeup_source(0);
    ulp_lp_core_halt();
}
