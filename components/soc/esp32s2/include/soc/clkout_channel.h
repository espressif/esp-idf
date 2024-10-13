/*
 * SPDX-FileCopyrightText: 2010-2023 Espressif Systems (Shanghai) CO LTD
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

#define CLKOUT_CHANNEL1_GPIO    GPIO_NUM_20
#define CLKOUT_CHANNEL2_GPIO    GPIO_NUM_19
#define CLKOUT_CHANNEL3_GPIO    GPIO_NUM_18
#define FUNC_CLK_OUT1           FUNC_GPIO20_CLK_OUT1
#define FUNC_CLK_OUT2           FUNC_GPIO19_CLK_OUT2
#define FUNC_CLK_OUT3           FUNC_DAC_2_CLK_OUT3

#define IONUM_TO_CLKOUT_CHANNEL(gpio_num)   ((gpio_num == CLKOUT_CHANNEL1_GPIO) ? CLKOUT_CHANNEL_1 : \
                                             (gpio_num == CLKOUT_CHANNEL2_GPIO) ? CLKOUT_CHANNEL_2 : \
                                             (gpio_num == CLKOUT_CHANNEL3_GPIO) ? CLKOUT_CHANNEL_3 : -1)
#define CLKOUT_CHANNEL_TO_IOMUX_FUNC(channel)   ((channel == CLKOUT_CHANNEL_1) ? FUNC_CLK_OUT1 : \
                                                 (channel == CLKOUT_CHANNEL_2) ? FUNC_CLK_OUT2 : \
                                                 (channel == CLKOUT_CHANNEL_3) ? FUNC_CLK_OUT3 : -1)
#define IS_VALID_CLKOUT_IO(gpio_num)    ((gpio_num == CLKOUT_CHANNEL1_GPIO) || (gpio_num == CLKOUT_CHANNEL2_GPIO) || (gpio_num == CLKOUT_CHANNEL3_GPIO))

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
