/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#pragma once

/* Specify the GPIO pin number for each UART signal in the IOMUX */
#define U0RXD_GPIO_NUM 15
#define U0TXD_GPIO_NUM 16
#define U0RTS_GPIO_NUM (-1)
#define U0CTS_GPIO_NUM (-1)
#define U0DTR_GPIO_NUM (-1)
#define U0DSR_GPIO_NUM (-1)

#define U1RXD_GPIO_NUM (-1)
#define U1TXD_GPIO_NUM (-1)
#define U1RTS_GPIO_NUM (-1)
#define U1CTS_GPIO_NUM (-1)
#define U1DTR_GPIO_NUM (-1)
#define U1DSR_GPIO_NUM (-1)

/* The following defines are necessary for reconfiguring the UART
 * to use IOMUX, at runtime. */
#define U0TXD_MUX_FUNC  (0)
#define U0RXD_MUX_FUNC  (0)
/* No func for the following pins, they shall not be used */
#define U0RTS_MUX_FUNC  (-1)
#define U0CTS_MUX_FUNC  (-1)
#define U0DTR_MUX_FUNC  (-1)
#define U0DSR_MUX_FUNC  (-1)

/* Same goes for UART1 */
#define U1TXD_MUX_FUNC  (-1)
#define U1RXD_MUX_FUNC  (-1)
#define U1RTS_MUX_FUNC  (-1)
#define U1CTS_MUX_FUNC  (-1)
#define U1DTR_MUX_FUNC  (-1)
#define U1DSR_MUX_FUNC  (-1)
