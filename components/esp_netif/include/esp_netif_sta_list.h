// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
