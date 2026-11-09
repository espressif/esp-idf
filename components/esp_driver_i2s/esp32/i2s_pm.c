/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "i2s_private.h"

bool i2s_clk_src_requires_apb_lock(i2s_clock_src_t clk_src)
{
    /* PLL_F160M is divided from the BBPLL shared with the CPU and stops when
     * the CPU clock switches to XTAL. Testing shows that APLL has the same
     * limitation, so neither source can be used while APB frequency scaling
     * is allowed.
     */
    return clk_src == I2S_CLK_SRC_PLL_160M || clk_src == I2S_CLK_SRC_APLL;
}
