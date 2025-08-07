/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/sdm_periph.h"
#include "soc/gpio_sig_map.h"

const soc_sdm_signal_desc_t soc_sdm_signals[1] = {
    [0] = {
        .module_name = "SDM0",
        .channels = {
            [0] = {
                .sig_id_matrix = GPIO_SD0_OUT_IDX
            },
            [1] = {
                .sig_id_matrix = GPIO_SD1_OUT_IDX
            },
            [2] = {
                .sig_id_matrix = GPIO_SD2_OUT_IDX
            },
            [3] = {
                .sig_id_matrix = GPIO_SD3_OUT_IDX
            }
        }
    }
};
