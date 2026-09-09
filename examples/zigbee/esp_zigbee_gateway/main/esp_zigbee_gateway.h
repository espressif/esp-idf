/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Included
 *
 * Zigbee Gateway Example
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */

#pragma once

#define ESP_ZIGBEE_PRIMARY_CHANNEL_MASK   (1U << CONFIG_ZB_EXAMPLE_PRIMARY_CHANNEL)
#define ESP_ZIGBEE_SECONDARY_CHANNEL_MASK CONFIG_ZB_EXAMPLE_SECONDARY_CHANNEL_MASK

#define ESP_ZIGBEE_CUSTOM_GATEWAY_EP_ID (10)

#define ESP_ZIGBEE_STORAGE_PARTITION_NAME "nvs"

#define ESP_MANUFACTURER_NAME "\x09""ESPRESSIF"
#define ESP_MODEL_IDENTIFIER "\x07"CONFIG_IDF_TARGET

#if defined(CONFIG_ZB_GW_RCP_CHIP_ESP32C6)
#define ESP_ZIGBEE_RCP_TARGET_CHIP ESP32C6_CHIP
#elif defined(CONFIG_ZB_GW_RCP_CHIP_ESP32H2)
#define ESP_ZIGBEE_RCP_TARGET_CHIP ESP32H2_CHIP
#else
#define ESP_ZIGBEE_RCP_TARGET_CHIP ESP_UNKNOWN_CHIP
#endif

#define ESP_ZIGBEE_ZC_CONFIG()                          \
    {                                                   \
        .device_type = EZB_NWK_DEVICE_TYPE_COORDINATOR, \
        .install_code_policy = false,                   \
        .zczr_config = {                                \
            .max_children = 10,                         \
        },                                              \
    }

#if CONFIG_SOC_IEEE802154_SUPPORTED
#define ESP_ZIGBEE_PLATFORM_CONFIG()                                 \
    {                                                                \
        .storage_partition_name = ESP_ZIGBEE_STORAGE_PARTITION_NAME, \
        .radio_config = {                                            \
            .radio_mode = ESP_ZIGBEE_RADIO_MODE_NATIVE,              \
        },                                                           \
    }
#else
#define ESP_ZIGBEE_UART_CONFIG()                     \
  {                                                  \
      .port = 1,                                     \
      .uart_config =                                 \
          {                                          \
              .baud_rate = 460800,                   \
              .data_bits = UART_DATA_8_BITS,         \
              .parity = UART_PARITY_DISABLE,         \
              .stop_bits = UART_STOP_BITS_1,         \
              .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, \
              .rx_flow_ctrl_thresh = 0,              \
              .source_clk = UART_SCLK_DEFAULT,       \
          },                                         \
      .rx_pin = CONFIG_DEFAULT_PIN_TO_RCP_TX,        \
      .tx_pin = CONFIG_DEFAULT_PIN_TO_RCP_RX,        \
  }

#define ESP_ZIGBEE_PLATFORM_CONFIG()                                 \
    {                                                                \
        .storage_partition_name = ESP_ZIGBEE_STORAGE_PARTITION_NAME, \
        .radio_config = {                                            \
            .radio_mode = ESP_ZIGBEE_RADIO_MODE_UART_RCP,            \
            .radio_uart_config = ESP_ZIGBEE_UART_CONFIG(),           \
        },                                                           \
    }
#endif

#define ESP_ZIGBEE_DEFAULT_CONFIG()                      \
    {                                                    \
        .device_config = ESP_ZIGBEE_ZC_CONFIG(),         \
        .platform_config = ESP_ZIGBEE_PLATFORM_CONFIG(), \
    };

#define ESP_ZIGBEE_RCP_CONFIG()                                \
  {                                                            \
      .rcp_type = RCP_TYPE_UART,                               \
      .uart_rx_pin = CONFIG_DEFAULT_PIN_TO_RCP_TX,             \
      .uart_tx_pin = CONFIG_DEFAULT_PIN_TO_RCP_RX,             \
      .uart_port = 1,                                          \
      .uart_baudrate = 115200,                                 \
      .reset_pin = CONFIG_DEFAULT_PIN_TO_RCP_RESET,            \
      .boot_pin = CONFIG_DEFAULT_PIN_TO_RCP_BOOT,              \
      .update_baudrate = 460800,                               \
      .firmware_dir = "/" CONFIG_RCP_PARTITION_NAME "/ot_rcp", \
      .target_chip = ESP_ZIGBEE_RCP_TARGET_CHIP,               \
  }
