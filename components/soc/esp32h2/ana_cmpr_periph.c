/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/ana_cmpr_periph.h"
#include "soc/ana_cmpr_struct.h"

const ana_cmpr_periph_t ana_cmpr_periph[SOC_ANA_CMPR_NUM] = {
    [0] = {
        .src_gpio = ANA_CMPR0_SRC_GPIO,
        .ext_ref_gpio = ANA_CMPR0_EXT_REF_GPIO,
        .intr_src = ETS_GPIO_INTR_SOURCE,
    },
};

analog_cmpr_dev_t ANALOG_CMPR[SOC_ANA_CMPR_NUM] = {
    [0] = {
        .pad_comp_config = &GPIO_EXT.pad_comp_config,
        .pad_comp_filter = &GPIO_EXT.pad_comp_filter,
        .int_st = &GPIO_EXT.int_st,
        .int_ena = &GPIO_EXT.int_ena,
        .int_clr = &GPIO_EXT.int_clr,
    },
};
