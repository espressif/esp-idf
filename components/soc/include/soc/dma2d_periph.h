/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps_full.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_HAS(DMA2D)

// helper macros to access module attributes
#define SOC_DMA2D_ATTR(_attr)    SOC_MODULE_ATTR(DMA2D, _attr)

typedef struct {
    struct {
        const int tx_irq_id[SOC_DMA2D_ATTR(TX_CHANS_PER_INST)];
        const int rx_irq_id[SOC_DMA2D_ATTR(RX_CHANS_PER_INST)];
    } groups[SOC_DMA2D_ATTR(INST_NUM)];
} dma2d_signal_conn_t;

extern const dma2d_signal_conn_t dma2d_periph_signals;
#endif

#ifdef __cplusplus
}
#endif
