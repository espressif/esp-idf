/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_openthread_dns64.h"

#include "esp_check.h"
#include "esp_event.h"
#include "esp_log.h"
#include "openthread/instance.h"
#include "openthread/netdata.h"
#include "lwip_default_hooks.h"
#include "lwip/api.h"
#include "lwip/dns.h"

#define TAG "OT_DNS64"
// The network data change callback sets the dns server address of index 0, while the CLI sets the dns server address of index 1.
#define OPENTHREAD_DNS_SERVER_INDEX 0

static void esp_openthread_netdata_change_callback(otChangedFlags changed_flags, void *ctx)
{
    if (OT_CHANGED_THREAD_NETDATA & changed_flags) {
        ip_addr_t dns_server_addr = *IP_ADDR_ANY;
        if (esp_openthread_get_nat64_prefix(&dns_server_addr.u_addr.ip6) == ESP_OK) {
            dns_server_addr.type = IPADDR_TYPE_V6;
            dns_server_addr.u_addr.ip6.addr[3] = ipaddr_addr(CONFIG_OPENTHREAD_DNS_SERVER_ADDR);
            const ip_addr_t *dnsserver = dns_getserver(OPENTHREAD_DNS_SERVER_INDEX);
            if (memcmp(dnsserver, &dns_server_addr, sizeof(ip_addr_t)) != 0) {
                ESP_LOGI(TAG, "Set dns server address: %s", ipaddr_ntoa(&dns_server_addr));
                dns_setserver(OPENTHREAD_DNS_SERVER_INDEX, &dns_server_addr);
                if (esp_event_post(OPENTHREAD_EVENT, OPENTHREAD_EVENT_SET_DNS_SERVER, NULL, 0, 0) != ESP_OK) {
                    ESP_LOGE(TAG, "Failed to post OpenThread set DNS server event");
                }
            }
        }
    }
}

esp_err_t esp_openthread_dns64_client_init(void)
{
    otInstance *instance = esp_openthread_get_instance();
    ESP_RETURN_ON_FALSE(otSetStateChangedCallback(instance, esp_openthread_netdata_change_callback, NULL) ==
                            OT_ERROR_NONE,
                        ESP_FAIL, TAG, "Failed to install network data change callback");
    dns_setserver(OPENTHREAD_DNS_SERVER_INDEX, NULL);
    return ESP_OK;
}

esp_err_t esp_openthread_get_dnsserver_addr(ip6_addr_t *dnsserver_addr)
{
    const ip_addr_t *dnsserver = dns_getserver(OPENTHREAD_DNS_SERVER_INDEX);
    ESP_RETURN_ON_FALSE(dnsserver_addr, ESP_ERR_INVALID_ARG, TAG, "dnsserver_addr cannot be NULL");
    ESP_RETURN_ON_FALSE(!ip_addr_isany(dnsserver), ESP_ERR_INVALID_STATE, TAG,
                        "DNS server address is not set");
    memcpy(dnsserver_addr, &dnsserver->u_addr.ip6, sizeof(ip6_addr_t));
    return ESP_OK;
}

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
