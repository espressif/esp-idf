/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/sdm_periph.h"
#include "soc/gpio_sig_map.h"

const sigma_delta_signal_conn_t sigma_delta_periph_signals = {
    .channels = {
        [0] = {
            GPIO_SD0_OUT_IDX
        },
        [1] = {
            GPIO_SD1_OUT_IDX
        },
        [2] = {
            GPIO_SD2_OUT_IDX
        },
        [3] = {
            GPIO_SD3_OUT_IDX
        }
    }
};
