/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/rmt_periph.h"
#include "soc/gpio_sig_map.h"

const rmt_signal_conn_t rmt_periph_signals = {
    .groups = {
        [0] = {
            .module = PERIPH_RMT_MODULE,
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
                    .tx_sig = -1,
                    .rx_sig = RMT_SIG_IN0_IDX
                },
                [3] = {
                    .tx_sig = -1,
                    .rx_sig = RMT_SIG_IN1_IDX
                },
            }
        }
    }
};
