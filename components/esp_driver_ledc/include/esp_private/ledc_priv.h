/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "hal/ledc_ll.h"
#include "hal/ledc_types.h"

#if SOC_PAU_SUPPORTED
#include "soc/regdma.h"
#include "soc/retention_periph_defs.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_PAU_SUPPORTED

#if SOC_LIGHT_SLEEP_SUPPORTED
#if SOC_PM_SUPPORT_PMU_MODEM_STATE
#define LEDC_RETENTION_ENTRY        (ENTRY(0) | ENTRY(2))
#else
#define LEDC_RETENTION_ENTRY        (ENTRY(0))
#endif
#else
#define LEDC_RETENTION_ENTRY        REGDMA_SW_TRIGGER_ENTRY
#endif

typedef struct {
    const regdma_entries_config_t *regdma_entry_array;
    uint32_t array_size;
} ledc_sub_reg_retention_info_t;

typedef struct {
    ledc_sub_reg_retention_info_t common;
    ledc_sub_reg_retention_info_t timer[SOC_LEDC_TIMER_NUM];
    ledc_sub_reg_retention_info_t channel[SOC_LEDC_CHANNEL_NUM];
    const periph_retention_module_t module_id;
} ledc_reg_retention_info_t;

extern const ledc_reg_retention_info_t ledc_reg_retention_info[LEDC_LL_GET(GROUP_NUM)];
#endif

#ifdef __cplusplus
}
#endif
