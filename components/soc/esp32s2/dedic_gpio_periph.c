/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/dedic_gpio_periph.h"
#include "soc/gpio_sig_map.h"

const dedic_gpio_signal_conn_t dedic_gpio_periph_signals = {
    .irq = ETS_DEDICATED_GPIO_INTR_SOURCE,
    .cores = {
        [0] = {
            .in_sig_per_channel = {
                [0] = PRO_ALONEGPIO_IN0_IDX,
                [1] = PRO_ALONEGPIO_IN1_IDX,
                [2] = PRO_ALONEGPIO_IN2_IDX,
                [3] = PRO_ALONEGPIO_IN3_IDX,
                [4] = PRO_ALONEGPIO_IN4_IDX,
                [5] = PRO_ALONEGPIO_IN5_IDX,
                [6] = PRO_ALONEGPIO_IN6_IDX,
                [7] = PRO_ALONEGPIO_IN7_IDX,
            },
            .out_sig_per_channel = {
                [0] = PRO_ALONEGPIO_OUT0_IDX,
                [1] = PRO_ALONEGPIO_OUT1_IDX,
                [2] = PRO_ALONEGPIO_OUT2_IDX,
                [3] = PRO_ALONEGPIO_OUT3_IDX,
                [4] = PRO_ALONEGPIO_OUT4_IDX,
                [5] = PRO_ALONEGPIO_OUT5_IDX,
                [6] = PRO_ALONEGPIO_OUT6_IDX,
                [7] = PRO_ALONEGPIO_OUT7_IDX,
            }
        },
    },
};
