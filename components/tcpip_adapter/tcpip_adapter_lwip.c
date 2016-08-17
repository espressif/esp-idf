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
#include <stdio.h>
#include <string.h>

#include "tcpip_adapter.h"

#if CONFIG_TCPIP_LWIP

#include "lwip/inet.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"
#include "lwip/ip_addr.h"

#include "netif/wlanif.h"

#include "apps/dhcpserver.h"

static struct netif *esp_netif[TCPIP_ADAPTER_IF_MAX];
static struct ip_info esp_ip[TCPIP_ADAPTER_IF_MAX];

static tcpip_adapter_dhcp_status_t dhcps_status = TCPIP_ADAPTER_DHCP_INIT;
static tcpip_adapter_dhcp_status_t dhcpc_status = TCPIP_ADAPTER_DHCP_INIT;


static esp_err_t tcpip_adapter_addr_change_cb(struct netif *netif);

#define TCPIP_ADAPTER_DEBUG  ets_printf

void tcpip_adapter_init(void)
{
    static bool tcpip_inited = false;

    if (tcpip_inited == false) {
        tcpip_inited = true;

        tcpip_init(NULL, NULL);

        IP4_ADDR(&esp_ip[TCPIP_ADAPTER_IF_AP].ip, 192, 168 , 4, 1);
        IP4_ADDR(&esp_ip[TCPIP_ADAPTER_IF_AP].gw, 192, 168 , 4, 1);
        IP4_ADDR(&esp_ip[TCPIP_ADAPTER_IF_AP].netmask, 255, 255 , 255, 0);
    }

    netif_reg_addr_change_cb(tcpip_adapter_addr_change_cb);
}

esp_err_t tcpip_adapter_start(tcpip_adapter_if_t tcpip_if, uint8_t *mac, struct ip_info *info)
{
    if (tcpip_if >= TCPIP_ADAPTER_IF_MAX || mac == NULL || info == NULL) {
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }

    if (esp_netif[tcpip_if] == NULL) {
        esp_netif[tcpip_if] = malloc(sizeof(struct netif));
        if (esp_netif[tcpip_if] == NULL) {
            return ESP_ERR_NO_MEM;
        }
        memcpy(esp_netif[tcpip_if]->hwaddr, mac, NETIF_MAX_HWADDR_LEN);
        netif_add(esp_netif[tcpip_if], &info->ip, &info->netmask, &info->gw, NULL, wlanif_init, tcpip_input);
    }

    if (tcpip_if == TCPIP_ADAPTER_IF_AP) {
        netif_set_up(esp_netif[tcpip_if]);

        if (dhcps_status == TCPIP_ADAPTER_DHCP_INIT) {
            dhcps_start(esp_netif[tcpip_if]);
            printf("dhcp server start:(ip: %s, mask: %s, gw: %s)\n", inet_ntoa(info->ip), inet_ntoa(info->netmask), inet_ntoa(info->gw));
            dhcps_status = TCPIP_ADAPTER_DHCP_STARTED;
        }
    }

    /* if ap is on, choose ap as default if */
    if (esp_netif[TCPIP_ADAPTER_IF_AP]) {
        netif_set_default(esp_netif[TCPIP_ADAPTER_IF_AP]);
    } else if (esp_netif[TCPIP_ADAPTER_IF_STA]) {
        netif_set_default(esp_netif[TCPIP_ADAPTER_IF_STA]);
    }

    return ESP_OK;
}

esp_err_t tcpip_adapter_stop(tcpip_adapter_if_t tcpip_if)
{
    if (tcpip_if >= TCPIP_ADAPTER_IF_MAX) {
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }

    if (esp_netif[tcpip_if] == NULL) {
        return ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY;
    }

    if (tcpip_if == TCPIP_ADAPTER_IF_AP) {
        dhcps_stop(esp_netif[tcpip_if]);    // TODO: dhcps checks status by its self
        dhcps_status = TCPIP_ADAPTER_DHCP_INIT;
    } else if (tcpip_if == TCPIP_ADAPTER_IF_STA) {
        dhcp_stop(esp_netif[tcpip_if]);
        dhcpc_status = TCPIP_ADAPTER_DHCP_INIT;
    }

    netif_remove(esp_netif[tcpip_if]);

    free(esp_netif[tcpip_if]);
    esp_netif[tcpip_if] = NULL;

    /* in ap + sta mode, if stop ap, choose sta as default if */
    if (esp_netif[TCPIP_ADAPTER_IF_STA] && tcpip_if == TCPIP_ADAPTER_IF_AP) {
        netif_set_default(esp_netif[TCPIP_ADAPTER_IF_STA]);
    }

    return ESP_OK;
}

esp_err_t tcpip_adapter_up(tcpip_adapter_if_t tcpip_if)
{
    if (tcpip_if == TCPIP_ADAPTER_IF_STA) {
        if (esp_netif[tcpip_if] == NULL){
            return ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY;
        }

        netif_set_up(esp_netif[tcpip_if]);
    }

    return ESP_OK;
}

esp_err_t tcpip_adapter_down(tcpip_adapter_if_t tcpip_if)
{
    if (tcpip_if == TCPIP_ADAPTER_IF_STA) {
        if (esp_netif[tcpip_if] == NULL){
            return ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY;
        }

        if (esp_netif[tcpip_if]->dhcp != NULL) {
            dhcp_release(esp_netif[tcpip_if]);
            dhcp_stop(esp_netif[tcpip_if]);
            dhcp_cleanup(esp_netif[tcpip_if]);
            dhcpc_status = TCPIP_ADAPTER_DHCP_STOPED;
        } else {
            netif_set_down(esp_netif[tcpip_if]);
            netif_set_addr(esp_netif[tcpip_if], IP4_ADDR_ANY, IP4_ADDR_ANY, IP4_ADDR_ANY);
        }
    }

    return ESP_OK;
}

esp_err_t tcpip_adapter_get_ip_info(tcpip_adapter_if_t tcpip_if, struct ip_info *if_ip)
{
    struct netif *p_netif;

    if (tcpip_if >= TCPIP_ADAPTER_IF_MAX || if_ip == NULL) {
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }

    p_netif = esp_netif[tcpip_if];

    if (p_netif != NULL && netif_is_up(p_netif)) {
        ip4_addr_set(&if_ip->ip, ip_2_ip4(&p_netif->ip_addr));
        ip4_addr_set(&if_ip->netmask, ip_2_ip4(&p_netif->netmask));
        ip4_addr_set(&if_ip->gw, ip_2_ip4(&p_netif->gw));

        return ESP_OK;
    }

    ip4_addr_copy(if_ip->ip, esp_ip[tcpip_if].ip);
    ip4_addr_copy(if_ip->gw, esp_ip[tcpip_if].gw);
    ip4_addr_copy(if_ip->netmask, esp_ip[tcpip_if].netmask);

    return ESP_OK;
}

esp_err_t tcpip_adapter_addr_change_cb(struct netif *netif)
{
    tcpip_adapter_if_t tcpip_if;

    if (!netif) {
        TCPIP_ADAPTER_DEBUG("null netif=%p\n", netif);
        return ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY;
    }

    if (netif == esp_netif[TCPIP_ADAPTER_IF_STA]) {
        tcpip_if = TCPIP_ADAPTER_IF_STA;
    } else if (netif == esp_netif[TCPIP_ADAPTER_IF_AP]){
        tcpip_if = TCPIP_ADAPTER_IF_AP;
    } else {
        TCPIP_ADAPTER_DEBUG("invalid netif=%p\n", netif);
        return ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY;
    }

    //check whether IP is changed
    if ( !ip4_addr_cmp(ip_2_ip4(&netif->ip_addr), &esp_ip[tcpip_if].ip) ||
         !ip4_addr_cmp(ip_2_ip4(&netif->gw), &esp_ip[tcpip_if].gw) ||
         !ip4_addr_cmp(ip_2_ip4(&netif->netmask), &esp_ip[tcpip_if].netmask) ) {

        ip4_addr_set(&esp_ip[tcpip_if].ip, ip_2_ip4(&netif->ip_addr));
        ip4_addr_set(&esp_ip[tcpip_if].netmask, ip_2_ip4(&netif->netmask));
        ip4_addr_set(&esp_ip[tcpip_if].gw, ip_2_ip4(&netif->gw));

        //notify event
        printf("ip: %s, ", inet_ntoa(esp_ip[tcpip_if].ip));
        printf("mask: %s, ", inet_ntoa(esp_ip[tcpip_if].netmask));
        printf("gw: %s\n", inet_ntoa(esp_ip[tcpip_if].gw));

        if (ip_2_ip4(&netif->ip_addr) != IP4_ADDR_ANY) {
        }
    } else {
        TCPIP_ADAPTER_DEBUG("ip unchanged\n");
    }

    return ESP_OK;
}

esp_err_t tcpip_adapter_set_ip_info(tcpip_adapter_if_t tcpip_if, struct ip_info *if_ip)
{
    struct netif *p_netif;

    if (tcpip_if >= TCPIP_ADAPTER_IF_MAX || if_ip == NULL) {
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }

    ip4_addr_copy(esp_ip[tcpip_if].ip, if_ip->ip);
    ip4_addr_copy(esp_ip[tcpip_if].gw, if_ip->gw);
    ip4_addr_copy(esp_ip[tcpip_if].netmask, if_ip->netmask);

    p_netif = esp_netif[tcpip_if];

    if (p_netif != NULL && netif_is_up(p_netif)) {
        netif_set_addr(p_netif, &if_ip->ip, &if_ip->netmask, &if_ip->gw);
    }

    return ESP_OK;
}

#if 0
esp_err_t tcpip_adapter_get_mac(tcpip_adapter_if_t tcpip_if, uint8_t mac[6])
{
    struct netif *p_netif;

    if (tcpip_if >= TCPIP_ADAPTER_IF_MAX || mac == NULL) {
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }

    p_netif = esp_netif[tcpip_if];

    if (p_netif != NULL) {
        memcpy(mac, p_netif->hwaddr, NETIF_MAX_HWADDR_LEN);

        return ESP_OK;
    }

    return ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY;
}

esp_err_t tcpip_adapter_set_mac(tcpip_adapter_if_t tcpip_if, uint8_t mac[6])
{
    struct netif *p_netif;

    if (tcpip_if >= TCPIP_ADAPTER_IF_MAX || mac == NULL) {
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }

    p_netif = esp_netif[tcpip_if];

    if (p_netif != NULL) {
        memcpy(p_netif->hwaddr, mac, NETIF_MAX_HWADDR_LEN);

        return ESP_OK;
    }

    return ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY;
}
#endif

esp_err_t tcpip_adapter_dhcps_get_status(tcpip_adapter_if_t tcpip_if, tcpip_adapter_dhcp_status_t *status)
{
    *status = dhcps_status;

    return ESP_OK;
}

esp_err_t tcpip_adapter_dhcps_start(tcpip_adapter_if_t tcpip_if)
{
    /* only support ap now */
    if (tcpip_if != TCPIP_ADAPTER_IF_AP || tcpip_if >= TCPIP_ADAPTER_IF_MAX) {
        TCPIP_ADAPTER_DEBUG("dhcp server invalid if=%d\n", tcpip_if);
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }

    if (dhcps_status != TCPIP_ADAPTER_DHCP_STARTED) {
        struct netif *p_netif = esp_netif[tcpip_if];

        if (p_netif != NULL && netif_is_up(p_netif)) {
            dhcps_start(p_netif);
            dhcps_status = TCPIP_ADAPTER_DHCP_STARTED;
            TCPIP_ADAPTER_DEBUG("dhcp server start successfully\n");
            return ESP_OK;
        } else {
            TCPIP_ADAPTER_DEBUG("dhcp server re init\n");
            dhcps_status = TCPIP_ADAPTER_DHCP_INIT;
            return ESP_OK;
        }
    }

    TCPIP_ADAPTER_DEBUG("dhcp server already start\n");
    return ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STARTED;
}

esp_err_t tcpip_adapter_dhcps_stop(tcpip_adapter_if_t tcpip_if)
{
    /* only support ap now */
    if (tcpip_if != TCPIP_ADAPTER_IF_AP || tcpip_if >= TCPIP_ADAPTER_IF_MAX) {
        TCPIP_ADAPTER_DEBUG("dhcp server invalid if=%d\n", tcpip_if);
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }

    if (dhcps_status == TCPIP_ADAPTER_DHCP_STARTED) {
        struct netif *p_netif = esp_netif[tcpip_if];

        if (p_netif != NULL) {
            dhcps_stop(p_netif);
        } else {
            TCPIP_ADAPTER_DEBUG("dhcp server if not ready\n");
            return ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY;
        }
    } else if (dhcps_status == TCPIP_ADAPTER_DHCP_STOPED) {
        TCPIP_ADAPTER_DEBUG("dhcp server already stoped\n");
        return ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STOPED;
    }

    TCPIP_ADAPTER_DEBUG("dhcp server stop successfully\n");
    dhcps_status = TCPIP_ADAPTER_DHCP_STOPED;
    return ESP_OK;
}

esp_err_t tcpip_adapter_dhcpc_get_status(tcpip_adapter_if_t tcpip_if, tcpip_adapter_dhcp_status_t *status)
{
    *status = dhcpc_status;

    return ESP_OK;
}

esp_err_t tcpip_adapter_dhcpc_start(tcpip_adapter_if_t tcpip_if)
{
    /* only support sta now, need to support ethernet */
    if (tcpip_if != TCPIP_ADAPTER_IF_STA || tcpip_if >= TCPIP_ADAPTER_IF_MAX) {
        TCPIP_ADAPTER_DEBUG("dhcp client invalid if=%d\n", tcpip_if);
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }

    if (dhcpc_status != TCPIP_ADAPTER_DHCP_STARTED) {
        struct netif *p_netif = esp_netif[tcpip_if];

        if (p_netif != NULL) {
            if (netif_is_up(p_netif)) {
                TCPIP_ADAPTER_DEBUG("dhcp client init ip/mask/gw to all-0\n");
                ip_addr_set_zero(&p_netif->ip_addr);;
                ip_addr_set_zero(&p_netif->netmask);
                ip_addr_set_zero(&p_netif->gw);
            }

            if (dhcp_start(p_netif) != ERR_OK) {
                TCPIP_ADAPTER_DEBUG("dhcp client start failed\n");
                return ESP_ERR_TCPIP_ADAPTER_DHCPC_START_FAILED;
            }

            TCPIP_ADAPTER_DEBUG("dhcp client start successfully\n");
            dhcpc_status = TCPIP_ADAPTER_DHCP_STARTED;
            return ESP_OK;
        } else {
            TCPIP_ADAPTER_DEBUG("dhcp client re init\n");
            dhcpc_status = TCPIP_ADAPTER_DHCP_INIT;
            return ESP_OK;
        }
    }

    TCPIP_ADAPTER_DEBUG("dhcp client already started\n");
    return ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STARTED;
}

esp_err_t tcpip_dep_dhcpc_stop(tcpip_adapter_if_t tcpip_if)
{
    /* only support sta now, need to support ethernet */
    if (tcpip_if != TCPIP_ADAPTER_IF_STA || tcpip_if >= TCPIP_ADAPTER_IF_MAX) {
        TCPIP_ADAPTER_DEBUG("dhcp client invalid if=%d\n", tcpip_if);
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }

    if (dhcpc_status == TCPIP_ADAPTER_DHCP_STARTED) {
        struct netif *p_netif = esp_netif[tcpip_if];

        if (p_netif != NULL) {
            dhcp_stop(p_netif);
        } else {
            TCPIP_ADAPTER_DEBUG("dhcp client if not ready\n");
            return ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY;
        }
    } else if (dhcps_status == TCPIP_ADAPTER_DHCP_STOPED) {
        TCPIP_ADAPTER_DEBUG("dhcp client already stoped\n");
        return ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STOPED;
    }

    TCPIP_ADAPTER_DEBUG("dhcp client stop successfully\n");
    dhcpc_status = TCPIP_ADAPTER_DHCP_STOPED;
    return ESP_OK;
}

esp_err_t tcpip_adapter_sta_input(void *buffer, uint16_t len, void* eb)
{
    wlanif_input(esp_netif[TCPIP_ADAPTER_IF_STA], buffer, len, eb);
    return ESP_OK;
}

esp_err_t tcpip_adapter_ap_input(void *buffer, uint16_t len, void* eb)
{
    wlanif_input(esp_netif[TCPIP_ADAPTER_IF_AP], buffer, len, eb);
    return ESP_OK;
}

#if 0
bool tcpip_dep_output(wifi_interface_t wifi_if, void *buffer, uint16_t len)
{

    return true;
}
#endif

wifi_interface_t tcpip_adapter_get_wifi_if(void *dev)
{
    struct netif *p_netif = (struct netif *)dev;

    if (p_netif == esp_netif[TCPIP_ADAPTER_IF_STA]) {
        return WIFI_IF_STA;
    } else if (p_netif == esp_netif[TCPIP_ADAPTER_IF_AP]) {
        return WIFI_IF_AP;
    }

    return WIFI_IF_MAX;
}
#endif
