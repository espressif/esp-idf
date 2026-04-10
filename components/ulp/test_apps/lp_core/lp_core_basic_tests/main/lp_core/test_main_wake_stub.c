/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "ulp_lp_core_utils.h"

/* Incremented on every LP timer wakeup, readable by HP core after test */
volatile uint32_t lp_wake_count = 0;

int main(void)
{
    lp_wake_count++;
    ulp_lp_core_wakeup_main_processor();
    return 0;
}
