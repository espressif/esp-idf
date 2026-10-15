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
