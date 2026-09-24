/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "sdkconfig.h"

void esp_crypto_common_clk_enable(bool enable);

static inline void esp_crypto_clk_init(void)
{
#if !CONFIG_ESP_CRYPTO_CLK_ON_DEMAND
    /* Keep crypto clocks always on for better crypto performance. */
    esp_crypto_common_clk_enable(true);
#endif
}
