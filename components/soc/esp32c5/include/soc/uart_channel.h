/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// This file defines GPIO lookup macros for available UART IO_MUX pins on ESP32C6.

#pragma once

//UART channels
#define UART_GPIO11_DIRECT_CHANNEL      UART_NUM_0
#define UART_NUM_0_TXD_DIRECT_GPIO_NUM  11
#define UART_GPIO12_DIRECT_CHANNEL      UART_NUM_0
#define UART_NUM_0_RXD_DIRECT_GPIO_NUM  12

#define UART_TXD_GPIO11_DIRECT_CHANNEL  UART_GPIO11_DIRECT_CHANNEL
#define UART_RXD_GPIO12_DIRECT_CHANNEL  UART_GPIO12_DIRECT_CHANNEL
