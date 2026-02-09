/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/dma2d_periph.h"
#include "soc/interrupts.h"

const dma2d_signal_conn_t dma2d_periph_signals = {
    .groups = {
        [0] = {
            .tx_irq_id = {
                [0] = ETS_DMA2D_OUT_CH0_INTR_SOURCE,
                [1] = ETS_DMA2D_OUT_CH1_INTR_SOURCE,
                [2] = ETS_DMA2D_OUT_CH2_INTR_SOURCE,
#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300
                [3] = ETS_DMA2D_OUT_CH3_INTR_SOURCE,
#endif
            },
            .rx_irq_id = {
                [0] = ETS_DMA2D_IN_CH0_INTR_SOURCE,
                [1] = ETS_DMA2D_IN_CH1_INTR_SOURCE,
#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300
                [2] = ETS_DMA2D_IN_CH2_INTR_SOURCE,
#endif
            }
        }
    }
};
