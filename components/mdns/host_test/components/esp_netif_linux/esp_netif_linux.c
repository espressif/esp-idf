// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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
#include<stdio.h>
#include <stdlib.h>

#include "esp_netif.h"
#include "esp_err.h"
#include <string.h>	//strlen
#include <sys/socket.h>
#include <arpa/inet.h>	//inet_addr
#include <sys/types.h>
#include <ifaddrs.h>
#include <net/if.h>
#include "esp_netif_types.h"

#define MAX_NETIFS  4

static esp_netif_t* s_netif_list[MAX_NETIFS] = { 0 };

struct esp_netif_obj
{
    const char *if_key;
    const char *if_desc;
};

esp_netif_t *esp_netif_get_handle_from_ifkey(const char *if_key)
{
    for (int i=0; i<MAX_NETIFS; ++i) {
        if (s_netif_list[i] && strcmp(s_netif_list[i]->if_key, if_key) == 0) {
            return s_netif_list[i];
        }
    }
    return NULL;
}

esp_err_t esp_netif_get_ip_info(esp_netif_t *esp_netif, esp_netif_ip_info_t *ip_info)
{
    if (esp_netif == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    struct ifaddrs *addrs, *tmp;
    getifaddrs(&addrs);
    tmp = addrs;

    while (tmp) {
        if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET) {
            char addr[20];
            struct sockaddr_in *pAddr = (struct sockaddr_in *) tmp->ifa_addr;
            inet_ntop(AF_INET, &pAddr->sin_addr, addr, sizeof(addr) );
            if (strcmp(esp_netif->if_desc, tmp->ifa_name) == 0) {
                printf("AF_INET: %s: %s\n", tmp->ifa_name, addr);
                memcpy(&ip_info->ip.addr, &pAddr->sin_addr, 4);
                break;
            }
        }
        tmp = tmp->ifa_next;
    }
    return ESP_OK;
}

esp_err_t esp_netif_dhcpc_get_status(esp_netif_t *esp_netif, esp_netif_dhcp_status_t *status)
{
    return ESP_OK;
}


esp_err_t esp_netif_get_ip6_linklocal(esp_netif_t *esp_netif, esp_ip6_addr_t *if_ip6)
{
    if (esp_netif == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    struct ifaddrs *addrs, *tmp;
    getifaddrs(&addrs);
    tmp = addrs;

    while (tmp)
    {
        if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET6) {
            char addr[64];
            struct sockaddr_in6 *pAddr = (struct sockaddr_in6 *)tmp->ifa_addr;
            inet_ntop(AF_INET6, &pAddr->sin6_addr, addr, sizeof(addr) );
            if (strcmp(esp_netif->if_desc, tmp->ifa_name) == 0) {
                printf("AF_INET6: %s: %s\n", tmp->ifa_name, addr);
                memcpy(if_ip6->addr, &pAddr->sin6_addr, 4*4);
                break;
            }
        }
        tmp = tmp->ifa_next;
    }

    freeifaddrs(addrs);
    return ESP_OK;
}


int esp_netif_get_netif_impl_index(esp_netif_t *esp_netif)
{
    if (esp_netif == NULL) {
        return -1;
    }
    uint32_t interfaceIndex = if_nametoindex(esp_netif->if_desc);
    return interfaceIndex;
}

esp_err_t esp_netif_get_netif_impl_name(esp_netif_t *esp_netif, char* name)
{
    if (esp_netif == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    strcpy(name, esp_netif->if_desc);
    return ESP_OK;
}

const char *esp_netif_get_desc(esp_netif_t *esp_netif)
{
    if (esp_netif == NULL) {
        return NULL;
    }
    return esp_netif->if_desc;
}

esp_netif_t *esp_netif_new(const esp_netif_config_t *config)
{
    if (esp_netif_get_handle_from_ifkey(config->base->if_key)) {
        return NULL;
    }
    esp_netif_t* netif = calloc(1, sizeof(struct esp_netif_obj));
    if (netif) {
        netif->if_desc = config->base->if_desc;
        netif->if_key = config->base->if_key;
    }

    for (int i=0; i<MAX_NETIFS; ++i) {
        if (s_netif_list[i] == NULL) {
            s_netif_list[i] = netif;
            break;
        }
    }

    return netif;
}

void esp_netif_destroy(esp_netif_t *esp_netif)
{
    for (int i=0; i<MAX_NETIFS; ++i) {
        if (s_netif_list[i] == esp_netif) {
            s_netif_list[i] = NULL;
            break;
        }
    }
    free(esp_netif);
}
