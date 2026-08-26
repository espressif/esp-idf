/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * LP-side firmware: positive PMP test.
 * Verifies that normal operations (data R/W, function call, peripheral register read)
 * succeed when PMP is enabled. Sets shared variable to signal success to HP-side.
 */

#include <stdint.h>
#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_print.h"
#include "soc/lp_system_reg.h"

volatile uint32_t pmp_positive_result = 0;

/* Small data buffer in .data to test read/write */
static volatile uint32_t data_buf[4] = {0xDEAD, 0xBEEF, 0xCAFE, 0xBABE};

static uint32_t sum_buffer(void)
{
    uint32_t s = 0;
    for (int i = 0; i < 4; i++) {
        s += data_buf[i];
    }
    return s;
}

int main(void)
{
    lp_core_print_str("PMP positive test: start\n");

    /* Read/write data in .data section */
    data_buf[0] = 0x1;
    data_buf[1] = 0x2;
    data_buf[2] = 0x3;
    data_buf[3] = 0x4;

    uint32_t s = sum_buffer();
    if (s != 10) {
        return 1;
    }

    /* Read an LP-system peripheral register listed in LP memory map headers */
    volatile uint32_t *lp_scratch =
        (volatile uint32_t *)LP_SYSTEM_REG_LP_STORE0_REG;
    __attribute__((unused)) uint32_t scratch = *lp_scratch;

    /* Signal success to HP core via shared variable */
    pmp_positive_result = 0xA5A5A5A5;

    return 0;
}
