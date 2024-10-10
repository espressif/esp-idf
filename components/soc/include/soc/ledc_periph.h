/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/ledc_reg.h"
#include "soc/ledc_struct.h"
#if SOC_PAU_SUPPORTED
#include "soc/regdma.h"
#include "soc/retention_periph_defs.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 Stores a bunch of per-ledc-peripheral data.
*/
typedef struct {
    const uint8_t sig_out0_idx;
} ledc_signal_conn_t;

#if SOC_LEDC_SUPPORT_HS_MODE
extern const ledc_signal_conn_t ledc_periph_signal[2];
#else
extern const ledc_signal_conn_t ledc_periph_signal[1];
#endif

#if SOC_PAU_SUPPORTED

#if SOC_LIGHT_SLEEP_SUPPORTED
#if SOC_PHY_SUPPORTED
#define LEDC_RETENTION_ENTRY        (ENTRY(0) | ENTRY(2))
#else
#define LEDC_RETENTION_ENTRY        (ENTRY(0))
#endif
#else // !SOC_LIGHT_SLEEP_SUPPORTED
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

extern const ledc_reg_retention_info_t ledc_reg_retention_info;
#endif

#ifdef __cplusplus
}
#endif
