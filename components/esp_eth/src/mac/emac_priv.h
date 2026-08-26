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

#define EMAC_REGDMA_LINK_EMAC_START_BEGIN   (10)
#define EMAC_REGDMA_LINK_EMAC_START_CNT     (3)

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_PAU_SUPPORTED
typedef struct {
    const periph_retention_module_t module_id;
    const regdma_entries_config_t *entry_array;
    uint32_t array_size;
} emac_reg_retention_info_t;

extern const emac_reg_retention_info_t emac_reg_retention_info;
#endif

#ifdef __cplusplus
}
#endif
