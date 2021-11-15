/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/gdma_periph.h"

const gdma_signal_conn_t gdma_periph_signals = {
    .groups = {
        [0] = {
            .module = PERIPH_GDMA_MODULE,
            .pairs = {
                [0]  = {
                    .rx_irq_id = ETS_DMA_CH0_INTR_SOURCE,
                    .tx_irq_id = ETS_DMA_CH0_INTR_SOURCE,
                },
                [1]  = {
                    .rx_irq_id = ETS_DMA_CH1_INTR_SOURCE,
                    .tx_irq_id = ETS_DMA_CH1_INTR_SOURCE,
                },
                [2]  = {
                    .rx_irq_id = ETS_DMA_CH2_INTR_SOURCE,
                    .tx_irq_id = ETS_DMA_CH2_INTR_SOURCE,
                }
            }
        }
    }
};
