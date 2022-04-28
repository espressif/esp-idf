/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef __CONSOLE_UART_H__
#define __CONSOLE_UART_H__

#define TAG_CNSL "CNSL"

/**
 * @brief     configure uart console for command input and process
 */
esp_err_t console_uart_init(void);

#endif /* __CONSOLE_UART_H__ */
