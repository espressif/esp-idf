// Copyright 2019-2020 Espressif Systems (Shanghai) PTE LTD
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

#pragma once

#include "soc/soc_caps.h"
#include "soc/periph_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    struct {
        struct {
            const int tx_sig;
            const int rx_sig;
        };
    } channels[SOC_RMT_CHANNELS_PER_GROUP];
    const int irq;
    const periph_module_t module;
} rmt_signal_conn_t;

extern const rmt_signal_conn_t rmt_periph_signals;

#ifdef __cplusplus
}
#endif
