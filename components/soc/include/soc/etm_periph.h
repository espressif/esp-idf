/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps_full.h"
#include "soc/regdma.h"

#if SOC_HAS(PAU)
#include "soc/retention_periph_defs.h"
#endif

// helper macros to access module attributes
#define SOC_ETM_ATTR(_attr)    SOC_MODULE_ATTR(ETM, _attr)

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_HAS(PAU)

typedef struct {
    periph_retention_module_t module;                  // ID of the ETM as a retention module
    const regdma_entries_config_t *regdma_entry_array; // Array of regdma entries for retention
    uint32_t array_size;                               // Size of the regdma_entry_array
} soc_etm_retention_desc_t;

extern const soc_etm_retention_desc_t soc_etm_retention_info[SOC_ETM_ATTR(INST_NUM)];

#endif // SOC_HAS(PAU)

#ifdef __cplusplus
}
#endif
