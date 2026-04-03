/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "soc/soc.h"
#include "soc/hp_sys_clkrst_reg.h"
#include "esp_private/esp_clk_tree_common.h"
#pragma once

static inline void esp_crypto_clk_init(void)
{
    // Set crypto clock (`clk_sec`) to use 240M PLL clock
    esp_clk_tree_enable_src(SOC_MOD_CLK_PLL_F240M, true);
    REG_SET_FIELD(HP_SYS_CLKRST_PERI_CLK_CTRL25_REG, HP_SYS_CLKRST_REG_CRYPTO_CLK_SRC_SEL, 0x2);
}
