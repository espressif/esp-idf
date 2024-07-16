/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_lp_timer_shared.h"

volatile uint32_t set_timer_wakeup_counter;
volatile uint32_t WAKEUP_PERIOD_BASE_US = 100000;

int main(void)
{
    set_timer_wakeup_counter++;

    /* Alternate between WAKEUP_PERIOD_BASE_US and 2*WAKEUP_PERIOD_BASE_US to let the main CPU see that
       the wake-up time can be reconfigured */
    ulp_lp_core_lp_timer_set_wakeup_time(((set_timer_wakeup_counter % 2) + 1)*WAKEUP_PERIOD_BASE_US);

    return 0;
}
