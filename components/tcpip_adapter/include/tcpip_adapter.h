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
#ifndef _ESP_NETIF_SUPPRESS_LEGACY_WARNING_
#warning "This header is deprecated, please use new network related API in esp_netif.h"
#include "esp_netif.h"
#endif

#ifndef _TCPIP_ADAPTER_H_
#define _TCPIP_ADAPTER_H_

#include "esp_netif.h"

#include "tcpip_adapter_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief tcpip adapter legacy init. It is used only to set the compatibility mode of esp-netif, which
 * will enable backward compatibility of esp-netif.
 */
void tcpip_adapter_init(void)  __attribute__ ((deprecated));

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

#if CONFIG_LWIP_IPV6
/**
 * @brief Translates to esp_netif_get_ip6_linklocal
 *
 * @param tcpip_if  Interface type corresponding to appropriate instance of esp-netif
 * @param if_ip6 See esp_netif_get_ip6_linklocal
 * @return See esp_netif_get_ip6_linklocal
 */
esp_err_t tcpip_adapter_get_ip6_linklocal(tcpip_adapter_if_t tcpip_if, ip6_addr_t *if_ip6);

/**
 * @brief Translates to esp_netif_get_ip6_global
 *
 * @param tcpip_if  Interface type corresponding to appropriate instance of esp-netif
 * @param if_ip6 See esp_netif_get_ip6_global
 * @return See esp_netif_get_ip6_global
 */
esp_err_t tcpip_adapter_get_ip6_global(tcpip_adapter_if_t tcpip_if, ip6_addr_t *if_ip6);
#endif

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

#if CONFIG_LWIP_IPV6
/**
 * @brief Translates to esp_netif_create_ip6_linklocal
 * @param tcpip_if Interface type corresponding to appropriate instance of esp-netif
 * @return see esp_netif_create_ip6_linklocal
 */
esp_err_t tcpip_adapter_create_ip6_linklocal(tcpip_adapter_if_t tcpip_if);
#endif

/**
 * @brief Compatible version of setting ethernet default handlers
 * @note Compatible version of wifi handlers are provided in a separate header,
 * as this used to be called privately from wifi_init()
 * @return ESP_OK on success
 */
esp_err_t tcpip_adapter_set_default_eth_handlers(void);

/**
 * @brief Compatible version of network stack input function. Translates to esp_netif_receive()
 */
esp_err_t tcpip_adapter_eth_input(void *buffer, uint16_t len, void *eb);

/**
 * @brief Compatible version of network stack input function. Translates to esp_netif_receive()
 */
esp_err_t tcpip_adapter_sta_input(void *buffer, uint16_t len, void *eb);

/**
 * @brief Compatible version of network stack input function. Translates to esp_netif_receive()
 */
esp_err_t tcpip_adapter_ap_input(void *buffer, uint16_t len, void *eb);

/**
 * @brief Compatible version of former tcpip_adapter API to clear default WIFI handlers
 * @return ESP_OK on success
 */
esp_err_t tcpip_adapter_clear_default_wifi_handlers(void);

/**
 * @brief Compatible version of former tcpip_adapter API to clear default ethernet handlers
 * @return ESP_OK on success
 */
esp_err_t tcpip_adapter_clear_default_eth_handlers(void);

/**
 * @brief Compatible version of former tcpip_adapter API of esp_netif_dhcps_stop
*/
esp_err_t tcpip_adapter_dhcps_stop(tcpip_adapter_if_t tcpip_if);

/**
 * @brief Compatible version of former tcpip_adapter API of esp_netif_dhcpc_stop
*/
esp_err_t tcpip_adapter_dhcpc_stop(tcpip_adapter_if_t tcpip_if);

/**
 * @brief Compatible version of former tcpip_adapter API of esp_netif_dhcps_start
*/
esp_err_t tcpip_adapter_dhcps_start(tcpip_adapter_if_t tcpip_if);

/**
 * @brief Compatible version of former tcpip_adapter API of esp_netif_dhcpc_start
*/
esp_err_t tcpip_adapter_dhcpc_start(tcpip_adapter_if_t tcpip_if);

/**
 * @brief Compatible version of former tcpip_adapter API of esp_netif_dhcps_get_status
*/
esp_err_t tcpip_adapter_dhcps_get_status(tcpip_adapter_if_t tcpip_if, tcpip_adapter_dhcp_status_t *status);

/**
 * @brief Compatible version of former tcpip_adapter API of esp_netif_dhcps_option
*/
esp_err_t tcpip_adapter_dhcps_option(tcpip_adapter_dhcp_option_mode_t opt_op, tcpip_adapter_dhcp_option_id_t opt_id, void *opt_val, uint32_t opt_len);

/**
 * @brief Compatible version of former tcpip_adapter API of esp_netif_dhcpc_option
*/
esp_err_t tcpip_adapter_dhcpc_option(tcpip_adapter_dhcp_option_mode_t opt_op, tcpip_adapter_dhcp_option_id_t opt_id, void *opt_val, uint32_t opt_len);

/**
 * @brief Compatible version of former tcpip_adapter API of esp_netif_set_ip_info
*/
esp_err_t tcpip_adapter_set_ip_info(tcpip_adapter_if_t tcpip_if, const tcpip_adapter_ip_info_t *ip_info);

/**
 * @brief Compatible version of former tcpip_adapter API of esp_netif_get_dns_info
*/
esp_err_t tcpip_adapter_get_dns_info(tcpip_adapter_if_t tcpip_if, tcpip_adapter_dns_type_t type, tcpip_adapter_dns_info_t *dns);

/**
 * @brief Compatible version of former tcpip_adapter API of esp_netif_set_dns_info
*/
esp_err_t tcpip_adapter_set_dns_info(tcpip_adapter_if_t tcpip_if, tcpip_adapter_dns_type_t type, tcpip_adapter_dns_info_t *dns);

/**
 * @brief Compatible version of former tcpip_adapter API of esp_netif_get_netif_impl_index
*/
int tcpip_adapter_get_netif_index(tcpip_adapter_if_t tcpip_if);

/**
 * @brief Compatible version of former tcpip_adapter API of esp_netif_get_sta_list
*/
esp_err_t tcpip_adapter_get_sta_list(const wifi_sta_list_t *wifi_sta_list, tcpip_adapter_sta_list_t *tcpip_sta_list);

/**
 * @brief Compatible version of former tcpip_adapter API of esp_netif_action_start for default ethernet
*/
esp_err_t tcpip_adapter_eth_start(uint8_t *mac, tcpip_adapter_ip_info_t *ip_info, void *args);

/**
 * @brief Compatible version of former tcpip_adapter API of esp_netif_action_start for default station
*/
esp_err_t tcpip_adapter_sta_start(uint8_t *mac, tcpip_adapter_ip_info_t *ip_info);

/**
 * @brief Compatible version of former tcpip_adapter API of esp_netif_action_start for default softAP
*/
esp_err_t tcpip_adapter_ap_start(uint8_t *mac, tcpip_adapter_ip_info_t *ip_info);

/**
 * @brief Compatible version of former tcpip_adapter API of esp_netif_action_stop
*/
esp_err_t tcpip_adapter_stop(tcpip_adapter_if_t tcpip_if);

/**
 * @brief Compatible version of former tcpip_adapter API of esp_netif_up
*/
esp_err_t tcpip_adapter_up(tcpip_adapter_if_t tcpip_if);

/**
 * @brief Compatible version of former tcpip_adapter API of esp_netif_down
*/
esp_err_t tcpip_adapter_down(tcpip_adapter_if_t tcpip_if);

/**
 * @brief Compatible version of former tcpip_adapter API of esp_netif_get_old_ip_info
*/
esp_err_t tcpip_adapter_get_old_ip_info(tcpip_adapter_if_t tcpip_if, tcpip_adapter_ip_info_t *ip_info);

/**
 * @brief Compatible version of former tcpip_adapter API of esp_netif_set_old_ip_info
*/
esp_err_t tcpip_adapter_set_old_ip_info(tcpip_adapter_if_t tcpip_if, const tcpip_adapter_ip_info_t *ip_info);

/**
 * @brief Compatible version of former tcpip_adapter API of esp_netif_get_handle_from_netif_impl
*/
esp_interface_t tcpip_adapter_get_esp_if(void *dev);

/**
 * @brief Compatible version of former tcpip_adapter API of esp_netif_set_hostname
*/
esp_err_t tcpip_adapter_set_hostname(tcpip_adapter_if_t tcpip_if, const char *hostname);

/**
 * @brief Compatible version of former tcpip_adapter API of esp_netif_get_hostname
*/
esp_err_t tcpip_adapter_get_hostname(tcpip_adapter_if_t tcpip_if, const char **hostname);

/**
 * @brief This function is called from wifi_init to assure backward compatibility mode
 * of tcpip_adapter. In case of legacy use, default instances of ap and sta
 * are created and handlers are registered
 *
 * @return ESP_OK on success
 */
esp_err_t tcpip_adapter_set_default_wifi_handlers(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif //_TCPIP_ADAPTER_H_
