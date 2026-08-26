/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/gdma_periph.h"
#include "soc/ahb_dma_reg.h"

const gdma_signal_conn_t gdma_periph_signals = {
    .groups = {
        [0] = {
            .pairs = {
                [0]  = {
                    .rx_irq_id = ETS_DMA_IN_CH0_INTR_SOURCE,
                    .tx_irq_id = ETS_DMA_OUT_CH0_INTR_SOURCE,
                    .name = "gdma_g0p0",
                },
                [1]  = {
                    .rx_irq_id = ETS_DMA_IN_CH1_INTR_SOURCE,
                    .tx_irq_id = ETS_DMA_OUT_CH1_INTR_SOURCE,
                    .name = "gdma_g0p1",
                }
            }
        }
    }
};
