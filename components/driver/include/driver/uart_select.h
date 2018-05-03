
// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
portMUX_TYPE *uart_get_selectlock();

#ifdef __cplusplus
}
#endif

#endif //_UART_SELECT_H_
