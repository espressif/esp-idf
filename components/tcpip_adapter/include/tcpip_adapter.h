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

#include "esp_wifi.h"

#define CONFIG_TCPIP_LWIP 1

#if CONFIG_TCPIP_LWIP
#include "lwip/ip_addr.h"

struct ip_info {
    ip4_addr_t ip;
    ip4_addr_t netmask;
    ip4_addr_t gw;
};
#endif

#define ESP_ERR_TCPIP_ADAPTER_BASE      0x5000      // base should be moved to esp_err.h

#define ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS        ESP_ERR_TCPIP_ADAPTER_BASE + 0x00
#define ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY          ESP_ERR_TCPIP_ADAPTER_BASE + 0x01
#define ESP_ERR_TCPIP_ADAPTER_DHCPC_START_FAILED    ESP_ERR_TCPIP_ADAPTER_BASE + 0x02
#define ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STARTED  ESP_ERR_TCPIP_ADAPTER_BASE + 0x03
#define ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STOPED   ESP_ERR_TCPIP_ADAPTER_BASE + 0x04

/* will add ethernet interface */
typedef enum {
    TCPIP_ADAPTER_IF_STA = 0,     /**< ESP32 station interface */
    TCPIP_ADAPTER_IF_AP,          /**< ESP32 soft-AP interface */
    TCPIP_ADAPTER_IF_MAX
} tcpip_adapter_if_t;

typedef enum {
    TCPIP_ADAPTER_DHCP_INIT = 0,
    TCPIP_ADAPTER_DHCP_STARTED,
    TCPIP_ADAPTER_DHCP_STOPED,
    TCPIP_ADAPTER_DHCP_STATUS_MAX
} tcpip_adapter_dhcp_status_t;

void tcpip_adapter_init(void);

esp_err_t tcpip_adapter_start(tcpip_adapter_if_t tcpip_if, uint8_t *mac, struct ip_info *info);

esp_err_t tcpip_adapter_stop(tcpip_adapter_if_t tcpip_if);

esp_err_t tcpip_adapter_up(tcpip_adapter_if_t tcpip_if);

esp_err_t tcpip_adapter_down(tcpip_adapter_if_t tcpip_if);

esp_err_t tcpip_adapter_get_ip_info(tcpip_adapter_if_t tcpip_if, struct ip_info *if_ip);

esp_err_t tcpip_adapter_set_ip_info(tcpip_adapter_if_t tcpip_if, struct ip_info *if_ip);

#if 0
esp_err_t tcpip_adapter_get_mac(tcpip_adapter_if_t tcpip_if, uint8_t *mac);

esp_err_t tcpip_adapter_set_mac(tcpip_adapter_if_t tcpip_if, uint8_t *mac);
#endif

esp_err_t tcpip_adapter_dhcps_get_status(tcpip_adapter_if_t tcpip_if, tcpip_adapter_dhcp_status_t *status);

esp_err_t tcpip_adapter_dhcps_start(tcpip_adapter_if_t tcpip_if);

esp_err_t tcpip_adapter_dhcps_stop(tcpip_adapter_if_t tcpip_if);

esp_err_t tcpip_adapter_dhcpc_get_status(tcpip_adapter_if_t tcpip_if, tcpip_adapter_dhcp_status_t *status);

esp_err_t tcpip_adapter_dhcpc_start(tcpip_adapter_if_t tcpip_if);

esp_err_t tcpip_adapter_dhcpc_stop(tcpip_adapter_if_t tcpip_if);

esp_err_t tcpip_adapter_sta_input(void *buffer, uint16_t len, void* eb);
esp_err_t tcpip_adapter_ap_input(void *buffer, uint16_t len, void* eb);

#if 0
//esp_err_t tcpip_dep_input(wifi_interface_t wifi_if, void *buffer, uint16 len);

esp_err_t tcpip_adapter_output(tcpip_adapter_if_t tcpip_if, void *buffer, uint16_t len);
#endif

wifi_interface_t tcpip_adapter_get_wifi_if(void *dev);

#endif /*  _TCPIP_ADAPTER_H_ */

