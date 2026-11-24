/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "i2s_private.h"

bool i2s_clk_src_requires_apb_lock(i2s_clock_src_t clk_src)
{
    (void)clk_src;
    return false;
}

bool i2s_clk_src_requires_cpu_lock(i2s_clock_src_t clk_src)
{
    (void)clk_src;
    /* Workaround for IDF-16798: keep the CPU frequency fixed while I2S is active.
     * DFS-triggered MSPI timing switches are not safe during I2S operation. */
    return true;
}
