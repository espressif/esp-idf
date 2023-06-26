/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// This file defines GPIO lookup macros for available UART IO_MUX pins on ESP32H2.

#pragma once

//UART channels
#define UART_GPIO24_DIRECT_CHANNEL      UART_NUM_0
#define UART_NUM_0_TXD_DIRECT_GPIO_NUM  24
#define UART_GPIO23_DIRECT_CHANNEL      UART_NUM_0
#define UART_NUM_0_RXD_DIRECT_GPIO_NUM  23

#define UART_TXD_GPIO24_DIRECT_CHANNEL  UART_GPIO24_DIRECT_CHANNEL
#define UART_RXD_GPIO23_DIRECT_CHANNEL  UART_GPIO23_DIRECT_CHANNEL
