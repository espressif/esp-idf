/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#include "soc/ana_cmpr_channel.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int src_gpio;
    int ext_ref_gpio;
} ana_cmpr_conn_t;

extern const ana_cmpr_conn_t ana_cmpr_io_map[SOC_ANA_CMPR_NUM];

#ifdef __cplusplus
}
#endif
