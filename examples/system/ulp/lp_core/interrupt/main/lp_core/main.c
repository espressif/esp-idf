/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_interrupts.h"

uint32_t lp_core_pmu_intr_count = 0;

/* Add LP_CORE_ISR_ATTR to ensure registers are saved and restored */
void LP_CORE_ISR_ATTR ulp_lp_core_lp_pmu_intr_handler(void)
{
    ulp_lp_core_sw_intr_clear();
    lp_core_pmu_intr_count++;
}

int main (void)
{
    ulp_lp_core_intr_enable();
    ulp_lp_core_sw_intr_enable(true);

    while(1) {
        /* Wait forever, handling interrupts */
        asm volatile("wfi");
    }
    return 0;
}
