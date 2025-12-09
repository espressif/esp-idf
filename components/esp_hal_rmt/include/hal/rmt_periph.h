/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/periph_defs.h"
#include "soc/regdma.h"
#if SOC_HAS(RMT)
#include "hal/rmt_ll.h"
#endif

#if SOC_RMT_SUPPORT_SLEEP_RETENTION
#include "soc/retention_periph_defs.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_HAS(RMT)

typedef struct {
    const int irq;
    struct {
        struct {
            const int tx_sig;
            const int rx_sig;
        };
    } channels[RMT_LL_GET(CHANS_PER_INST)];
} soc_rmt_signal_desc_t;

extern const soc_rmt_signal_desc_t soc_rmt_signals[RMT_LL_GET(INST_NUM)];

#if SOC_RMT_SUPPORT_SLEEP_RETENTION
typedef struct {
    periph_retention_module_t module;
    const regdma_entries_config_t *regdma_entry_array;
    uint32_t array_size;
} rmt_reg_retention_info_t;

// TODO: implement the retention link on the channel level, this can:
// - save memory when not all RMT channels are used
// - specify different retention dependency, e.g. only RMT channel x is capable to use DMA, we only want to add the DMA dependency for that channel
extern const rmt_reg_retention_info_t rmt_reg_retention_info[RMT_LL_GET(INST_NUM)];
#endif // SOC_RMT_SUPPORT_SLEEP_RETENTION

#endif // SOC_HAS(RMT)

#ifdef __cplusplus
}
#endif
