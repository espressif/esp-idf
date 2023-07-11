/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/gdma_periph.h"

const gdma_signal_conn_t gdma_periph_signals = {
    .groups = {
        [0] = {
            .module = PERIPH_AHB_PDMA_MODULE,
            .pairs = {
                [0] = {
                    .rx_irq_id = ETS_AHB_PDMA_IN_CH0_INTR_SOURCE,
                    .tx_irq_id = ETS_AHB_PDMA_OUT_CH0_INTR_SOURCE,
                },
                [1] = {
                    .rx_irq_id = ETS_AHB_PDMA_IN_CH1_INTR_SOURCE,
                    .tx_irq_id = ETS_AHB_PDMA_OUT_CH1_INTR_SOURCE,
                },
                [2] = {
                    .rx_irq_id = ETS_AHB_PDMA_IN_CH2_INTR_SOURCE,
                    .tx_irq_id = ETS_AHB_PDMA_OUT_CH2_INTR_SOURCE,
                }
            }
        },
        [1] = {
            .module = PERIPH_AXI_PDMA_MODULE,
            .pairs = {
                [0] = {
                    .rx_irq_id = ETS_AXI_PDMA_IN_CH0_INTR_SOURCE,
                    .tx_irq_id = ETS_AXI_PDMA_OUT_CH0_INTR_SOURCE,
                },
                [1] = {
                    .rx_irq_id = ETS_AXI_PDMA_IN_CH1_INTR_SOURCE,
                    .tx_irq_id = ETS_AXI_PDMA_OUT_CH1_INTR_SOURCE,
                },
                [2] = {
                    .rx_irq_id = ETS_AXI_PDMA_IN_CH2_INTR_SOURCE,
                    .tx_irq_id = ETS_AXI_PDMA_OUT_CH2_INTR_SOURCE,
                }
            }
        }
    }
};
