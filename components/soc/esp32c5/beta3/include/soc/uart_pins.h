/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/io_mux_reg.h"

/* Specify the number of pins for UART */
#define SOC_UART_PINS_COUNT  (4)

/* Specify the GPIO pin number for each UART signal in the IOMUX */
#define U0RXD_GPIO_NUM 11
#define U0TXD_GPIO_NUM 10
#define U0RTS_GPIO_NUM (-1)
#define U0CTS_GPIO_NUM (-1)

#define U1RXD_GPIO_NUM (-1)
#define U1TXD_GPIO_NUM (-1)
#define U1RTS_GPIO_NUM (-1)
#define U1CTS_GPIO_NUM (-1)

#define LP_U0RXD_GPIO_NUM 4
#define LP_U0TXD_GPIO_NUM 5
#define LP_U0RTS_GPIO_NUM 2
#define LP_U0CTS_GPIO_NUM 3

/* The following defines are necessary for reconfiguring the UART
 * to use IOMUX, at runtime. */
#define U0TXD_MUX_FUNC  (FUNC_U0TXD_U0TXD)
#define U0RXD_MUX_FUNC  (FUNC_U0RXD_U0RXD)
/* No func for the following pins, they shall not be used */
#define U0RTS_MUX_FUNC  (-1)
#define U0CTS_MUX_FUNC  (-1)
/* Same goes for UART1 */
#define U1TXD_MUX_FUNC  (-1)
#define U1RXD_MUX_FUNC  (-1)
#define U1RTS_MUX_FUNC  (-1)
#define U1CTS_MUX_FUNC  (-1)

#define LP_U0TXD_MUX_FUNC  (1)
#define LP_U0RXD_MUX_FUNC  (1)
#define LP_U0RTS_MUX_FUNC  (1)
#define LP_U0CTS_MUX_FUNC  (1)
