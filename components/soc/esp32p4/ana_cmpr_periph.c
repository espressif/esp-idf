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
        .intr_src = ETS_GPIO_PAD_COMP_INTR_SOURCE,
    },
    [1] = {
        .src_gpio = ANA_CMPR1_SRC_GPIO,
        .ext_ref_gpio = ANA_CMPR1_EXT_REF_GPIO,
        .intr_src = ETS_GPIO_PAD_COMP_INTR_SOURCE,
    },
};

analog_cmpr_dev_t ANALOG_CMPR[SOC_ANA_CMPR_NUM] = {
    [0] = {
        .pad_comp_config = &LP_SYS.pad_comp[0],
        .pad_comp_filter = &GPIO.zero_det_filter_cnt[0],
        .int_st = &GPIO.int_st,
        .int_ena = &GPIO.int_ena,
        .int_clr = &GPIO.int_clr,
    },
    [1] = {
        .pad_comp_config = &LP_SYS.pad_comp[1],
        .pad_comp_filter = &GPIO.zero_det_filter_cnt[1],
        .int_st = &GPIO.int_st,
        .int_ena = &GPIO.int_ena,
        .int_clr = &GPIO.int_clr,
    },
};
