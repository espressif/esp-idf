/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_gpio.h"

volatile uint32_t gpio_wkup_cause;

int main(void)
{
    gpio_wkup_cause = ulp_lp_core_get_wakeup_cause();
    ulp_lp_core_wakeup_main_processor();
    ulp_lp_core_gpio_clear_intr_status();
    /* Return, as the LP IO wakeup test does. Halt-and-clear from here drops
     * the HP trigger on P4 before the PMU latches it. */
    return 0;
}
