// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _TCPIP_ADAPTER_TYPES_H_
#define _TCPIP_ADAPTER_TYPES_H_

#include "lwip/ip_addr.h"
#include "dhcpserver/dhcpserver.h"
#include "esp_netif_sta_list.h"

#ifdef __cplusplus
extern "C" {
#endif

//
// Define compatible types if tcpip_adapter interface used
//
#define TCPIP_ADAPTER_DHCP_STARTED ESP_NETIF_DHCP_STARTED
#define TCPIP_ADAPTER_DHCP_STOPPED ESP_NETIF_DHCP_STOPPED
#define TCPIP_ADAPTER_DHCP_INIT    ESP_NETIF_DHCP_INIT
#define TCPIP_ADAPTER_OP_SET       ESP_NETIF_OP_SET
#define TCPIP_ADAPTER_OP_GET       ESP_NETIF_OP_GET
#define TCPIP_ADAPTER_DOMAIN_NAME_SERVER             ESP_NETIF_DOMAIN_NAME_SERVER
#define TCPIP_ADAPTER_ROUTER_SOLICITATION_ADDRESS    ESP_NETIF_ROUTER_SOLICITATION_ADDRESS
#define TCPIP_ADAPTER_REQUESTED_IP_ADDRESS           ESP_NETIF_REQUESTED_IP_ADDRESS
#define TCPIP_ADAPTER_IP_ADDRESS_LEASE_TIME          ESP_NETIF_IP_ADDRESS_LEASE_TIME
#define TCPIP_ADAPTER_IP_REQUEST_RETRY_TIME          ESP_NETIF_IP_REQUEST_RETRY_TIME

/** @brief Legacy error code definitions
 *
 */
#define ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS        ESP_ERR_ESP_NETIF_INVALID_PARAMS
#define ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY          ESP_ERR_ESP_NETIF_IF_NOT_READY
#define ESP_ERR_TCPIP_ADAPTER_DHCPC_START_FAILED    ESP_ERR_ESP_NETIF_DHCPC_START_FAILED
#define ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STARTED  ESP_ERR_ESP_NETIF_DHCP_ALREADY_STARTED
#define ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STOPPED  ESP_ERR_ESP_NETIF_DHCP_ALREADY_STOPPED
#define ESP_ERR_TCPIP_ADAPTER_NO_MEM                ESP_ERR_ESP_NETIF_NO_MEM
#define ESP_ERR_TCPIP_ADAPTER_DHCP_NOT_STOPPED      ESP_ERR_ESP_NETIF_DHCP_NOT_STOPPED

typedef enum {
    TCPIP_ADAPTER_IF_STA = 0,     /**< Wi-Fi STA (station) interface */
    TCPIP_ADAPTER_IF_AP,          /**< Wi-Fi soft-AP interface */
    TCPIP_ADAPTER_IF_ETH,         /**< Ethernet interface */
    TCPIP_ADAPTER_IF_TEST,        /**< tcpip stack test interface */
    TCPIP_ADAPTER_IF_MAX
} tcpip_adapter_if_t;

/** @brief legacy ip_info type
 */
typedef struct {
    ip4_addr_t ip;      /**< Interface IPV4 address */
    ip4_addr_t netmask; /**< Interface IPV4 netmask */
    ip4_addr_t gw;      /**< Interface IPV4 gateway address */
} tcpip_adapter_ip_info_t;

/** @brief legacy typedefs
 */
typedef esp_netif_dhcp_status_t tcpip_adapter_dhcp_status_t;
typedef dhcps_lease_t tcpip_adapter_dhcps_lease_t;
typedef esp_netif_dhcp_option_mode_t tcpip_adapter_dhcp_option_mode_t;
typedef esp_netif_dhcp_option_id_t tcpip_adapter_dhcp_option_id_t;
typedef esp_netif_dns_type_t tcpip_adapter_dns_type_t;
typedef esp_netif_dns_info_t tcpip_adapter_dns_info_t;
typedef esp_netif_sta_list_t tcpip_adapter_sta_list_t;
typedef esp_netif_sta_info_t tcpip_adapter_sta_info_t;

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _TCPIP_ADAPTER_TYPES_H_
