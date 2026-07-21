/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "sdkconfig.h"
#include "hal/sec_ll.h"
#include "soc/clk_tree_defs.h"

void esp_crypto_common_clk_enable(bool enable);

static inline void esp_crypto_clk_init(void)
{
    // Set crypto clock (`clk_sec`) to use 240M PLL clock
    sec_ll_crypto_clk_src_sel(SOC_MOD_CLK_PLL_F240M);
#if !CONFIG_ESP_CRYPTO_CLK_ON_DEMAND
    /* Keep crypto clocks always on for better crypto performance. */
    esp_crypto_common_clk_enable(true);
#endif
}
