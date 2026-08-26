/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * LP-side firmware: PMP store-to-text violation test.
 * Attempts to write to an address in the .text (RX-only) region.
 * With PMP enabled, this triggers a store access fault (mcause=7).
 */

#include <stdint.h>
#include "ulp_lp_core_print.h"

/* Must survive optimization: noinline keeps it as a real symbol, used prevents DCE */
static void __attribute__((noinline, used)) dummy_func(void)
{
    asm volatile("" ::: "memory");
}

int main(void)
{
    lp_core_print_str("PMP write-to-text test: start\n");

    /* Obtain a pointer into .text and write through it — PMP violation: store to RX region */
    void (*fn)(void) = dummy_func;
    volatile uint32_t *text_addr = (volatile uint32_t *)(uintptr_t)fn;
    *text_addr = 0xDEADBEEF;

    /* Should never reach here — PMP fault expected above */
    while (1) {}
}
