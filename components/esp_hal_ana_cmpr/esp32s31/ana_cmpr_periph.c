/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/ana_cmpr_periph.h"

const ana_cmpr_periph_t ana_cmpr_periph[1] = {
    [0] = {
        .pad_gpios = {37, 38, 39, 40},
        .intr_src = ETS_ZERO_DET_INTR_SOURCE,
        .module_name = "ANA_CMPR_U0",
    },
};
