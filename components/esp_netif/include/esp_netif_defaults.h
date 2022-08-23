/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_NETIF_DEFAULTS_H
#define _ESP_NETIF_DEFAULTS_H

#include "esp_compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

//
// Macros to assemble master configs with partial configs from netif, stack and driver
//

#define ESP_NETIF_INHERENT_DEFAULT_WIFI_STA() \
    {   \
        .flags = (esp_netif_flags_t)(ESP_NETIF_DHCP_CLIENT | ESP_NETIF_FLAG_GARP | ESP_NETIF_FLAG_EVENT_IP_MODIFIED), \
        ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_EMPTY(mac) \
        ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_EMPTY(ip_info) \
        .get_ip_event = IP_EVENT_STA_GOT_IP, \
        .lost_ip_event = IP_EVENT_STA_LOST_IP, \
        .if_key = "WIFI_STA_DEF", \
        .if_desc = "sta", \
        .route_prio = 100, \
        .bridge_info = NULL \
     }  \

#ifdef CONFIG_ESP_WIFI_SOFTAP_SUPPORT
#define ESP_NETIF_INHERENT_DEFAULT_WIFI_AP() \
    {   \
        .flags = (esp_netif_flags_t)(ESP_NETIF_DHCP_SERVER | ESP_NETIF_FLAG_AUTOUP), \
        ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_EMPTY(mac) \
        .ip_info = &_g_esp_netif_soft_ap_ip, \
        .get_ip_event = 0, \
        .lost_ip_event = 0, \
        .if_key = "WIFI_AP_DEF", \
        .if_desc = "ap", \
        .route_prio = 10, \
        .bridge_info = NULL \
    };
#endif

#define ESP_NETIF_INHERENT_DEFAULT_ETH() \
    {   \
        .flags = (esp_netif_flags_t)(ESP_NETIF_DHCP_CLIENT | ESP_NETIF_FLAG_GARP | ESP_NETIF_FLAG_EVENT_IP_MODIFIED), \
        ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_EMPTY(mac) \
        ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_EMPTY(ip_info) \
        .get_ip_event = IP_EVENT_ETH_GOT_IP, \
        .lost_ip_event = IP_EVENT_ETH_LOST_IP, \
        .if_key = "ETH_DEF", \
        .if_desc = "eth", \
        .route_prio = 50, \
        .bridge_info = NULL \
    };

#define ESP_NETIF_INHERENT_DEFAULT_PPP() \
    {   \
        .flags = ESP_NETIF_FLAG_IS_PPP, \
        ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_EMPTY(mac) \
        ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_EMPTY(ip_info) \
        .get_ip_event = IP_EVENT_PPP_GOT_IP,    \
        .lost_ip_event = IP_EVENT_PPP_LOST_IP,  \
        .if_key = "PPP_DEF",    \
        .if_desc = "ppp",   \
        .route_prio = 20,  \
        .bridge_info = NULL \
};




#define ESP_NETIF_INHERENT_DEFAULT_BR() \
    {   \
        .flags = (esp_netif_flags_t)(ESP_NETIF_DHCP_CLIENT | ESP_NETIF_FLAG_GARP | ESP_NETIF_FLAG_EVENT_IP_MODIFIED | ESP_NETIF_FLAG_IS_BRIDGE), \
        ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_EMPTY(mac) \
        ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_EMPTY(ip_info) \
        .get_ip_event = IP_EVENT_ETH_GOT_IP, \
        .lost_ip_event = IP_EVENT_ETH_LOST_IP, \
        .if_key = "BR", \
        .if_desc = "br", \
        .route_prio = 70, \
        .bridge_info = NULL \
    };

/**
 * @brief  Default configuration reference of ethernet interface
 */
#define ESP_NETIF_DEFAULT_ETH()                  \
    {                                            \
        .base = ESP_NETIF_BASE_DEFAULT_ETH,      \
        .driver = NULL,                          \
        .stack = ESP_NETIF_NETSTACK_DEFAULT_ETH, \
    }

#ifdef CONFIG_ESP_WIFI_SOFTAP_SUPPORT
/**
 * @brief  Default configuration reference of WIFI AP
 */
#define ESP_NETIF_DEFAULT_WIFI_AP()                  \
    {                                                \
        .base = ESP_NETIF_BASE_DEFAULT_WIFI_AP,      \
        .driver = NULL,                              \
        .stack = ESP_NETIF_NETSTACK_DEFAULT_WIFI_AP, \
    }
#endif

/**
* @brief  Default configuration reference of WIFI STA
*/
#define ESP_NETIF_DEFAULT_WIFI_STA()                  \
    {                                                 \
        .base = ESP_NETIF_BASE_DEFAULT_WIFI_STA,      \
        .driver = NULL,                               \
        .stack = ESP_NETIF_NETSTACK_DEFAULT_WIFI_STA, \
    }

/**
* @brief  Default configuration reference of PPP client
*/
#define ESP_NETIF_DEFAULT_PPP()                       \
    {                                                 \
        .base = ESP_NETIF_BASE_DEFAULT_PPP,           \
        .driver = NULL,                               \
        .stack = ESP_NETIF_NETSTACK_DEFAULT_PPP,      \
    }

/**
 * @brief  Default base config (esp-netif inherent) of WIFI STA
 */
#define ESP_NETIF_BASE_DEFAULT_WIFI_STA        &_g_esp_netif_inherent_sta_config

#ifdef CONFIG_ESP_WIFI_SOFTAP_SUPPORT
/**
 * @brief  Default base config (esp-netif inherent) of WIFI AP
 */
#define ESP_NETIF_BASE_DEFAULT_WIFI_AP         &_g_esp_netif_inherent_ap_config
#endif

/**
 * @brief  Default base config (esp-netif inherent) of ethernet interface
 */
#define ESP_NETIF_BASE_DEFAULT_ETH             &_g_esp_netif_inherent_eth_config

/**
 * @brief  Default base config (esp-netif inherent) of ppp interface
 */
#define ESP_NETIF_BASE_DEFAULT_PPP             &_g_esp_netif_inherent_ppp_config


#define ESP_NETIF_NETSTACK_DEFAULT_ETH          _g_esp_netif_netstack_default_eth
#define ESP_NETIF_NETSTACK_DEFAULT_BR           _g_esp_netif_netstack_default_br
#define ESP_NETIF_NETSTACK_DEFAULT_WIFI_STA     _g_esp_netif_netstack_default_wifi_sta
#ifdef CONFIG_ESP_WIFI_SOFTAP_SUPPORT
#define ESP_NETIF_NETSTACK_DEFAULT_WIFI_AP      _g_esp_netif_netstack_default_wifi_ap
#endif
#define ESP_NETIF_NETSTACK_DEFAULT_PPP          _g_esp_netif_netstack_default_ppp

//
// Include default network stacks configs
//  - Network stack configurations are provided in a specific network stack
//      implementation that is invisible to user API
//  - Here referenced only as opaque pointers
//
extern const esp_netif_netstack_config_t *_g_esp_netif_netstack_default_eth;
extern const esp_netif_netstack_config_t *_g_esp_netif_netstack_default_br;
extern const esp_netif_netstack_config_t *_g_esp_netif_netstack_default_wifi_sta;
#ifdef CONFIG_ESP_WIFI_SOFTAP_SUPPORT
extern const esp_netif_netstack_config_t *_g_esp_netif_netstack_default_wifi_ap;
#endif
extern const esp_netif_netstack_config_t *_g_esp_netif_netstack_default_ppp;

//
// Include default common configs inherent to esp-netif
//  - These inherent configs are defined in esp_netif_defaults.c and describe
//    common behavioural patterns for common interfaces such as STA, AP, ETH, PPP
//
extern const esp_netif_inherent_config_t _g_esp_netif_inherent_sta_config;
#ifdef CONFIG_ESP_WIFI_SOFTAP_SUPPORT
extern const esp_netif_inherent_config_t _g_esp_netif_inherent_ap_config;
#endif
extern const esp_netif_inherent_config_t _g_esp_netif_inherent_eth_config;
extern const esp_netif_inherent_config_t _g_esp_netif_inherent_ppp_config;

#ifdef CONFIG_ESP_WIFI_SOFTAP_SUPPORT
extern const esp_netif_ip_info_t _g_esp_netif_soft_ap_ip;
#endif

#ifdef __cplusplus
}
#endif

#endif //_ESP_NETIF_DEFAULTS_H
