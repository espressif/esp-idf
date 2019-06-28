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

#ifndef _TCPIP_ADAPTER_H_
#define _TCPIP_ADAPTER_H_

#include "esp_netif.h"
#include "lwip/ip_addr.h"
#include "dhcpserver/dhcpserver.h"


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

/**
 * @brief tcpip adapter legacy init. It is used only to set the compatibility mode of esp-netif, which
 * will enable backward compatibility of esp-netif.
 */
void tcpip_adapter_init(void);

/**
 * @brief Compatiblity mode: convert the esp-netif handle to tcpip_adapter legacy interface enum
 *
 * @param esp_netif
 *
 * @return corresponding interface if valid or known esp_netif provided, TCPIP_ADAPTER_IF_MAX otherwise
 */
tcpip_adapter_if_t tcpip_adapter_if_from_esp_netif(esp_netif_t *esp_netif);

/**
 * @brief Translates to esp_netif_get_ip_info
 *
 * @param tcpip_if Interface type corresponding to appropriate instance of esp-netif
 * @param ip_info See esp_netif_get_ip_info
 * @return See esp_netif_get_ip_info
 */
esp_err_t tcpip_adapter_get_ip_info(tcpip_adapter_if_t tcpip_if, tcpip_adapter_ip_info_t *ip_info);

/**
 * @brief Translates to esp_netif_get_ip6_linklocal
 *
 * @param tcpip_if  Interface type corresponding to appropriate instance of esp-netif
 * @param if_ip6 See esp_netif_get_ip6_linklocal
 * @return See esp_netif_get_ip6_linklocal
 */
esp_err_t tcpip_adapter_get_ip6_linklocal(tcpip_adapter_if_t tcpip_if, ip6_addr_t *if_ip6);

/**
 * @brief`Translates to esp_netif_dhcpc_get_status
 * @param tcpip_if Interface type corresponding to appropriate instance of esp-netif
 * @param status
 * @return See esp_netif_dhcpc_get_status
 */
esp_err_t tcpip_adapter_dhcpc_get_status(tcpip_adapter_if_t tcpip_if, tcpip_adapter_dhcp_status_t *status);

/**
 * @brief Translates to esp_netif_is_netif_up
 * @param tcpip_if Interface type corresponding to appropriate instance of esp-netif
 * @return see esp_netif_is_netif_up
 */
bool tcpip_adapter_is_netif_up(tcpip_adapter_if_t tcpip_if);

/**
 * @brief Translates to esp_netif_get_netif
 * @param tcpip_if Interface type corresponding to appropriate instance of esp-netif
 * @param netif
 * @return see esp_netif_get_netif
 */
esp_err_t tcpip_adapter_get_netif(tcpip_adapter_if_t tcpip_if, void ** netif);

/**
 * @brief Translates to esp_netif_create_ip6_linklocal
 * @param tcpip_if Interface type corresponding to appropriate instance of esp-netif
 * @return see esp_netif_create_ip6_linklocal
 */
esp_err_t tcpip_adapter_create_ip6_linklocal(tcpip_adapter_if_t tcpip_if);

/**
 * @brief Compatible version of setting ethernet default handlers
 * @note Compatible version of wifi handlers are provided in a separate header,
 * as this used to be called privately from wifi_init()
 * @return ESP_OK on success
 */
esp_err_t tcpip_adapter_set_default_eth_handlers(void);

/**
 * @brief Compatible version of network stack input function. Translates to esp_netif_receive()
 * @param buffer
 * @param len
 * @param eb
 * @return see esp_netif_receive
 */
esp_err_t tcpip_adapter_eth_input(void *buffer, uint16_t len, void *eb);

/**
 * @brief Compatible version of former tcpip_adapter API to clear default WIFI handlers
 * @return ESP_OK on success
 */
esp_err_t tcpip_adapter_clear_default_wifi_handlers(void);

esp_err_t tcpip_adapter_dhcps_stop(tcpip_adapter_if_t tcpip_if);
esp_err_t tcpip_adapter_dhcpc_stop(tcpip_adapter_if_t tcpip_if);
esp_err_t tcpip_adapter_dhcps_start(tcpip_adapter_if_t tcpip_if);
esp_err_t tcpip_adapter_dhcpc_start(tcpip_adapter_if_t tcpip_if);

esp_err_t tcpip_adapter_dhcps_get_status(tcpip_adapter_if_t tcpip_if, tcpip_adapter_dhcp_status_t *status);
esp_err_t tcpip_adapter_dhcps_option(tcpip_adapter_dhcp_option_mode_t opt_op, tcpip_adapter_dhcp_option_id_t opt_id, void *opt_val, uint32_t opt_len);
esp_err_t tcpip_adapter_dhcpc_option(tcpip_adapter_dhcp_option_mode_t opt_op, tcpip_adapter_dhcp_option_id_t opt_id, void *opt_val, uint32_t opt_len);
esp_err_t tcpip_adapter_set_ip_info(tcpip_adapter_if_t tcpip_if, const tcpip_adapter_ip_info_t *ip_info);
esp_err_t tcpip_adapter_get_dns_info(tcpip_adapter_if_t tcpip_if, tcpip_adapter_dns_type_t type, tcpip_adapter_dns_info_t *dns);
esp_err_t tcpip_adapter_set_dns_info(tcpip_adapter_if_t tcpip_if, tcpip_adapter_dns_type_t type, tcpip_adapter_dns_info_t *dns);
int tcpip_adapter_get_netif_index(tcpip_adapter_if_t tcpip_if);
esp_err_t tcpip_adapter_get_sta_list(const wifi_sta_list_t *wifi_sta_list, tcpip_adapter_sta_list_t *tcpip_sta_list);

#endif //_TCPIP_ADAPTER_H_
