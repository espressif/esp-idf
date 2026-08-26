/*
 * SPDX-FileCopyrightText: 2020-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/periph_defs.h"
#if SOC_HAS(GDMA)
#include "hal/gdma_ll.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_HAS(GDMA)
typedef struct {
    const int rx_irq_id;
    const int tx_irq_id;
    const char *name; // pair name, format: "gdma_gXpY"
} gdma_pair_signal_conn_t;

typedef struct {
    struct {
        const shared_periph_module_t module;
        gdma_pair_signal_conn_t pairs[GDMA_LL_GET(PAIRS_PER_INST)];
    } groups[GDMA_LL_GET(INST_NUM)];
} gdma_signal_conn_t;

extern const gdma_signal_conn_t gdma_periph_signals;

#endif

#ifdef __cplusplus
}
#endif
