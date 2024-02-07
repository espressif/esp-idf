/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/periph_defs.h"

#if SOC_PM_SUPPORT_TOP_PD
#include "soc/regdma.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_GDMA_SUPPORTED
typedef struct {
    struct {
        const periph_module_t module;
        struct {
            const int rx_irq_id;
            const int tx_irq_id;
        } pairs[SOC_GDMA_PAIRS_PER_GROUP_MAX];
    } groups[SOC_GDMA_NUM_GROUPS_MAX];
} gdma_signal_conn_t;

extern const gdma_signal_conn_t gdma_periph_signals;

#if SOC_GDMA_SUPPORT_SLEEP_RETENTION
typedef struct {
    const regdma_entries_config_t *link_list;
    uint32_t link_num;
} gdma_chx_reg_ctx_link_t;

extern const gdma_chx_reg_ctx_link_t gdma_chx_regs_retention[SOC_GDMA_PAIRS_PER_GROUP_MAX][SOC_GDMA_PAIRS_PER_GROUP_MAX];
#endif

#endif

#ifdef __cplusplus
}
#endif
