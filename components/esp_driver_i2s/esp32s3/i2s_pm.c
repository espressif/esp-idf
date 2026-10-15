/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "i2s_private.h"

bool i2s_clk_src_requires_apb_lock(i2s_clock_src_t clk_src)
{
    /* PLL_F160M and PLL_D2 are divided from the BBPLL shared with the CPU.
     * On ESP32-S3 they stop when the CPU clock switches to XTAL, so PLL_160M
     * and PLL_240M cannot be used while APB frequency scaling is allowed.
     * XTAL is unaffected.
     */
    return clk_src == I2S_CLK_SRC_PLL_240M || clk_src == I2S_CLK_SRC_PLL_160M;
}
