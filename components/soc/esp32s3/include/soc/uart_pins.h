/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#pragma once

/* Specify the GPIO pin number for each UART signal in the IOMUX */
#define U0RXD_GPIO_NUM 44
#define U0TXD_GPIO_NUM 43
#define U0CTS_GPIO_NUM 16
#define U0RTS_GPIO_NUM 15
#define U0DTR_GPIO_NUM (-1)
#define U0DSR_GPIO_NUM (-1)

#define U1RXD_GPIO_NUM 18
#define U1TXD_GPIO_NUM 17
#define U1CTS_GPIO_NUM 20
#define U1RTS_GPIO_NUM 19
#define U1DTR_GPIO_NUM (-1)
#define U1DSR_GPIO_NUM (-1)

#define U2RXD_GPIO_NUM (-1)
#define U2TXD_GPIO_NUM (-1)
#define U2CTS_GPIO_NUM (-1)
#define U2RTS_GPIO_NUM (-1)
#define U2DTR_GPIO_NUM (-1)
#define U2DSR_GPIO_NUM (-1)

/* The following defines are necessary for reconfiguring the UART
 * to use IOMUX, at runtime. */
#define U0TXD_MUX_FUNC  (0)
#define U0RXD_MUX_FUNC  (0)
#define U0RTS_MUX_FUNC  (2)
#define U0CTS_MUX_FUNC  (2)
#define U0DTR_MUX_FUNC  (-1)
#define U0DSR_MUX_FUNC  (-1)

#define U1TXD_MUX_FUNC  (2)
#define U1RXD_MUX_FUNC  (2)
#define U1RTS_MUX_FUNC  (2)
#define U1CTS_MUX_FUNC  (2)
#define U1DTR_MUX_FUNC  (-1)
#define U1DSR_MUX_FUNC  (-1)

/* UART2 cannot be used directly through the IOMUX, these value
 * shall not be used. */
#define U2TXD_MUX_FUNC  (-1)
#define U2RXD_MUX_FUNC  (-1)
#define U2RTS_MUX_FUNC  (-1)
#define U2CTS_MUX_FUNC  (-1)
#define U2DTR_MUX_FUNC  (-1)
#define U2DSR_MUX_FUNC  (-1)
