/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Included
 *
 * Zigbee HA_on_off_switch Example
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

#define ESP_ZIGBEE_HA_ON_OFF_SWITCH_EP_ID (1)

#define ESP_ZIGBEE_STORAGE_PARTITION_NAME "nvs"

#define ESP_MANUFACTURER_NAME "\x09""ESPRESSIF"
#define ESP_MODEL_IDENTIFIER  "\x07" CONFIG_IDF_TARGET

#define ESP_ZIGBEE_ZED_CONFIG()                         \
    {                                                   \
        .device_type = EZB_NWK_DEVICE_TYPE_END_DEVICE,  \
        .install_code_policy = false,                   \
        .zed_config = {                                 \
            .ed_timeout = EZB_NWK_ED_TIMEOUT_64MIN,     \
            .keep_alive = 4000,                         \
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
#warning "The example is not for IEEE 802.15.4-disabled SoC usage, please refer to esp_zigbee_gateway for RCP configuration"
#endif

#define ESP_ZIGBEE_DEFAULT_CONFIG()                      \
    {                                                    \
        .device_config = ESP_ZIGBEE_ZED_CONFIG(),        \
        .platform_config = ESP_ZIGBEE_PLATFORM_CONFIG(), \
    };
