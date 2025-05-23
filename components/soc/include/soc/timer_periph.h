/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/timer_group_reg.h"
#include "soc/timer_group_struct.h"
#include "soc/soc_caps.h"
#include "soc/periph_defs.h"
#include "soc/regdma.h"

#if SOC_PAU_SUPPORTED
#include "soc/retention_periph_defs.h"
#endif // SOC_PAU_SUPPORTED

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    struct {
        const char *module_name[SOC_TIMER_GROUP_TIMERS_PER_GROUP];
        const periph_module_t module; // Peripheral module
        const int timer_irq_id[SOC_TIMER_GROUP_TIMERS_PER_GROUP]; // interrupt source ID
    } groups[SOC_TIMER_GROUPS];
} timer_group_signal_conn_t;

extern const timer_group_signal_conn_t timer_group_periph_signals;

#if SOC_PAU_SUPPORTED && SOC_TIMER_SUPPORT_SLEEP_RETENTION
typedef struct {
    const periph_retention_module_t module;
    const regdma_entries_config_t *regdma_entry_array;
    uint32_t array_size;
} tg_timer_reg_retention_info_t;

extern const tg_timer_reg_retention_info_t tg_timer_reg_retention_info[SOC_TIMER_GROUPS][SOC_TIMER_GROUP_TIMERS_PER_GROUP];
#endif // SOC_TIMER_SUPPORT_SLEEP_RETENTION

#ifdef __cplusplus
}
#endif
