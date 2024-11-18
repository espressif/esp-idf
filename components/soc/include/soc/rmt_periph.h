/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/periph_defs.h"
#include "soc/regdma.h"

#if SOC_RMT_SUPPORT_SLEEP_RETENTION
#include "soc/retention_periph_defs.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_RMT_SUPPORTED

typedef struct {
    struct {
        const int irq;
        struct {
            struct {
                const int tx_sig;
                const int rx_sig;
            };
        } channels[SOC_RMT_CHANNELS_PER_GROUP];
    } groups[SOC_RMT_GROUPS];
} rmt_signal_conn_t;

extern const rmt_signal_conn_t rmt_periph_signals;

#if SOC_RMT_SUPPORT_SLEEP_RETENTION
typedef struct {
    periph_retention_module_t module;
    const regdma_entries_config_t *regdma_entry_array;
    uint32_t array_size;
} rmt_reg_retention_info_t;

// TODO: implement the retention link on the channel level, this can:
// - save memory when not all RMT channels are used
// - specify different retention dependency, e.g. only RMT channel x is capable to use DMA, we only want to add the DMA dependency for that channel
extern const rmt_reg_retention_info_t rmt_reg_retention_info[SOC_RMT_GROUPS];
#endif // SOC_RMT_SUPPORT_SLEEP_RETENTION

#endif // SOC_RMT_SUPPORTED

#ifdef __cplusplus
}
#endif
