// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "soc/rmt_periph.h"
#include "soc/gpio_sig_map.h"

const rmt_signal_conn_t rmt_periph_signals = {
    .groups = {
        [0] = {
            .module = PERIPH_RMT_MODULE,
            .irq = ETS_RMT_INTR_SOURCE,
            .channels = {
                [0] = {
                    .tx_sig = RMT_SIG_OUT0_IDX,
                    .rx_sig = -1
                },
                [1] = {
                    .tx_sig = RMT_SIG_OUT1_IDX,
                    .rx_sig = -1
                },
                [2] = {
                    .tx_sig = -1,
                    .rx_sig = RMT_SIG_IN0_IDX
                },
                [3] = {
                    .tx_sig = -1,
                    .rx_sig = RMT_SIG_IN1_IDX
                },
            }
        }
    }
};
