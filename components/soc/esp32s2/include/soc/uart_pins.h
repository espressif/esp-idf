/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/io_mux_reg.h"

/* Specify the number of pins for UART */
#define SOC_UART_PINS_COUNT  (4)

/* Specify the GPIO pin number for each UART signal in the IOMUX */
#define U0RXD_GPIO_NUM  (44)
#define U0TXD_GPIO_NUM  (43)
#define U0RTS_GPIO_NUM  (15)
#define U0CTS_GPIO_NUM  (16)

#define U1RXD_GPIO_NUM  (18)
#define U1TXD_GPIO_NUM  (17)
#define U1RTS_GPIO_NUM  (19)
#define U1CTS_GPIO_NUM  (20)

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
