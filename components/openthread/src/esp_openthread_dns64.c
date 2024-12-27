/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_openthread_dns64.h"
#include "esp_netif.h"
#include "esp_openthread_state.h"
#include "esp_openthread_netif_glue.h"

#include "esp_check.h"
#include "esp_event.h"
#include "esp_log.h"
#include "openthread/instance.h"
#include "openthread/netdata.h"
#include "lwip_default_hooks.h"
#include "lwip/api.h"
#include "lwip/dns.h"

#define TAG "OT_DNS64"

esp_err_t esp_openthread_dns64_client_init(void)
{
    dns_setserver(OPENTHREAD_DNS_SERVER_INDEX, NULL);
    return ESP_OK;
}

esp_err_t esp_openthread_get_dnsserver_addr_with_type(ip6_addr_t *dnsserver_addr,
                                                      esp_netif_dns_type_t dns_type)
{
    ESP_RETURN_ON_FALSE(dns_type < ESP_NETIF_DNS_MAX, ESP_ERR_INVALID_ARG, TAG, "Invalid DNS type");
    ESP_RETURN_ON_FALSE(dnsserver_addr, ESP_ERR_INVALID_ARG, TAG, "dnsserver_addr cannot be NULL");
    esp_netif_t *openthread_netif = esp_openthread_get_netif();
    ESP_RETURN_ON_FALSE(openthread_netif, ESP_ERR_ESP_NETIF_IF_NOT_READY, TAG, "openthread netif is not initializd");
    esp_netif_dns_info_t dns;
    ESP_RETURN_ON_ERROR(esp_netif_get_dns_info(openthread_netif, dns_type, &dns), TAG, "Failed to get dns info");
    if (dns.ip.type == ESP_IPADDR_TYPE_V6) {
        memcpy(dnsserver_addr->addr, dns.ip.u_addr.ip6.addr, sizeof(dnsserver_addr->addr));
        dnsserver_addr->zone = dns.ip.u_addr.ip6.zone;
    }
    return ESP_OK;
}

esp_err_t esp_openthread_get_dnsserver_addr(ip6_addr_t *dnsserver_addr)
{
    return esp_openthread_get_dnsserver_addr_with_type(dnsserver_addr, ESP_NETIF_DNS_MAIN);
}

esp_err_t esp_openthread_set_dnsserver_addr_with_type(const ip6_addr_t dnsserver_addr, esp_netif_dns_type_t dns_type)
{
    ESP_RETURN_ON_FALSE(dns_type < ESP_NETIF_DNS_MAX, ESP_ERR_INVALID_ARG, TAG, "Invalid DNS type");
    ESP_RETURN_ON_FALSE(!ip6_addr_isany(&dnsserver_addr), ESP_ERR_INVALID_ARG, TAG, "dnsserver_addr cannot be any");
    esp_netif_t *openthread_netif = esp_openthread_get_netif();
    ESP_RETURN_ON_FALSE(openthread_netif, ESP_ERR_ESP_NETIF_IF_NOT_READY, TAG, "openthread netif is not initializd");
    esp_netif_dns_info_t dns;
    dns.ip.type = ESP_IPADDR_TYPE_V6;
    dns.ip.u_addr.ip6.zone = dnsserver_addr.zone;
    memcpy(dns.ip.u_addr.ip6.addr, dnsserver_addr.addr, sizeof(dns.ip.u_addr.ip6.addr));
    ESP_RETURN_ON_ERROR(esp_netif_set_dns_info(openthread_netif, dns_type, &dns), TAG, "Failed to get dns info");
    return ESP_OK;
}

esp_err_t esp_openthread_set_dnsserver_addr(const ip6_addr_t dnsserver_addr)
{
    return esp_openthread_set_dnsserver_addr_with_type(dnsserver_addr, ESP_NETIF_DNS_MAIN);
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
    if (addrtype == NETCONN_DNS_IPV4 || esp_netif_get_default_netif() != esp_openthread_get_netif()) {
        // If the DNS address type is IPv4 or the openthread netif is not the default netif, skip this hook.
        return 0;
    }

    *err = netconn_gethostbyname_addrtype(name, addr, NETCONN_DNS_IPV4);
    if (*err == ERR_OK && addr->type == IPADDR_TYPE_V4) {
        ip4_addr_t addr_copy = addr->u_addr.ip4;
        ip6_addr_t nat64_prefix;
        if (esp_openthread_get_nat64_prefix(&nat64_prefix) == ESP_OK) {
            addr->type = IPADDR_TYPE_V6;
            memcpy(addr->u_addr.ip6.addr, nat64_prefix.addr, sizeof(nat64_prefix.addr));
            addr->u_addr.ip6.addr[3] = addr_copy.addr;
            addr->u_addr.ip6.zone = IP6_NO_ZONE;
        }
    }
    return 1;
}
