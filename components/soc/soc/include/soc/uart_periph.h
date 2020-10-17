// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#pragma once

#include "soc/soc_caps.h"
#include "soc/uart_reg.h"
#include "soc/uart_struct.h"
#include "soc/periph_defs.h"
#include "soc/gpio_sig_map.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const uint8_t tx_sig;
    const uint8_t rx_sig;
    const uint8_t rts_sig;
    const uint8_t cts_sig;
    const uint8_t irq;
    const periph_module_t module;
} uart_signal_conn_t;

extern const uart_signal_conn_t uart_periph_signal[SOC_UART_NUM];

#ifdef __cplusplus
}
#endif

