/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#ifndef __ESP_INTERFACE_H__
#define __ESP_INTERFACE_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ESP_IF_WIFI_STA = 0,     /**< Station interface */
    ESP_IF_WIFI_AP,          /**< Soft-AP interface */
    ESP_IF_WIFI_NAN,         /**< NAN interface */
    ESP_IF_ETH,              /**< Ethernet interface */
    ESP_IF_MAX
} esp_interface_t;

#ifdef __cplusplus
}
#endif


#endif /* __ESP_INTERFACE_TYPES_H__ */
