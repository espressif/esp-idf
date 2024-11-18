/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdint.h>
#include <stdbool.h>
#include "ulp_lp_core.h"
#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_gpio.h"


int main (void)
{
    ulp_lp_core_wakeup_main_processor();

    /* Wakeup interrupt is a level interrupt, wait 1 sec to
        allow user to release button to avoid waking up the ULP multiple times */
    ulp_lp_core_delay_us(1000*1000);
    ulp_lp_core_gpio_clear_intr_status();

    return 0;
}
