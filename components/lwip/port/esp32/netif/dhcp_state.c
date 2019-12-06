// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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

#include <stdio.h>
#include <assert.h>
#include "nvs.h"
#include "lwip/opt.h"
#include "lwip/dhcp.h"
#include "lwip/netif.h"
#include "esp_interface.h"
#include "esp_netif.h"
#include "esp_netif_net_stack.h"
#include "netif/dhcp_state.h"

#define DHCP_NAMESPACE "dhcp_state"

// DHCP_Client has to be enabled for this netif
#define VALID_NETIF_ID(netif) (ESP_NETIF_DHCP_CLIENT&esp_netif_get_flags(netif))

bool dhcp_ip_addr_restore(void *netif)
{
    nvs_handle_t nvs;
    bool err = false;
    struct netif *net = (struct netif *)netif;
    struct dhcp *dhcp = netif_dhcp_data(net);
    esp_netif_t *esp_netif = esp_netif_get_handle_from_netif_impl(netif);

    if(VALID_NETIF_ID(esp_netif)) {
        uint32_t *ip_addr = &dhcp->offered_ip_addr.addr;
        if (nvs_open(DHCP_NAMESPACE, NVS_READONLY, &nvs) == ESP_OK) {
            if (nvs_get_u32(nvs, esp_netif_get_ifkey(esp_netif), ip_addr) == ESP_OK) {
                err = true;
            }
            nvs_close(nvs);
        }
    }
    return err;
}

void dhcp_ip_addr_store(void *netif)
{
    nvs_handle_t nvs;
    struct netif *net = (struct netif *)netif;
    struct dhcp *dhcp = netif_dhcp_data(net);
    uint32_t ip_addr = dhcp->offered_ip_addr.addr;
    esp_netif_t *esp_netif = esp_netif_get_handle_from_netif_impl(netif);

    if(VALID_NETIF_ID(esp_netif)) {
        if (nvs_open(DHCP_NAMESPACE, NVS_READWRITE, &nvs) == ESP_OK) {
            nvs_set_u32(nvs,esp_netif_get_ifkey(esp_netif), ip_addr);
            nvs_commit(nvs);
            nvs_close(nvs);
        }
    }
}

void dhcp_ip_addr_erase(void *esp_netif)
{
    nvs_handle_t nvs;

    if(VALID_NETIF_ID(esp_netif)) {
        if (nvs_open(DHCP_NAMESPACE, NVS_READWRITE, &nvs) == ESP_OK) {
            nvs_erase_key(nvs, esp_netif_get_ifkey(esp_netif));
            nvs_commit(nvs);
            nvs_close(nvs);
        }
    }
}