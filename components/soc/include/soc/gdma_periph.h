/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/periph_defs.h"

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

#endif

#ifdef __cplusplus
}
#endif
