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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define SOC_UART_FIFO_LEN          (128)      /*!< The UART hardware FIFO length */
#define SOC_UART_BITRATE_MAX       (5000000)  /*!< Max bit rate supported by UART */

#define SOC_UART_SUPPORT_RTC_CLK    (1)
#define SOC_UART_SUPPORT_XTAL_CLK   (1)

// ESP32-C3 have 2 UART
#define SOC_UART_NUM           (2)

// UART has an extra TX_WAIT_SEND state when the FIFO is not empty and XOFF is enabled
#define SOC_UART_SUPPORT_FSM_TX_WAIT_SEND   (1)
#define UART_FSM_IDLE                       (0x0)
#define UART_FSM_TX_WAIT_SEND               (0xf)

#ifdef __cplusplus
}
#endif
