/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "nvs.h"
#include "lwip/dhcp.h"
#include "lwip/netif.h"
#include "netif/dhcp_state.h"

#define DHCP_NAMESPACE "dhcp_state"
#define IF_KEY_SIZE 3

/*
 * As a NVS key, use string representation of the interface index number
 */
static inline char *gen_if_key(struct netif *netif, char *name)
{
    lwip_itoa(name, IF_KEY_SIZE, netif->num);
    return name;
}

bool dhcp_ip_addr_restore(struct netif *netif)
{
    nvs_handle_t nvs;
    char if_key[IF_KEY_SIZE];
    bool err = false;
    if (netif == NULL) {
        return false;
    }
    struct dhcp *dhcp = netif_dhcp_data(netif);

    uint32_t *ip_addr = &dhcp->offered_ip_addr.addr;
    if (nvs_open(DHCP_NAMESPACE, NVS_READONLY, &nvs) == ESP_OK) {
        if (nvs_get_u32(nvs, gen_if_key(netif, if_key), ip_addr) == ESP_OK) {
            err = true;
        }
        nvs_close(nvs);
    }
    return err;
}

void dhcp_ip_addr_store(struct netif *netif)
{
    nvs_handle_t nvs;
    char if_key[IF_KEY_SIZE];
    if (netif == NULL) {
        return;
    }
    struct dhcp *dhcp = netif_dhcp_data(netif);
    uint32_t ip_addr = dhcp->offered_ip_addr.addr;

    if (nvs_open(DHCP_NAMESPACE, NVS_READWRITE, &nvs) == ESP_OK) {
        nvs_set_u32(nvs, gen_if_key(netif, if_key), ip_addr);
        nvs_commit(nvs);
        nvs_close(nvs);
    }
}

void dhcp_ip_addr_erase(struct netif *netif)
{
    nvs_handle_t nvs;
    char if_key[IF_KEY_SIZE];
    if (netif == NULL) {
        return;
    }
    if (nvs_open(DHCP_NAMESPACE, NVS_READWRITE, &nvs) == ESP_OK) {
        nvs_erase_key(nvs, gen_if_key(netif, if_key));
        nvs_commit(nvs);
        nvs_close(nvs);
    }
}
