/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#pragma once

// TODO: to be checked IDF-14789

/* Specify the GPIO pin number for each UART signal in the IOMUX */
#define U0RXD_GPIO_NUM 59
#define U0TXD_GPIO_NUM 58
#define U0RTS_GPIO_NUM (-1)
#define U0CTS_GPIO_NUM (-1)
// DTR
// DSR

#define U1RXD_GPIO_NUM (-1)
#define U1TXD_GPIO_NUM (-1)
#define U1RTS_GPIO_NUM (-1)
#define U1CTS_GPIO_NUM (-1)

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

#define LP_U0RXD_GPIO_NUM 7
#define LP_U0TXD_GPIO_NUM 6
#define LP_U0RTS_GPIO_NUM (-1)
#define LP_U0CTS_GPIO_NUM (-1)

/* The following defines are necessary for reconfiguring the UART
 * to use IOMUX, at runtime. */
#define U0TXD_MUX_FUNC  (0)
#define U0RXD_MUX_FUNC  (0)
#define U0RTS_MUX_FUNC  (-1)
#define U0CTS_MUX_FUNC  (-1)

#define U1TXD_MUX_FUNC  (-1)
#define U1RXD_MUX_FUNC  (-1)
#define U1RTS_MUX_FUNC  (-1)
#define U1CTS_MUX_FUNC  (-1)

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
