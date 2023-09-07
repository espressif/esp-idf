/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/io_mux_reg.h"

/* Specify the number of pins for UART */
#define SOC_UART_PINS_COUNT  (4)

/* Specify the GPIO pin number for each UART signal in the IOMUX */
#define U0RXD_GPIO_NUM 38
#define U0TXD_GPIO_NUM 37
#define U0RTS_GPIO_NUM 8
#define U0CTS_GPIO_NUM 9

#define U1RXD_GPIO_NUM 11
#define U1TXD_GPIO_NUM 10
#define U1RTS_GPIO_NUM 12
#define U1CTS_GPIO_NUM 13

#define U2RXD_GPIO_NUM (-1)
#define U2TXD_GPIO_NUM (-1)
#define U2RTS_GPIO_NUM (-1)
#define U2CTS_GPIO_NUM (-1)

#define U3RXD_GPIO_NUM (-1)
#define U3TXD_GPIO_NUM (-1)
#define U3RTS_GPIO_NUM (-1)
#define U3CTS_GPIO_NUM (-1)

#define U4RXD_GPIO_NUM (-1)
#define U4TXD_GPIO_NUM (-1)
#define U4RTS_GPIO_NUM (-1)
#define U4CTS_GPIO_NUM (-1)

#define LP_U0RXD_GPIO_NUM 15
#define LP_U0TXD_GPIO_NUM 14
#define LP_U0RTS_GPIO_NUM (-1)
#define LP_U0CTS_GPIO_NUM (-1)

/* The following defines are necessary for reconfiguring the UART
 * to use IOMUX, at runtime. */
#define U0TXD_MUX_FUNC  (FUNC_GPIO37_UART0_TXD_PAD)
#define U0RXD_MUX_FUNC  (FUNC_GPIO38_UART0_RXD_PAD)
#define U0RTS_MUX_FUNC  (FUNC_GPIO8_UART0_RTS_PAD)
#define U0CTS_MUX_FUNC  (FUNC_GPIO9_UART0_CTS_PAD)

/* Same goes for UART1 */
#define U1TXD_MUX_FUNC  (FUNC_GPIO10_UART1_TXD_PAD)
#define U1RXD_MUX_FUNC  (FUNC_GPIO11_UART1_RXD_PAD)
#define U1RTS_MUX_FUNC  (FUNC_GPIO12_UART1_RTS_PAD)
#define U1CTS_MUX_FUNC  (FUNC_GPIO13_UART1_CTS_PAD)

/* No func for the following pins, they shall not be used */
#define U2TXD_MUX_FUNC  (-1)
#define U2RXD_MUX_FUNC  (-1)
#define U2RTS_MUX_FUNC  (-1)
#define U2CTS_MUX_FUNC  (-1)

#define U3TXD_MUX_FUNC  (-1)
#define U3RXD_MUX_FUNC  (-1)
#define U3RTS_MUX_FUNC  (-1)
#define U3CTS_MUX_FUNC  (-1)

#define U4TXD_MUX_FUNC  (-1)
#define U4RXD_MUX_FUNC  (-1)
#define U4RTS_MUX_FUNC  (-1)
#define U4CTS_MUX_FUNC  (-1)

#define LP_U0TXD_MUX_FUNC  (0)
#define LP_U0RXD_MUX_FUNC  (0)
#define LP_U0RTS_MUX_FUNC  (-1)
#define LP_U0CTS_MUX_FUNC  (-1)
