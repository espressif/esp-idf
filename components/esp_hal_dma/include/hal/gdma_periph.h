/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/periph_defs.h"
#include "soc/regdma.h"
#if SOC_HAS(GDMA)
#include "hal/gdma_ll.h"
#endif
#if SOC_HAS(PAU)
#include "soc/retention_periph_defs.h"
#endif

#if CI_TEST_SW_RETENTION
#define GDMA_RETENTION_ENTRY    REGDMA_SW_TRIGGER_ENTRY
#else
#if SOC_PHY_SUPPORTED
#define GDMA_RETENTION_ENTRY    (ENTRY(0) | ENTRY(2))
#else
#define GDMA_RETENTION_ENTRY    (ENTRY(0))
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_HAS(GDMA)
typedef struct {
    struct {
        const shared_periph_module_t module;
        struct {
            const int rx_irq_id;
            const int tx_irq_id;
        } pairs[GDMA_LL_GET(PAIRS_PER_INST)];
    } groups[GDMA_LL_GET(INST_NUM)];
} gdma_signal_conn_t;

extern const gdma_signal_conn_t gdma_periph_signals;

#if SOC_GDMA_SUPPORT_SLEEP_RETENTION && SOC_LIGHT_SLEEP_SUPPORTED
typedef struct {
    const regdma_entries_config_t *link_list;
    uint32_t link_num;
    const periph_retention_module_t module_id;
} gdma_chx_reg_ctx_link_t;

extern const gdma_chx_reg_ctx_link_t gdma_chx_regs_retention[GDMA_LL_GET(INST_NUM)][GDMA_LL_GET(PAIRS_PER_INST)];
#endif

#endif

#ifdef __cplusplus
}
#endif
