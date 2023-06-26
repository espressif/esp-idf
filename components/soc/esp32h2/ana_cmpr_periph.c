/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/ana_cmpr_periph.h"

const ana_cmpr_conn_t ana_cmpr_io_map[SOC_ANA_CMPR_NUM] = {
    [0] = {
        .src_gpio = ANA_CMPR0_SRC_GPIO,
        .ext_ref_gpio = ANA_CMPR0_EXT_REF_GPIO,
    },
};
