/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_NETIF_STA_LIST_H_
#define _ESP_NETIF_STA_LIST_H_

#include "esp_netif_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief station list info element
 */
typedef struct {
    uint8_t mac[6]; /**< Station MAC address */
    esp_ip4_addr_t ip;  /**< Station assigned IP address */
} esp_netif_sta_info_t;

/**
 * @brief station list structure
 */
typedef struct {
    esp_netif_sta_info_t sta[ESP_WIFI_MAX_CONN_NUM]; /**< Connected stations */
    int num; /**< Number of connected stations */
} esp_netif_sta_list_t;

/**
 * @defgroup ESP_NETIF_STA_LIST ESP-NETIF STA list api
 * @brief List of stations for Wi-Fi AP interface
 *
 */

/** @addtogroup ESP_NETIF_STA_LIST
 * @{
 */

/**
 * @brief  Get IP information for stations connected to the Wi-Fi AP interface
 *
 * @warning This API works only for the default Wi-Fi AP interface, i.e. esp-netif with key="WIFI_AP_DEF"
 *
 * @param[in]   wifi_sta_list Wi-Fi station info list, returned from esp_wifi_ap_get_sta_list()
 * @param[out]  netif_sta_list IP layer station info list, corresponding to MAC addresses provided in wifi_sta_list
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_ESP_NETIF_NO_MEM
 *         - ESP_ERR_ESP_NETIF_INVALID_PARAMS
 */
esp_err_t esp_netif_get_sta_list(const wifi_sta_list_t *wifi_sta_list, esp_netif_sta_list_t *netif_sta_list);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif //_ESP_NETIF_STA_LIST_H_
