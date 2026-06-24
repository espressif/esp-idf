/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/ana_cmpr_periph.h"
#include "soc/ana_cmpr_struct.h"

const ana_cmpr_periph_t ana_cmpr_periph[1] = {
    [0] = {
        .src_gpio = 11,
        .ext_ref_gpio = 10,
        .intr_src = ETS_GPIO_INTR_SOURCE,
        .module_name = "ANA_CMPR_U0",
    },
};

analog_cmpr_dev_t ANALOG_CMPR[1] = {
    [0] = {
        .pad_comp_config = &GPIO_EXT.pad_comp_config,
        .pad_comp_filter = &GPIO_EXT.pad_comp_filter,
        .int_st = &GPIO_EXT.int_st,
        .int_ena = &GPIO_EXT.int_ena,
        .int_clr = &GPIO_EXT.int_clr,
    },
};
