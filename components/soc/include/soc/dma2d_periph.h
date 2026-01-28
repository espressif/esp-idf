/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"

#if SOC_PAU_SUPPORTED
#include "soc/regdma.h"
#include "soc/retention_periph_defs.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_DMA2D_SUPPORTED
typedef struct {
    struct {
        const int tx_irq_id[SOC_DMA2D_TX_CHANNELS_PER_GROUP];
        const int rx_irq_id[SOC_DMA2D_RX_CHANNELS_PER_GROUP];
    } groups[SOC_DMA2D_GROUPS];
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
