/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_netif_types.h"
#include "esp_wifi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ESP_WIFI_MAX_CONN_NUM
// Number of maximum wifi connection may be undefined if we have no native wifi support on this target
//   and at the same time there's no native interface injected by the wifi_remote component.
// In this case, we just let the header compilable, since no wifi API could be used (let's make a sanity check)
#if !CONFIG_SOC_WIFI_SUPPORTED && !CONFIG_ESP_WIFI_REMOTE_ENABLED && !CONFIG_ESP_HOST_WIFI_ENABLED
#define ESP_WIFI_MAX_CONN_NUM  (15)
typedef struct wifi_sta_list_t wifi_sta_list_t;
#else
#error WiFi header mismatch! Please make sure you use the correct version of WiFi API
#endif
#endif // ESP_WIFI_MAX_CONN_NUM

/**
 * @brief station list structure
 */
typedef struct {
    int num;                                                /**< Number of connected stations */
    esp_netif_pair_mac_ip_t sta[ESP_WIFI_MAX_CONN_NUM];     /**< Connected stations */
} wifi_sta_mac_ip_list_t;

/**
 * @brief  Get IP information for stations connected to the Wi-Fi AP interface
 *
 * @note   If `CONFIG_LWIP_DHCPS` is disabled then `ip` address field will not be populated in sta list
 *
 * @warning This API works only for the default Wi-Fi AP interface, i.e. esp-netif with key="WIFI_AP_DEF"
 *
 * @param[in]   wifi_sta_list Wi-Fi station info list, returned from esp_wifi_ap_get_sta_list()
 * @param[out]  wifi_sta_ip_mac_list IP layer station info list, corresponding to MAC addresses provided in wifi_sta_list
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_ESP_NETIF_NO_MEM
 *         - ESP_ERR_ESP_NETIF_INVALID_PARAMS
 */
esp_err_t esp_wifi_ap_get_sta_list_with_ip(const wifi_sta_list_t *wifi_sta_list, wifi_sta_mac_ip_list_t *wifi_sta_ip_mac_list);

#ifdef __cplusplus
}
#endif
