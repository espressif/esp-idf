/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_netif_types.h"
#include "esp_wifi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

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
