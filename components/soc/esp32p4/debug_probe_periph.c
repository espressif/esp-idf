/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/gpio_sig_map.h"
#include "soc/debug_probe_periph.h"

const debug_probe_signal_conn_t debug_probe_periph_signals = {
    .units = {
        [0] = {
            .out_sig = {
                [0] = HP_PROBE_TOP_OUT0_IDX,
                [1] = HP_PROBE_TOP_OUT1_IDX,
                [2] = HP_PROBE_TOP_OUT2_IDX,
                [3] = HP_PROBE_TOP_OUT3_IDX,
                [4] = HP_PROBE_TOP_OUT4_IDX,
                [5] = HP_PROBE_TOP_OUT5_IDX,
                [6] = HP_PROBE_TOP_OUT6_IDX,
                [7] = HP_PROBE_TOP_OUT7_IDX,
                [8] = HP_PROBE_TOP_OUT8_IDX,
                [9] = HP_PROBE_TOP_OUT9_IDX,
                [10] = HP_PROBE_TOP_OUT10_IDX,
                [11] = HP_PROBE_TOP_OUT11_IDX,
                [12] = HP_PROBE_TOP_OUT12_IDX,
                [13] = HP_PROBE_TOP_OUT13_IDX,
                [14] = HP_PROBE_TOP_OUT14_IDX,
                [15] = HP_PROBE_TOP_OUT15_IDX,
            }
        }
    }
};
