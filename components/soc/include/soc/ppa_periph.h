/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#if SOC_PAU_SUPPORTED
#include "soc/regdma.h"
#include "soc/retention_periph_defs.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_PPA_SUPPORTED
#if SOC_LIGHT_SLEEP_SUPPORTED
typedef struct {
    const periph_retention_module_t module;
    const regdma_entries_config_t *regdma_entry_array;
    uint32_t array_size;
} ppa_reg_retention_info_t;

extern const ppa_reg_retention_info_t ppa_reg_retention_info;
#endif
#endif

#ifdef __cplusplus
}
#endif
