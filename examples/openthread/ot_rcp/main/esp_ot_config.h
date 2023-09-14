/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 *
 * OpenThread Radio Co-Processor (RCP) Example
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */

#pragma once

#if CONFIG_EXTERNAL_COEX_ENABLE
#include "esp_coexist.h"
#endif

#include "esp_openthread_types.h"
#define ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG()                   \
    {                                                           \
        .radio_mode = RADIO_MODE_NATIVE,                        \
    }

#if CONFIG_OPENTHREAD_RCP_UART
#if CONFIG_OPENTHREAD_UART_PIN_MANUAL
#define OPENTHREAD_RCP_UART_RX_PIN CONFIG_OPENTHREAD_UART_RX_PIN
#define OPENTHREAD_RCP_UART_TX_PIN CONFIG_OPENTHREAD_UART_TX_PIN
#else
#define OPENTHREAD_RCP_UART_RX_PIN UART_PIN_NO_CHANGE
#define OPENTHREAD_RCP_UART_TX_PIN UART_PIN_NO_CHANGE
#endif

#define ESP_OPENTHREAD_DEFAULT_HOST_CONFIG()                    \
    {                                                           \
        .host_connection_mode = HOST_CONNECTION_MODE_RCP_UART,  \
        .host_uart_config = {                                   \
            .port = 0,                                          \
            .uart_config =                                      \
                {                                               \
                    .baud_rate =  460800,                       \
                    .data_bits = UART_DATA_8_BITS,              \
                    .parity = UART_PARITY_DISABLE,              \
                    .stop_bits = UART_STOP_BITS_1,              \
                    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,      \
                    .rx_flow_ctrl_thresh = 0,                   \
                    .source_clk = UART_SCLK_DEFAULT,            \
                },                                              \
            .rx_pin = OPENTHREAD_RCP_UART_RX_PIN,               \
            .tx_pin = OPENTHREAD_RCP_UART_TX_PIN,               \
        },                                                      \
    }
#else // CONFIG_OPENTHREAD_RCP_SPI
#define ESP_OPENTHREAD_DEFAULT_HOST_CONFIG()                    \
    {                                                           \
        .host_connection_mode = HOST_CONNECTION_MODE_RCP_SPI,   \
        .spi_slave_config = {                                   \
            .host_device = SPI2_HOST,                           \
            .bus_config = {                                     \
                .mosi_io_num = 3,                               \
                .miso_io_num = 1,                               \
                .sclk_io_num = 0,                               \
                .quadhd_io_num = -1,                            \
                .quadwp_io_num = -1,                            \
                .isr_cpu_id = ESP_INTR_CPU_AFFINITY_AUTO,       \
            },                                                  \
            .slave_config = {                                   \
                .mode = 0,                                      \
                .spics_io_num = 2,                              \
                .queue_size = 3,                                \
                .flags = 0,                                     \
            },                                                  \
            .intr_pin = 9,                                      \
        },                                                      \
    }
#endif

#define ESP_OPENTHREAD_DEFAULT_PORT_CONFIG()    \
    {                                           \
        .storage_partition_name = "nvs",        \
        .netif_queue_size = 10,                 \
        .task_queue_size = 10,                  \
    }

#if CONFIG_EXTERNAL_COEX_ENABLE
#if CONFIG_EXTERNAL_COEX_WIRE_TYPE == EXTERNAL_COEXIST_WIRE_1
#define ESP_OPENTHREAD_DEFAULT_EXTERNAL_COEX_CONFIG()   \
    {                                                   \
        .request = CONFIG_EXTERNAL_COEX_REQUEST_PIN,    \
    }
#elif CONFIG_EXTERNAL_COEX_WIRE_TYPE == EXTERNAL_COEXIST_WIRE_2
#define ESP_OPENTHREAD_DEFAULT_EXTERNAL_COEX_CONFIG()   \
    {                                                   \
        .request = CONFIG_EXTERNAL_COEX_REQUEST_PIN,    \
        .grant = CONFIG_EXTERNAL_COEX_GRANT_PIN,        \
    }
#elif CONFIG_EXTERNAL_COEX_WIRE_TYPE == EXTERNAL_COEXIST_WIRE_3
#define ESP_OPENTHREAD_DEFAULT_EXTERNAL_COEX_CONFIG()   \
    {                                                   \
        .request = CONFIG_EXTERNAL_COEX_REQUEST_PIN,    \
        .priority = CONFIG_EXTERNAL_COEX_PRIORITY_PIN,  \
        .grant = CONFIG_EXTERNAL_COEX_GRANT_PIN,        \
    }
#elif CONFIG_EXTERNAL_COEX_WIRE_TYPE == EXTERNAL_COEXIST_WIRE_4
#define ESP_OPENTHREAD_DEFAULT_EXTERNAL_COEX_CONFIG()   \
    {                                                   \
        .request = CONFIG_EXTERNAL_COEX_REQUEST_PIN,    \
        .priority = CONFIG_EXTERNAL_COEX_PRIORITY_PIN,  \
        .grant = CONFIG_EXTERNAL_COEX_GRANT_PIN,        \
        .tx_line = CONFIG_EXTERNAL_COEX_TX_LINE_PIN,    \
    }
#endif
#endif // CONFIG_EXTERNAL_COEX_ENABLE
