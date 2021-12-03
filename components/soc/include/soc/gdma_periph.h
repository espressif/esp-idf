/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/periph_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    struct {
        const periph_module_t module;
        struct {
            const int rx_irq_id;
            const int tx_irq_id;
        } pairs[SOC_GDMA_PAIRS_PER_GROUP];
    } groups[SOC_GDMA_GROUPS];
} gdma_signal_conn_t;

extern const gdma_signal_conn_t gdma_periph_signals;

#ifdef __cplusplus
}
#endif
