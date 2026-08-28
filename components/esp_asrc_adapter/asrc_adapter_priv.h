/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "asrc_adapter.h"

#if SOC_PAU_SUPPORTED
#include "soc/regdma.h"
#include "soc/retention_periph_defs.h"
#endif  /* SOC_PAU_SUPPORTED */

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#if SOC_PAU_SUPPORTED
/**
 * @brief  Use retention link only when PM is enabled and peripheral power-down in light sleep is allowed
 */
#define ASRC_USE_RETENTION_LINK (CONFIG_PM_ENABLE && CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP)

/**
 * @brief  ASRC register sleep retention information
 */
typedef struct {
    const periph_retention_module_t  retention_module;
    const regdma_entries_config_t   *entry_array;
    uint32_t                         array_size;
} asrc_reg_retention_info_t;

extern const asrc_reg_retention_info_t asrc_reg_retention_info[1];
#endif  /* SOC_PAU_SUPPORTED */

#ifdef __cplusplus
}
#endif  /* __cplusplus */
