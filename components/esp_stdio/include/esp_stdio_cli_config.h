/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "sdkconfig.h"

#if CONFIG_ESP_CONSOLE_UART_DEFAULT || CONFIG_ESP_CONSOLE_UART_CUSTOM
#include "soc/uart_pins.h"
#if CONFIG_ESP_CONSOLE_UART_CUSTOM
#define ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT()       \
{                                                   \
    .channel = CONFIG_ESP_CONSOLE_UART_NUM,         \
    .baud_rate = CONFIG_ESP_CONSOLE_UART_BAUDRATE,  \
    .tx_gpio_num = (CONFIG_ESP_CONSOLE_UART_TX_GPIO >= 0) ? CONFIG_ESP_CONSOLE_UART_TX_GPIO : U0TXD_GPIO_NUM, \
    .rx_gpio_num = (CONFIG_ESP_CONSOLE_UART_RX_GPIO >= 0) ? CONFIG_ESP_CONSOLE_UART_RX_GPIO : U0RXD_GPIO_NUM, \
}
#else
#define ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT()      \
{                                                  \
    .channel = CONFIG_ESP_CONSOLE_UART_NUM,        \
    .baud_rate = CONFIG_ESP_CONSOLE_UART_BAUDRATE, \
    .tx_gpio_num = -1,                             \
    .rx_gpio_num = -1,                             \
}
#endif // CONFIG_ESP_CONSOLE_UART_CUSTOM
#endif // CONFIG_ESP_CONSOLE_UART_DEFAULT || CONFIG_ESP_CONSOLE_UART_CUSTOM

#if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG || (defined __DOXYGEN__ && SOC_USB_SERIAL_JTAG_SUPPORTED)
#define ESP_CONSOLE_DEV_USB_SERIAL_JTAG_CONFIG_DEFAULT() {}
#endif // CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG || (defined __DOXYGEN__ && SOC_USB_SERIAL_JTAG_SUPPORTED)

#if CONFIG_ESP_CONSOLE_USB_CDC || (defined __DOXYGEN__ && SOC_USB_OTG_SUPPORTED)
#define ESP_CONSOLE_DEV_CDC_CONFIG_DEFAULT() {}
#endif // CONFIG_ESP_CONSOLE_USB_CDC || (defined __DOXYGEN__ && SOC_USB_OTG_SUPPORTED)

#if CONFIG_IDF_TARGET_LINUX
#define ESP_CONSOLE_DEV_LINUX_CONFIG_DEFAULT() {}
#endif // CONFIG_IDF_TARGET_LINUX

#ifdef __cplusplus
}
#endif
