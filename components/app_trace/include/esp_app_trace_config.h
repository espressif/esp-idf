/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ESP_APP_TRACE_CONFIG_H_
#define ESP_APP_TRACE_CONFIG_H_

#include "sdkconfig.h"

/* Default configurations for runtime selection (APPTRACE_DEST_ALL)
 * These values are used when building with both JTAG and UART enabled
 * to allow runtime selection. You can switch between destinations
 * via esp_apptrace_get_user_params(). If this function is
 * not provided by the application, JTAG is used by default with the
 * configuration defined below. See esp_app_trace.h for details.
 */

#if !defined(CONFIG_APPTRACE_UART_TX_GPIO) || !defined(CONFIG_APPTRACE_UART_RX_GPIO)
#include "soc/uart_pins.h"
#endif

#ifndef CONFIG_APPTRACE_BUF_SIZE
#define CONFIG_APPTRACE_BUF_SIZE 16384
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

#ifndef CONFIG_APPTRACE_UART_TX_GPIO
#define CONFIG_APPTRACE_UART_TX_GPIO U1TXD_GPIO_NUM
#endif

#ifndef CONFIG_APPTRACE_UART_RX_GPIO
#define CONFIG_APPTRACE_UART_RX_GPIO U1RXD_GPIO_NUM
#endif

#ifndef CONFIG_APPTRACE_DEST_UART_NUM
#define CONFIG_APPTRACE_DEST_UART_NUM 1
#endif

#endif /* ESP_APP_TRACE_CONFIG_H_ */
