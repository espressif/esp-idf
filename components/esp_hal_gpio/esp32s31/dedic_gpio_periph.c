/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/gpio_sig_map.h"
#include "hal/dedic_gpio_periph.h"

const dedic_gpio_signal_conn_t dedic_gpio_periph_signals = {
    .irq = -1,
    .cores = {
        [0] = {
            .in_sig_per_channel = {
                [0] = CORE_GPIO_IN_PAD_IN0_IDX,
                [1] = CORE_GPIO_IN_PAD_IN1_IDX,
                [2] = CORE_GPIO_IN_PAD_IN2_IDX,
                [3] = CORE_GPIO_IN_PAD_IN3_IDX,
                [4] = CORE_GPIO_IN_PAD_IN4_IDX,
                [5] = CORE_GPIO_IN_PAD_IN5_IDX,
                [6] = CORE_GPIO_IN_PAD_IN6_IDX,
                [7] = CORE_GPIO_IN_PAD_IN7_IDX,
            },
            .out_sig_per_channel = {
                [0] = CORE_GPIO_OUT_PAD_OUT0_IDX,
                [1] = CORE_GPIO_OUT_PAD_OUT1_IDX,
                [2] = CORE_GPIO_OUT_PAD_OUT2_IDX,
                [3] = CORE_GPIO_OUT_PAD_OUT3_IDX,
                [4] = CORE_GPIO_OUT_PAD_OUT4_IDX,
                [5] = CORE_GPIO_OUT_PAD_OUT5_IDX,
                [6] = CORE_GPIO_OUT_PAD_OUT6_IDX,
                [7] = CORE_GPIO_OUT_PAD_OUT7_IDX,
            }
        },
        [1] = {
            .in_sig_per_channel = {
                [0] = CORE_GPIO_IN_PAD_IN8_IDX,
                [1] = CORE_GPIO_IN_PAD_IN9_IDX,
                [2] = CORE_GPIO_IN_PAD_IN10_IDX,
                [3] = CORE_GPIO_IN_PAD_IN11_IDX,
                [4] = CORE_GPIO_IN_PAD_IN12_IDX,
                [5] = CORE_GPIO_IN_PAD_IN13_IDX,
                [6] = CORE_GPIO_IN_PAD_IN14_IDX,
                [7] = CORE_GPIO_IN_PAD_IN15_IDX,
            },
            .out_sig_per_channel = {
                [0] = CORE_GPIO_OUT_PAD_OUT8_IDX,
                [1] = CORE_GPIO_OUT_PAD_OUT9_IDX,
                [2] = CORE_GPIO_OUT_PAD_OUT10_IDX,
                [3] = CORE_GPIO_OUT_PAD_OUT11_IDX,
                [4] = CORE_GPIO_OUT_PAD_OUT12_IDX,
                [5] = CORE_GPIO_OUT_PAD_OUT13_IDX,
                [6] = CORE_GPIO_OUT_PAD_OUT14_IDX,
                [7] = CORE_GPIO_OUT_PAD_OUT15_IDX,
            }
        },
    },
};
