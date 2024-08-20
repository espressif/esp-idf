/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc.h"
#include "soc/pcr_reg.h"

#pragma once

static inline void esp_crypto_clk_init(void)
{
    // Set crypto clock (`clk_sec`) to use 96M PLL clock
    REG_SET_FIELD(PCR_SEC_CONF_REG, PCR_SEC_CLK_SEL, 0x3);
}
