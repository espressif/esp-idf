/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#pragma once

#include "driver/uart.h"
#include "esp_check.h"
#include "esp_openthread_types.h"
#include "esp_zigbee.h"

#define ESP_ZIGBEE_ERROR_CHECK(err) ESP_ERROR_CHECK(esp_zigbee_err_to_esp(err))

/* Shared multipan-RCP UART parameters: keep OT and ZB in sync from a single
 * place to avoid drift between the two side-by-side configurations. */
#define OT_ZB_SHARED_UART_PORT      1
#define OT_ZB_SHARED_UART_BAUD      460800
#define OT_ZB_SHARED_UART_RX_PIN    4
#define OT_ZB_SHARED_UART_TX_PIN    5

#define ESP_ZIGBEE_PRIMARY_CHANNEL_MASK   (1U << 13)
#define ESP_ZIGBEE_SECONDARY_CHANNEL_MASK 0x07FFF800
#define ESP_ZIGBEE_CUSTOM_GATEWAY_EP_ID 1
#define ESP_ZIGBEE_MAX_CHILDREN 10
#define ESP_ZIGBEE_STORAGE_PARTITION_NAME "nvs"

#define ESP_MANUFACTURER_NAME "\x09""ESPRESSIF"
#define ESP_MODEL_IDENTIFIER "\x0e""OT_ZB_MULTIPAN"

#define ESP_MULTIPAN_DEFAULT_UART_RCP_CONFIG()            \
    {                                                     \
        .port = OT_ZB_SHARED_UART_PORT,                   \
        .uart_config =                                    \
            {                                             \
                .baud_rate = OT_ZB_SHARED_UART_BAUD,      \
                .data_bits = UART_DATA_8_BITS,            \
                .parity = UART_PARITY_DISABLE,            \
                .stop_bits = UART_STOP_BITS_1,            \
                .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,    \
                .rx_flow_ctrl_thresh = 0,                 \
                .source_clk = UART_SCLK_DEFAULT,          \
            },                                            \
        .rx_pin = OT_ZB_SHARED_UART_RX_PIN,               \
        .tx_pin = OT_ZB_SHARED_UART_TX_PIN,               \
    }

#define ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG()               \
    {                                                       \
        .radio_mode = RADIO_MODE_UART_RCP,                  \
        .radio_uart_config = ESP_MULTIPAN_DEFAULT_UART_RCP_CONFIG(), \
    }

#define ESP_OPENTHREAD_DEFAULT_HOST_CONFIG()                \
    {                                                       \
        .host_connection_mode = HOST_CONNECTION_MODE_NONE,  \
    }

#define ESP_OPENTHREAD_DEFAULT_PORT_CONFIG() \
    {                                        \
        .storage_partition_name = "nvs",     \
        .netif_queue_size = 10,              \
        .task_queue_size = 10,               \
    }

#define ESP_ZIGBEE_ZC_CONFIG()                          \
    {                                                   \
        .device_type = EZB_NWK_DEVICE_TYPE_COORDINATOR, \
        .install_code_policy = false,                   \
        .zczr_config = {                                \
            .max_children = ESP_ZIGBEE_MAX_CHILDREN,    \
        },                                              \
    }

#define ESP_ZIGBEE_PLATFORM_CONFIG()                                  \
    {                                                                 \
        .storage_partition_name = ESP_ZIGBEE_STORAGE_PARTITION_NAME,  \
        .radio_config = {                                             \
            .radio_mode = ESP_ZIGBEE_RADIO_MODE_UART_RCP,             \
            .radio_uart_config = ESP_MULTIPAN_DEFAULT_UART_RCP_CONFIG(), \
        },                                                            \
    }

#define ESP_ZIGBEE_DEFAULT_CONFIG()                      \
    {                                                    \
        .device_config = ESP_ZIGBEE_ZC_CONFIG(),         \
        .platform_config = ESP_ZIGBEE_PLATFORM_CONFIG(), \
    }
