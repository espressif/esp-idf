
/*
 * SPDX-FileCopyrightText: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _UART_SELECT_H_
#define _UART_SELECT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "driver/uart.h"

typedef enum {
    UART_SELECT_READ_NOTIF,
    UART_SELECT_WRITE_NOTIF,
    UART_SELECT_ERROR_NOTIF,
} uart_select_notif_t;

typedef void (*uart_select_notif_callback_t)(uart_port_t uart_num, uart_select_notif_t uart_select_notif, BaseType_t *task_woken);

/**
 * @brief Set notification callback function for select() events
 * @param uart_num UART port number
 * @param uart_select_notif_callback callback function
 */
void uart_set_select_notif_callback(uart_port_t uart_num, uart_select_notif_callback_t uart_select_notif_callback);

/**
 * @brief Get mutex guarding select() notifications
 */
portMUX_TYPE *uart_get_selectlock(void);

#ifdef __cplusplus
}
#endif

#endif //_UART_SELECT_H_
