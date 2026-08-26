/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/clk_tree_defs.h"
#include "soc/pcr_struct.h"
#include "hal/assert.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Select crypto clock source.
 *
 * @param src Clock source (SOC_MOD_CLK_XTAL, SOC_MOD_CLK_RC_FAST, SOC_MOD_CLK_XTAL_X2_F64M, or SOC_MOD_CLK_PLL_F96M)
 */
static inline __attribute__((always_inline)) void sec_ll_crypto_clk_src_sel(soc_module_clk_t src)
{
    uint32_t reg_val = 3;
    if (src == SOC_MOD_CLK_XTAL) {
        reg_val = 0;
    } else if (src == SOC_MOD_CLK_RC_FAST) {
        reg_val = 1;
    } else if (src == SOC_MOD_CLK_XTAL_X2_F64M) {
        reg_val = 2;
    } else if (src == SOC_MOD_CLK_PLL_F96M) {
        reg_val = 3;
    } else {
        HAL_ASSERT(false);
    }
    PCR.sec_conf.sec_clk_sel = reg_val;
}

#ifdef __cplusplus
}
#endif
