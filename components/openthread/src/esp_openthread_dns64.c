/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_openthread_dns64.h"

#include "esp_log.h"
#include "openthread/instance.h"
#include "openthread/netdata.h"
#include "lwip_default_hooks.h"
#include "lwip/api.h"

#define TAG "OT_DNS64"

esp_err_t esp_openthread_get_nat64_prefix(ip6_addr_t *nat64_prefix)
{
    otNetworkDataIterator iter = OT_NETWORK_DATA_ITERATOR_INIT;
    otInstance *instance = esp_openthread_get_instance();
    otExternalRouteConfig route;

    memset(&route, 0, sizeof(route));
    while (otNetDataGetNextRoute(instance, &iter, &route) == OT_ERROR_NONE) {
        if (route.mNat64) {
            break;
        }
    }

    if (route.mNat64) {
        memcpy(nat64_prefix->addr, route.mPrefix.mPrefix.mFields.m8, sizeof(nat64_prefix->addr));
        return ESP_OK;
    } else {
        return ESP_ERR_NOT_FOUND;
    }
}

int lwip_hook_netconn_external_resolve(const char *name, ip_addr_t *addr, u8_t addrtype, err_t *err)
{
    if (addrtype == NETCONN_DNS_IPV4) {
        return 0;
    }

    ip6_addr_t nat64_prefix;
    if (esp_openthread_get_nat64_prefix(&nat64_prefix) != ESP_OK) {
        ESP_LOGE(TAG, "Cannot find NAT64 prefix\n");
        *err = ERR_ABRT;
        return 1;
    }

    *err = netconn_gethostbyname_addrtype(name, addr, NETCONN_DNS_IPV4);
    if (*err != ERR_OK) {
        return 1;
    }
    if (addr->type == IPADDR_TYPE_V4) {
        ip4_addr_t addr_copy = addr->u_addr.ip4;
        addr->type = IPADDR_TYPE_V6;
        memcpy(addr->u_addr.ip6.addr, nat64_prefix.addr, sizeof(nat64_prefix.addr));
        addr->u_addr.ip6.addr[3] = addr_copy.addr;
        addr->u_addr.ip6.zone = IP6_NO_ZONE;
    }
    return 1;
}
