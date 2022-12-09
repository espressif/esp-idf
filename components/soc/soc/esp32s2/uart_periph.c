// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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

#include "soc/uart_periph.h"

/*
 Bunch of constants for every UART peripheral: GPIO signals, irqs, hw addr of registers etc
*/
const uart_signal_conn_t uart_periph_signal[SOC_UART_NUM] = {
    {
        .tx_sig = U0TXD_OUT_IDX,
        .rx_sig = U0RXD_IN_IDX,
        .rts_sig = U0RTS_OUT_IDX,
        .cts_sig = U0CTS_IN_IDX,
        .irq = ETS_UART0_INTR_SOURCE,
        .module = PERIPH_UART0_MODULE,
    },
    {
        .tx_sig = U1TXD_OUT_IDX,
        .rx_sig = U1RXD_IN_IDX,
        .rts_sig = U1RTS_OUT_IDX,
        .cts_sig = U1CTS_IN_IDX,
        .irq = ETS_UART1_INTR_SOURCE,
        .module = PERIPH_UART1_MODULE,
    },
};
