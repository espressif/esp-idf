/*
 * SPDX-FileCopyrightText: 2010-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_assert.h"
#include "soc/soc_caps.h"
#include "soc/io_mux_reg.h"
#include "soc/gpio_sig_map.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum clock_out_channel {
    CLKOUT_CHANNEL_1,
    CLKOUT_CHANNEL_2,
    CLKOUT_CHANNEL_MAX,
} clock_out_channel_t;

#define CLKOUT_CHANNEL_TO_GPIO_SIG_ID(channel)  ((channel == CLKOUT_CHANNEL_1) ? DBG_CH0_CLK_IDX : \
                                                 (channel == CLKOUT_CHANNEL_2) ? DBG_CH1_CLK_IDX : SIG_GPIO_OUT_IDX)

ESP_STATIC_ASSERT(CLKOUT_CHANNEL_MAX == SOC_GPIO_CLOCKOUT_CHANNEL_NUM, "clock_out_channel enumeration mismatch");

#ifdef __cplusplus
}
#endif
