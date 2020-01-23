// Copyright 2010-2019 Espressif Systems (Shanghai) PTE LTD
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

#define UART_FIFO_LEN          (128)      /*!< The UART hardware FIFO length */
#define UART_BITRATE_MAX       (5000000)  /*!< Max bit rate supported by UART */

// ESP32-S2 have 2 UART.
#define SOC_UART_NUM           (2)
#define SOC_UART_MIN_WAKEUP_THRESH (2)
#define UART_INTR_MASK         (0x7ffff) //All interrupt mask

#ifdef __cplusplus
}
#endif

