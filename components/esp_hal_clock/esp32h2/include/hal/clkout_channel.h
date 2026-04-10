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
    CLKOUT_CHANNEL_3,
    CLKOUT_CHANNEL_MAX,
} clock_out_channel_t;

#define CLKOUT_CHANNEL_TO_GPIO_SIG_ID(channel)  ((channel == CLKOUT_CHANNEL_1) ? CLK_OUT_OUT1_IDX : \
                                                 (channel == CLKOUT_CHANNEL_2) ? CLK_OUT_OUT2_IDX : \
                                                 (channel == CLKOUT_CHANNEL_3) ? CLK_OUT_OUT3_IDX : SIG_GPIO_OUT_IDX)

#define CLKOUT_CHANNEL_MASK(channel)    ((channel == CLKOUT_CHANNEL_1) ? CLK_OUT1 : \
                                         (channel == CLKOUT_CHANNEL_2) ? CLK_OUT2 : \
                                         (channel == CLKOUT_CHANNEL_3) ? CLK_OUT3 : 0)

#define CLKOUT_CHANNEL_SHIFT(channel)   ((channel == CLKOUT_CHANNEL_1) ? CLK_OUT1_S : \
                                         (channel == CLKOUT_CHANNEL_2) ? CLK_OUT2_S : \
                                         (channel == CLKOUT_CHANNEL_3) ? CLK_OUT3_S : 0)

ESP_STATIC_ASSERT(CLKOUT_CHANNEL_MAX == SOC_GPIO_CLOCKOUT_CHANNEL_NUM, "clock_out_channel enumeration mismatch");

#ifdef __cplusplus
}
#endif
