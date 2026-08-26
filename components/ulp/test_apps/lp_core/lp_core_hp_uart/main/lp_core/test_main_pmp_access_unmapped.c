/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * LP-side firmware: PMP denied-region access violation test.
 * Reads from an HP peripheral address not covered by any permitted PMP entry.
 * With PMP deny-by-default (catch-all NONE entry), this triggers a
 * load access fault (mcause=5).
 */

#include <stdint.h>
#include "ulp_lp_core_print.h"
#include "soc/reg_base.h"

int main(void)
{
    lp_core_print_str("PMP access-unmapped test: start\n");

    /* Access HP MMIO outside allow-listed regions — should trigger load access fault */
    volatile uint32_t *unmapped = (volatile uint32_t *)DR_REG_GPIO_BASE;
    __attribute__((unused)) uint32_t val = *unmapped;

    /* Should never reach here */
    while (1) {}
}
