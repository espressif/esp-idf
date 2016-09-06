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

#ifndef _TCPIP_ADAPTER_H_
#define _TCPIP_ADAPTER_H_

#include <stdint.h>

#include "rom/queue.h"

#include "esp_wifi.h"

#define CONFIG_TCPIP_LWIP 1
#define CONFIG_DHCP_STA_LIST 1

#if CONFIG_TCPIP_LWIP
#include "lwip/ip_addr.h"
#include "apps/dhcpserver.h"

#define IP2STR(ipaddr) ip4_addr1_16(ipaddr), \
    ip4_addr2_16(ipaddr), \
    ip4_addr3_16(ipaddr), \
    ip4_addr4_16(ipaddr)

#define IPSTR "%d.%d.%d.%d"

typedef struct {
    ip4_addr_t ip;
    ip4_addr_t netmask;
    ip4_addr_t gw;
} tcpip_adapter_ip_info_t;

typedef dhcps_lease_t tcpip_adapter_dhcps_lease_t;

#if CONFIG_DHCP_STA_LIST 
struct station_list {
    STAILQ_ENTRY(station_list) next;
    uint8_t mac[6];
    ip4_addr_t ip;
};
#endif

#endif

#define ESP_ERR_TCPIP_ADAPTER_BASE      0x5000      // base should be moved to esp_err.h

#define ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS        ESP_ERR_TCPIP_ADAPTER_BASE + 0x00
#define ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY          ESP_ERR_TCPIP_ADAPTER_BASE + 0x01
#define ESP_ERR_TCPIP_ADAPTER_DHCPC_START_FAILED    ESP_ERR_TCPIP_ADAPTER_BASE + 0x02
#define ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STARTED  ESP_ERR_TCPIP_ADAPTER_BASE + 0x03
#define ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STOPPED  ESP_ERR_TCPIP_ADAPTER_BASE + 0x04
#define ESP_ERR_TCPIP_ADAPTER_NO_MEM                ESP_ERR_TCPIP_ADAPTER_BASE + 0x05
#define ESP_ERR_TCPIP_ADAPTER_DHCP_NOT_STOPPED      ESP_ERR_TCPIP_ADAPTER_BASE + 0x06

/* will add ethernet interface */
typedef enum {
    TCPIP_ADAPTER_IF_STA = 0,     /**< ESP32 station interface */
    TCPIP_ADAPTER_IF_AP,          /**< ESP32 soft-AP interface */
    TCPIP_ADAPTER_IF_MAX
} tcpip_adapter_if_t;

typedef enum {
    TCPIP_ADAPTER_DHCP_INIT = 0,
    TCPIP_ADAPTER_DHCP_STARTED,
    TCPIP_ADAPTER_DHCP_STOPPED,
    TCPIP_ADAPTER_DHCP_STATUS_MAX
} tcpip_adapter_dhcp_status_t;

typedef enum{
    TCPIP_ADAPTER_OP_START = 0,
    TCPIP_ADAPTER_OP_SET,
    TCPIP_ADAPTER_OP_GET,
    TCPIP_ADAPTER_OP_MAX
} tcpip_adapter_option_mode_t;

typedef enum{
    TCPIP_ADAPTER_ROUTER_SOLICITATION_ADDRESS   = 32,
    TCPIP_ADAPTER_REQUESTED_IP_ADDRESS          = 50,
    TCPIP_ADAPTER_IP_ADDRESS_LEASE_TIME         = 51,
    TCPIP_ADAPTER_IP_REQUEST_RETRY_TIME         = 52,
} tcpip_adapter_option_id_t;

void tcpip_adapter_init(void);

esp_err_t tcpip_adapter_start(tcpip_adapter_if_t tcpip_if, uint8_t *mac, tcpip_adapter_ip_info_t *ip_info);

esp_err_t tcpip_adapter_stop(tcpip_adapter_if_t tcpip_if);

esp_err_t tcpip_adapter_up(tcpip_adapter_if_t tcpip_if);

esp_err_t tcpip_adapter_down(tcpip_adapter_if_t tcpip_if);

esp_err_t tcpip_adapter_get_ip_info(tcpip_adapter_if_t tcpip_if, tcpip_adapter_ip_info_t *ip_info);

esp_err_t tcpip_adapter_set_ip_info(tcpip_adapter_if_t tcpip_if, tcpip_adapter_ip_info_t *ip_info);

#if 0
esp_err_t tcpip_adapter_get_mac(tcpip_adapter_if_t tcpip_if, uint8_t *mac);

esp_err_t tcpip_adapter_set_mac(tcpip_adapter_if_t tcpip_if, uint8_t *mac);
#endif

esp_err_t tcpip_adapter_dhcps_get_status(tcpip_adapter_if_t tcpip_if, tcpip_adapter_dhcp_status_t *status);
esp_err_t tcpip_adapter_dhcps_option(tcpip_adapter_option_mode_t opt_op, tcpip_adapter_option_id_t opt_id, void *opt_val, uint32_t opt_len);
esp_err_t tcpip_adapter_dhcps_start(tcpip_adapter_if_t tcpip_if);
esp_err_t tcpip_adapter_dhcps_stop(tcpip_adapter_if_t tcpip_if);

esp_err_t tcpip_adapter_dhcpc_get_status(tcpip_adapter_if_t tcpip_if, tcpip_adapter_dhcp_status_t *status);
esp_err_t tcpip_adapter_dhcpc_option(tcpip_adapter_option_mode_t opt_op, tcpip_adapter_option_id_t opt_id, void *opt_val, uint32_t opt_len);
esp_err_t tcpip_adapter_dhcpc_start(tcpip_adapter_if_t tcpip_if);
esp_err_t tcpip_adapter_dhcpc_stop(tcpip_adapter_if_t tcpip_if);

esp_err_t tcpip_adapter_sta_input(void *buffer, uint16_t len, void *eb);
esp_err_t tcpip_adapter_ap_input(void *buffer, uint16_t len, void *eb);

wifi_interface_t tcpip_adapter_get_wifi_if(void *dev);

esp_err_t tcpip_adapter_get_sta_list(struct station_info *sta_info, struct station_list **sta_list);
esp_err_t tcpip_adapter_free_sta_list(struct station_list *sta_list);

#endif /*  _TCPIP_ADAPTER_H_ */

