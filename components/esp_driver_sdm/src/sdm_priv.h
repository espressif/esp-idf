/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>
#include "soc/soc_caps.h"
#include "hal/sdm_caps.h"
#if SOC_PAU_SUPPORTED
#include "soc/regdma.h"
#include "soc/retention_periph_defs.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_PAU_SUPPORTED

typedef struct {
    const periph_retention_module_t module;
    const regdma_entries_config_t *regdma_entry_array;
    const size_t array_size;
} sdm_reg_retention_info_t;

extern const sdm_reg_retention_info_t sdm_reg_retention_infos[SDM_CAPS_GET(INST_NUM)];

#endif // SOC_PAU_SUPPORTED

#ifdef __cplusplus
}
#endif
