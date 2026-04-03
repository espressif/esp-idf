/*
 * SPDX-FileCopyrightText: 2019-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/interrupts.h"
#if SOC_LEDC_SUPPORTED
#include "hal/ledc_ll.h"
#include "hal/ledc_types.h"
#endif
#if SOC_PAU_SUPPORTED
#include "soc/regdma.h"
#include "soc/retention_periph_defs.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_LEDC_SUPPORTED
/*
 Stores a bunch of per-ledc-peripheral data.
*/
typedef struct {
    const int irq_id;
    struct {
        const int sig_out0_idx;
    } speed_mode[LEDC_SPEED_MODE_MAX];
} ledc_signal_conn_t;

extern const ledc_signal_conn_t ledc_periph_signal[LEDC_LL_GET(GROUP_NUM)];

#if SOC_PAU_SUPPORTED && SOC_LEDC_SUPPORT_SLEEP_RETENTION

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

extern const ledc_reg_retention_info_t ledc_reg_retention_info[LEDC_LL_GET(GROUP_NUM)];
#endif // SOC_PAU_SUPPORTED && SOC_LEDC_SUPPORT_SLEEP_RETENTION

#endif // SOC_LEDC_SUPPORTED

#ifdef __cplusplus
}
#endif
