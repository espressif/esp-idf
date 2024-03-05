/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// This file defines GPIO lookup macros for available UART IO_MUX pins on ESP32C6.

#pragma once

// TODO: [ESP32C5] IDF-8722

//UART channels
#define UART_GPIO16_DIRECT_CHANNEL      UART_NUM_0
#define UART_NUM_0_TXD_DIRECT_GPIO_NUM  16
#define UART_GPIO17_DIRECT_CHANNEL      UART_NUM_0
#define UART_NUM_0_RXD_DIRECT_GPIO_NUM  17

#define UART_TXD_GPIO16_DIRECT_CHANNEL  UART_GPIO16_DIRECT_CHANNEL
#define UART_RXD_GPIO17_DIRECT_CHANNEL  UART_GPIO17_DIRECT_CHANNEL
