/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "hal/sec_ll.h"
#include "soc/clk_tree_defs.h"

void esp_crypto_common_clk_enable(bool enable);

static inline void esp_crypto_clk_init(void)
{
    // Set crypto clock (`clk_sec`) to use 480M SPLL clock
    sec_ll_crypto_clk_src_sel(SOC_MOD_CLK_SPLL);
}
