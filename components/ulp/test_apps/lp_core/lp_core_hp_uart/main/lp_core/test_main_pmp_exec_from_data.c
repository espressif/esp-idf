/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * LP-side firmware: PMP execute-from-data violation test.
 * Copies a RISC-V ret instruction (0x00008067) into a .data buffer,
 * then jumps to it. With PMP enabled, this triggers an instruction access
 * fault (mcause=1) since .data is RW-only.
 */

#include <stdint.h>
#include <string.h>
#include "ulp_lp_core_print.h"

typedef void (*func_t)(void);

/* Buffer in .data to hold the injected instruction */
static uint32_t exec_buf[4];

int main(void)
{
    lp_core_print_str("PMP exec-from-data test: start\n");

    /* RISC-V compressed ret: c.jr ra = 0x8082 (16-bit), or full ret = 0x00008067 */
    exec_buf[0] = 0x00008067U; /* jalr x0, 0(ra) — standard ret */

    /* Jump into the data buffer — PMP violation: execute from RW region */
    func_t f = (func_t)(uintptr_t)exec_buf;
    f();

    /* Should never reach here */
    while (1) {}
}
