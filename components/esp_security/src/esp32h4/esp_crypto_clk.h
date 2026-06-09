/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc.h"
#include "soc/pcr_reg.h"
#include "esp_private/esp_clk_tree_common.h"

#pragma once

#include <stdbool.h>

void esp_crypto_common_clk_enable(bool enable);

static inline void esp_crypto_clk_init(void)
{
    // Set crypto clock (`clk_sec`) to use 96M PLL clock
    esp_clk_tree_enable_src(SOC_MOD_CLK_PLL_F96M, true);
    REG_SET_FIELD(PCR_SEC_CONF_REG, PCR_SEC_CLK_SEL, 0x3);
}
