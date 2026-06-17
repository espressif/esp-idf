/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ulp_lp_core_utils.h"

int main(void)
{
    // Wait for 1 second to ensure the HP Core enters deep sleep
    ulp_lp_core_delay_us(1000000);

    // Trigger an exception to wake up the HP Core
    asm volatile("unimp");
}
