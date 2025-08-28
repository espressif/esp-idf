/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ESP_APP_TRACE_CONFIG_H_
#define ESP_APP_TRACE_CONFIG_H_

#include "sdkconfig.h"

/* Default configurations for runtime selection (CONFIG_APPTRACE_DEST_NONE)
 * These values are used when CONFIG_APPTRACE_DEST_NONE is selected in menuconfig.
 * To customize at runtime, implement esp_apptrace_get_user_params()
 * in your application. See esp_app_trace.h for details.
 */

#if !defined(CONFIG_APPTRACE_UART_TX_GPIO) || !defined(CONFIG_APPTRACE_UART_RX_GPIO)
#include "soc/uart_pins.h"
#endif

#ifndef CONFIG_APPTRACE_BUF_SIZE
#define CONFIG_APPTRACE_BUF_SIZE 16384
#endif

#ifndef CONFIG_APPTRACE_UART_RX_BUFF_SIZE
#define CONFIG_APPTRACE_UART_RX_BUFF_SIZE 128
#endif

#ifndef CONFIG_APPTRACE_UART_TX_BUFF_SIZE
#define CONFIG_APPTRACE_UART_TX_BUFF_SIZE 4096
#endif

#ifndef CONFIG_APPTRACE_UART_TX_MSG_SIZE
#define CONFIG_APPTRACE_UART_TX_MSG_SIZE 128
#endif

#ifndef CONFIG_APPTRACE_UART_BAUDRATE
#define CONFIG_APPTRACE_UART_BAUDRATE 1000000
#endif

#ifndef CONFIG_APPTRACE_UART_TASK_PRIO
#define CONFIG_APPTRACE_UART_TASK_PRIO 1
#endif

#ifndef CONFIG_APPTRACE_UART_TX_GPIO
#define CONFIG_APPTRACE_UART_TX_GPIO U1TXD_GPIO_NUM
#endif

#ifndef CONFIG_APPTRACE_UART_RX_GPIO
#define CONFIG_APPTRACE_UART_RX_GPIO U1RXD_GPIO_NUM
#endif

#ifndef CONFIG_APPTRACE_DEST_UART_NUM
#define CONFIG_APPTRACE_DEST_UART_NUM 1
#endif

#ifndef CONFIG_APPTRACE_SV_DEST_CPU_0
#define CONFIG_APPTRACE_SV_DEST_CPU_0 1
#endif

#endif /* ESP_APP_TRACE_CONFIG_H_ */
