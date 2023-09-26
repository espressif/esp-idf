/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 *
 * OpenThread Command Line Example
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */

#pragma once

#include "esp_openthread_types.h"

#if SOC_IEEE802154_SUPPORTED
#define ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG()              \
    {                                                      \
        .radio_mode = RADIO_MODE_NATIVE,                   \
    }

#else
#define ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG()                 \
    {                                                         \
        .radio_mode = RADIO_MODE_UART_RCP,                    \
        .radio_uart_config = {                                \
            .port = 1,                                        \
            .uart_config =                                    \
                {                                             \
                    .baud_rate = 115200,                      \
                    .data_bits = UART_DATA_8_BITS,            \
                    .parity = UART_PARITY_DISABLE,            \
                    .stop_bits = UART_STOP_BITS_1,            \
                    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,    \
                    .rx_flow_ctrl_thresh = 0,                 \
                    .source_clk = UART_SCLK_DEFAULT,          \
                },                                            \
            .rx_pin = 4,                                      \
            .tx_pin = 5,                                      \
        },                                                    \
    }
#endif

#if CONFIG_OPENTHREAD_CONSOLE_TYPE_UART
#define ESP_OPENTHREAD_DEFAULT_HOST_CONFIG()                    \
    {                                                           \
        .host_connection_mode = HOST_CONNECTION_MODE_CLI_UART,  \
        .host_uart_config = {                                   \
            .port = 0,                                          \
            .uart_config =                                      \
                {                                               \
                    .baud_rate = 115200,                        \
                    .data_bits = UART_DATA_8_BITS,              \
                    .parity = UART_PARITY_DISABLE,              \
                    .stop_bits = UART_STOP_BITS_1,              \
                    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,      \
                    .rx_flow_ctrl_thresh = 0,                   \
                    .source_clk = UART_SCLK_DEFAULT,            \
                },                                              \
            .rx_pin = UART_PIN_NO_CHANGE,                       \
            .tx_pin = UART_PIN_NO_CHANGE,                       \
        },                                                      \
    }
#elif CONFIG_OPENTHREAD_CONSOLE_TYPE_USB_SERIAL_JTAG
#define ESP_OPENTHREAD_DEFAULT_HOST_CONFIG()                        \
    {                                                               \
        .host_connection_mode = HOST_CONNECTION_MODE_CLI_USB,       \
        .host_usb_config = USB_SERIAL_JTAG_DRIVER_CONFIG_DEFAULT(), \
    }
#endif

#define ESP_OPENTHREAD_DEFAULT_PORT_CONFIG()    \
    {                                           \
        .storage_partition_name = "nvs",        \
        .netif_queue_size = 10,                 \
        .task_queue_size = 10,                  \
    }
