/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"

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
#endif

#ifdef __cplusplus
}
#endif
