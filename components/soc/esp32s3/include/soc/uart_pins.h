// Copyright 2015-2021 Espressif Systems (Shanghai) PTE LTD
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

#include "soc/io_mux_reg.h"

/* Specify the number of pins for UART */
#define SOC_UART_PINS_COUNT  (4)

/* Specify the GPIO pin number for each UART signal in the IOMUX */
#define U0RXD_GPIO_NUM 44
#define U0TXD_GPIO_NUM 43
#define U0CTS_GPIO_NUM 16
#define U0RTS_GPIO_NUM 15

#define U1RXD_GPIO_NUM 18
#define U1TXD_GPIO_NUM 17
#define U1CTS_GPIO_NUM 20
#define U1RTS_GPIO_NUM 19

#define U2RXD_GPIO_NUM (-1)
#define U2TXD_GPIO_NUM (-1)
#define U2CTS_GPIO_NUM (-1)
#define U2RTS_GPIO_NUM (-1)

/* The following defines are necessary for reconfiguring the UART
 * to use IOMUX, at runtime. */
#define U0TXD_MUX_FUNC  (FUNC_U0TXD_U0TXD)
#define U0RXD_MUX_FUNC  (FUNC_U0RXD_U0RXD)
#define U0RTS_MUX_FUNC  (FUNC_XTAL_32K_P_U0RTS)
#define U0CTS_MUX_FUNC  (FUNC_XTAL_32K_N_U0CTS)

#define U1TXD_MUX_FUNC  (FUNC_DAC_1_U1TXD)
#define U1RXD_MUX_FUNC  (FUNC_DAC_2_U1RXD)
#define U1RTS_MUX_FUNC  (FUNC_GPIO19_U1RTS)
#define U1CTS_MUX_FUNC  (FUNC_GPIO20_U1CTS)

/* UART2 cannot be used directly through the IOMUX, these value
 * shall not be used. */
#define U2TXD_MUX_FUNC  (-1)
#define U2RXD_MUX_FUNC  (-1)
#define U2RTS_MUX_FUNC  (-1)
#define U2CTS_MUX_FUNC  (-1)
