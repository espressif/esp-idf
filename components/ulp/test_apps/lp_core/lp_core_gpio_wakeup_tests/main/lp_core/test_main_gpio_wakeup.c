/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <stdbool.h>
#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_gpio.h"
#include "hal/lp_core_ll.h"

uint32_t lp_wakeup_cause;

int main(void)
{
    lp_wakeup_cause = ulp_lp_core_get_wakeup_cause();

    ulp_lp_core_wakeup_main_processor();

    ulp_lp_core_gpio_clear_intr_status();

    return 0;
}
