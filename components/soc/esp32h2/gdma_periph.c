// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
