/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/gpio_sig_map.h"
#include "soc/dedic_gpio_periph.h"

const dedic_gpio_signal_conn_t dedic_gpio_periph_signals = {
    .irq = -1,
    .cores = {
        [0] = {
            .in_sig_per_channel = {
                [0] = CPU0_GPIO_IN0_IDX,
                [1] = CPU0_GPIO_IN1_IDX,
                [2] = CPU0_GPIO_IN2_IDX,
                [3] = CPU0_GPIO_IN3_IDX,
                [4] = CPU0_GPIO_IN4_IDX,
                [5] = CPU0_GPIO_IN5_IDX,
                [6] = CPU0_GPIO_IN6_IDX,
                [7] = CPU0_GPIO_IN7_IDX,
            },
            .out_sig_per_channel = {
                [0] = CPU0_GPIO_OUT0_IDX,
                [1] = CPU0_GPIO_OUT1_IDX,
                [2] = CPU0_GPIO_OUT2_IDX,
                [3] = CPU0_GPIO_OUT3_IDX,
                [4] = CPU0_GPIO_OUT4_IDX,
                [5] = CPU0_GPIO_OUT5_IDX,
                [6] = CPU0_GPIO_OUT6_IDX,
                [7] = CPU0_GPIO_OUT7_IDX,
            }
        },
        [1] = {
            .in_sig_per_channel = {
                [0] = CPU1_GPIO_IN0_IDX,
                [1] = CPU1_GPIO_IN1_IDX,
                [2] = CPU1_GPIO_IN2_IDX,
                [3] = CPU1_GPIO_IN3_IDX,
                [4] = CPU1_GPIO_IN4_IDX,
                [5] = CPU1_GPIO_IN5_IDX,
                [6] = CPU1_GPIO_IN6_IDX,
                [7] = CPU1_GPIO_IN7_IDX,
            },
            .out_sig_per_channel = {
                [0] = CPU1_GPIO_OUT0_IDX,
                [1] = CPU1_GPIO_OUT1_IDX,
                [2] = CPU1_GPIO_OUT2_IDX,
                [3] = CPU1_GPIO_OUT3_IDX,
                [4] = CPU1_GPIO_OUT4_IDX,
                [5] = CPU1_GPIO_OUT5_IDX,
                [6] = CPU1_GPIO_OUT6_IDX,
                [7] = CPU1_GPIO_OUT7_IDX,
            }
        },
    },
};
