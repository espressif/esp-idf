/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/rmt_periph.h"
#include "soc/gpio_sig_map.h"

const soc_rmt_signal_desc_t soc_rmt_signals[1] = {
    [0] = {
        .irq = ETS_RMT_INTR_SOURCE,
        .channels = {
            [0] = {
                .tx_sig = RMT_SIG_OUT0_IDX,
                .rx_sig = -1
            },
            [1] = {
                .tx_sig = RMT_SIG_OUT1_IDX,
                .rx_sig = -1
            },
            [2] = {
                .tx_sig = RMT_SIG_OUT2_IDX,
                .rx_sig = -1
            },
            [3] = {
                .tx_sig = RMT_SIG_OUT3_IDX,
                .rx_sig = -1
            },
            [4] = {
                .tx_sig = -1,
                .rx_sig = RMT_SIG_IN0_IDX
            },
            [5] = {
                .tx_sig = -1,
                .rx_sig = RMT_SIG_IN1_IDX
            },
            [6] = {
                .tx_sig = -1,
                .rx_sig = RMT_SIG_IN2_IDX
            },
            [7] = {
                .tx_sig = -1,
                .rx_sig = RMT_SIG_IN3_IDX
            }
        }
    }
};
