/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#include "soc/periph_defs.h"
#if SOC_ANA_CMPR_SUPPORTED
#include "soc/ana_cmpr_channel.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_ANA_CMPR_SUPPORTED
typedef struct {
    int src_gpio;
    int ext_ref_gpio;
    int intr_src;
} ana_cmpr_conn_t;

extern const ana_cmpr_conn_t ana_cmpr_io_map[SOC_ANA_CMPR_NUM];
#endif

#ifdef __cplusplus
}
#endif
