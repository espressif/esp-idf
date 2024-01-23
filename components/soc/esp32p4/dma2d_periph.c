/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/dma2d_periph.h"
#include "soc/interrupts.h"

const dma2d_signal_conn_t dma2d_periph_signals = {
    .groups = {
        [0] = {
            .tx_irq_id = {
                [0] = ETS_DMA2D_OUT_CH0_INTR_SOURCE,
                [1] = ETS_DMA2D_OUT_CH1_INTR_SOURCE,
                [2] = ETS_DMA2D_OUT_CH2_INTR_SOURCE,
            },
            .rx_irq_id = {
                [0] = ETS_DMA2D_IN_CH0_INTR_SOURCE,
                [1] = ETS_DMA2D_IN_CH1_INTR_SOURCE,
            }
        }
    }
};
