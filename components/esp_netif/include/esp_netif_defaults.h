// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _ESP_NETIF_DEFAULTS_H
#define _ESP_NETIF_DEFAULTS_H

//
// Macros to assemble master configs with partial configs from netif, stack and driver
//

/**
 * @brief  Default configuration reference of ethernet interface
 */
#define ESP_NETIF_DEFAULT_ETH()                  \
    {                                            \
        .base = ESP_NETIF_BASE_DEFAULT_ETH,      \
        .stack = ESP_NETIF_NETSTACK_DEFAULT_ETH, \
        .driver = NULL,  \
    }

/**
 * @brief  Default configuration reference of WIFI AP
 */
#define ESP_NETIF_DEFAULT_WIFI_AP()                  \
{                                                    \
        .base = ESP_NETIF_BASE_DEFAULT_WIFI_AP,      \
        .stack = ESP_NETIF_NETSTACK_DEFAULT_WIFI_AP, \
        .driver = NULL,  \
    }

/**
* @brief  Default configuration reference of WIFI STA
*/
#define ESP_NETIF_DEFAULT_WIFI_STA()                  \
    {                                                 \
        .base = ESP_NETIF_BASE_DEFAULT_WIFI_STA,      \
        .stack = ESP_NETIF_NETSTACK_DEFAULT_WIFI_STA, \
        .driver = NULL,  \
    }
/**
 * @brief  Default base config (esp-netif inherent) of WIFI STA
 */
#define ESP_NETIF_BASE_DEFAULT_WIFI_STA        &_g_esp_netif_inherent_sta_config

/**
 * @brief  Default base config (esp-netif inherent) of WIFI AP
 */
#define ESP_NETIF_BASE_DEFAULT_WIFI_AP         &_g_esp_netif_inherent_ap_config

/**
 * @brief  Default base config (esp-netif inherent) of ethernet interface
 */
#define ESP_NETIF_BASE_DEFAULT_ETH             &_g_esp_netif_inherent_eth_config

#define ESP_NETIF_NETSTACK_DEFAULT_ETH          _g_esp_netif_netstack_default_eth
#define ESP_NETIF_NETSTACK_DEFAULT_WIFI_STA     _g_esp_netif_netstack_default_wifi_sta
#define ESP_NETIF_NETSTACK_DEFAULT_WIFI_AP      _g_esp_netif_netstack_default_wifi_ap
//
// Include default network stacks configs
//  - Network stack configurations are provided in a specific network stack
//      implementation that is invisible to user API
//  - Here referenced only as opaque pointers
//
extern const esp_netif_netstack_config_t *_g_esp_netif_netstack_default_eth;
extern const esp_netif_netstack_config_t *_g_esp_netif_netstack_default_wifi_sta;
extern const esp_netif_netstack_config_t *_g_esp_netif_netstack_default_wifi_ap;

//
// Include default common configs inherent to esp-netif
//  - These inherent configs are defined in esp_netif_defaults.c and describe
//    common behavioural patters for common interfaces such as STA, AP, ETH
//
extern const esp_netif_inherent_config_t _g_esp_netif_inherent_sta_config;
extern const esp_netif_inherent_config_t _g_esp_netif_inherent_ap_config;
extern const esp_netif_inherent_config_t _g_esp_netif_inherent_eth_config;

#endif //_ESP_NETIF_DEFAULTS_H
