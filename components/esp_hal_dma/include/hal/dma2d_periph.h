/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"

#if SOC_HAS(DMA2D)
#include "hal/dma2d_ll.h"
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
#endif

#ifdef __cplusplus
}
#endif
