/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/regdma.h"

#if SOC_ETM_SUPPORT_SLEEP_RETENTION
#include "soc/retention_periph_defs.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_ETM_SUPPORT_SLEEP_RETENTION
typedef struct {
    periph_retention_module_t module;
    const regdma_entries_config_t *regdma_entry_array;
    uint32_t array_size;
} etm_reg_retention_info_t;

extern const etm_reg_retention_info_t etm_reg_retention_info[SOC_ETM_GROUPS];
#endif // SOC_ETM_SUPPORT_SLEEP_RETENTION

#ifdef __cplusplus
}
#endif
