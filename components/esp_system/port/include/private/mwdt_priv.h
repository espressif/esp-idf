/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#include "hal/mwdt_ll.h"
#if SOC_PAU_SUPPORTED
#include "soc/regdma.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_PAU_SUPPORTED
typedef struct {
    const regdma_entries_config_t *regdma_entry_array;
    uint32_t array_size;
} mwdt_reg_retention_info_t;

extern const mwdt_reg_retention_info_t mwdt_reg_retention_info[TIMG_LL_GET(INST_NUM)];
#endif

#ifdef __cplusplus
}
#endif
