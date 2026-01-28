/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"

#if SOC_HAS(DMA2D)
#include "hal/dma2d_ll.h"
#if SOC_HAS(PAU)
#include "soc/regdma.h"
#include "soc/retention_periph_defs.h"
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_HAS(DMA2D)
typedef struct {
    struct {
        const int tx_irq_id[DMA2D_LL_GET(TX_CHANS_PER_INST)];
        const int rx_irq_id[DMA2D_LL_GET(RX_CHANS_PER_INST)];
    } groups[DMA2D_LL_GET(INST_NUM)];
} dma2d_signal_conn_t;

extern const dma2d_signal_conn_t dma2d_periph_signals;

#if SOC_LIGHT_SLEEP_SUPPORTED
typedef struct {
    const periph_retention_module_t module;
    const regdma_entries_config_t *regdma_entry_array;
    uint32_t array_size;
} dma2d_reg_retention_info_t;

extern const dma2d_reg_retention_info_t dma2d_reg_retention_info;
#endif
#endif

#ifdef __cplusplus
}
#endif
