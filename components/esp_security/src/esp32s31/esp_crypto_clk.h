/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "soc/soc.h"
#include "soc/hp_sys_clkrst_reg.h"

#pragma once

static inline void esp_crypto_clk_init(void)
{
    // Set crypto clock (`clk_sec`) to use 240M PLL clock
    // TODO: ["ESP32S31"] IDF-14629
    // REG_SET_FIELD(HP_SYS_CLKRST_CRYPTO_CTRL0_REG, HP_SYS_CLKRST_REG_CRYPTO_CLK_SRC_SEL, 0x2);
}
