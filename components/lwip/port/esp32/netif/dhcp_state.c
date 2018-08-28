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
#include "tcpip_adapter.h"
#include "netif/dhcp_state.h"

#define DHCP_NAMESPACE "dhcp_state"
#define VALID_NETIF_ID(id) ((id < ESP_IF_MAX) && (id != ESP_IF_WIFI_AP))

static uint32_t restored_ip_addr[TCPIP_ADAPTER_IF_MAX];
static const char *interface_key[] = {"IF_STA", "IF_AP", "IF_ETH", "IF_TEST"};

_Static_assert(sizeof(interface_key) / sizeof(char*) == TCPIP_ADAPTER_IF_MAX,
               "Number interface keys differs from number of interfaces");

bool dhcp_ip_addr_restore(void *netif)
{
    nvs_handle_t nvs;
    bool err = false;
    struct netif *net = (struct netif *)netif;
    struct dhcp *dhcp = netif_dhcp_data(net);
    esp_interface_t netif_id = tcpip_adapter_get_esp_if(net);

    if(VALID_NETIF_ID(netif_id)) {
        uint32_t *ip_addr = &dhcp->offered_ip_addr.addr;
        if (nvs_open(DHCP_NAMESPACE, NVS_READONLY, &nvs) == ESP_OK) {
            if (nvs_get_u32(nvs, interface_key[netif_id], ip_addr) == ESP_OK) {
                restored_ip_addr[netif_id] = *ip_addr;
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
    esp_interface_t netif_id = tcpip_adapter_get_esp_if(net);

    if(VALID_NETIF_ID(netif_id)) {
        if (restored_ip_addr[netif_id] != ip_addr) {
            if (nvs_open(DHCP_NAMESPACE, NVS_READWRITE, &nvs) == ESP_OK) {
                nvs_set_u32(nvs, interface_key[netif_id], ip_addr);
                nvs_commit(nvs);
                nvs_close(nvs);
            }
        }
    }
}

void dhcp_ip_addr_erase(void *netif)
{
    nvs_handle_t nvs;
    struct netif *net = (struct netif *)netif;
    esp_interface_t netif_id = tcpip_adapter_get_esp_if(net);

    if(VALID_NETIF_ID(netif_id)) {
        if (nvs_open(DHCP_NAMESPACE, NVS_READWRITE, &nvs) == ESP_OK) {
            nvs_erase_key(nvs, interface_key[netif_id]);
            nvs_commit(nvs);
            nvs_close(nvs);
        }
    }
}