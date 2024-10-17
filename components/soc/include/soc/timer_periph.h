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
<<<<<<< HEAD
=======

#if SOC_TIMER_SUPPORT_SLEEP_RETENTION
#include "soc/retention_periph_defs.h"
#endif // SOC_TIMER_SUPPORT_SLEEP_RETENTION
>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    struct {
        const periph_module_t module; // Peripheral module
        const int timer_irq_id[SOC_TIMER_GROUP_TIMERS_PER_GROUP]; // interrupt source ID
    } groups[SOC_TIMER_GROUPS];
} timer_group_signal_conn_t;

extern const timer_group_signal_conn_t timer_group_periph_signals;

<<<<<<< HEAD
#if SOC_TIMER_SUPPORT_SLEEP_RETENTION && SOC_PAU_SUPPORTED
typedef struct {
    const regdma_entries_config_t *link_list;
    uint32_t link_num;
} tg_reg_ctx_link_t;

extern const tg_reg_ctx_link_t tg_wdt_regs_retention[SOC_TIMER_GROUPS];
extern const tg_reg_ctx_link_t tg_timer_regs_retention[SOC_TIMER_GROUPS];
#endif
=======
#if SOC_TIMER_SUPPORT_SLEEP_RETENTION
typedef struct {
    const periph_retention_module_t module;
    const regdma_entries_config_t *regdma_entry_array;
    uint32_t array_size;
} tg_timer_reg_retention_info_t;

extern const tg_timer_reg_retention_info_t tg_timer_reg_retention_info[SOC_TIMER_GROUPS];
#endif // SOC_TIMER_SUPPORT_SLEEP_RETENTION
>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade

#ifdef __cplusplus
}
#endif
