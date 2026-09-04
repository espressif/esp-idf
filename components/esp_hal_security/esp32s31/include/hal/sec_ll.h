/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/clk_tree_defs.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "hal/assert.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Select crypto clock source.
 *
 * @param src Clock source (SOC_MOD_CLK_XTAL, SOC_MOD_CLK_RC_FAST, or SOC_MOD_CLK_PLL_F240M)
 */
static inline __attribute__((always_inline)) void sec_ll_crypto_clk_src_sel(soc_module_clk_t src)
{
    uint32_t reg_val = 3;
    if (src == SOC_MOD_CLK_XTAL) {
        reg_val = 0;
    } else if (src == SOC_MOD_CLK_RC_FAST) {
        reg_val = 1;
    } else if (src == SOC_MOD_CLK_PLL_F240M) {
        reg_val = 2;
    } else {
        HAL_ASSERT(false);
    }
    HP_SYS_CLKRST.crypto_ctrl0.reg_crypto_clk_src_sel = reg_val;
}

#ifdef __cplusplus
}
#endif
