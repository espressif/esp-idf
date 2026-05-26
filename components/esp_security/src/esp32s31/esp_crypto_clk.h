/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "soc/soc.h"
#include "soc/hp_sys_clkrst_reg.h"
#include "soc/hp_sys_clkrst_struct.h"

static inline void esp_crypto_clk_init(void)
{
    // Set crypto clock (`clk_sec`) to use 240M PLL clock
    REG_SET_FIELD(HP_SYS_CLKRST_CRYPTO_CTRL0_REG, HP_SYS_CLKRST_REG_CRYPTO_CLK_SRC_SEL, 0x2);
}

void esp_crypto_common_clk_enable(bool enable);
