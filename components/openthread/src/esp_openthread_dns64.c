/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_openthread_dns64.h"
#include "esp_netif.h"
#include "esp_netif_types.h"
#include "esp_openthread_lock.h"
#include "esp_openthread_netif_glue.h"

#include "esp_check.h"
#include "esp_log.h"
#include "lwip/ip6_addr.h"
#include "lwip/ip_addr.h"
#include "lwip_default_hooks.h"
#include "lwip/def.h"
#include "lwip/dns.h"
#include "lwip/opt.h"
#include "openthread/instance.h"
#include "openthread/netdata.h"

#define TAG "OT_DNS64"

typedef struct dns_resolve_entry {
    char name[DNS_MAX_NAME_LENGTH];
    dns_found_callback found;
    void *callback_arg;
    bool is_using;
} dns_resolve_entry_t;

static dns_resolve_entry_t s_dns_resolve_entry[DNS_TABLE_SIZE];

esp_err_t esp_openthread_dns64_client_init(void)
{
    memset(s_dns_resolve_entry, 0, sizeof(s_dns_resolve_entry));
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
    esp_openthread_task_switching_lock_acquire(portMAX_DELAY);
    while (otNetDataGetNextRoute(instance, &iter, &route) == OT_ERROR_NONE) {
        if (route.mNat64) {
            break;
        }
    }
    esp_openthread_task_switching_lock_release();

    if (route.mNat64) {
        memcpy(nat64_prefix->addr, route.mPrefix.mPrefix.mFields.m8, sizeof(nat64_prefix->addr));
        return ESP_OK;
    } else {
        return ESP_ERR_NOT_FOUND;
    }
}

static void dns_found_handler(const char *name, const ip_addr_t *ipaddr, void *callback_arg)
{
    dns_resolve_entry_t *resolve_entry = (dns_resolve_entry_t *)callback_arg;
    if (resolve_entry && resolve_entry->found) {
        if (!ipaddr) {
            resolve_entry->found(name, NULL, resolve_entry->callback_arg);
        } else if (lwip_strnicmp(name, resolve_entry->name, sizeof(resolve_entry->name)) == 0) {
            ip_addr_t ipaddr_copy = *ipaddr;
            ip6_addr_t nat64_prefix;
            if (ipaddr_copy.type == IPADDR_TYPE_V4 && esp_openthread_get_nat64_prefix(&nat64_prefix) == ESP_OK) {
                ipaddr_copy.type = IPADDR_TYPE_V6;
                memcpy(ipaddr_copy.u_addr.ip6.addr, nat64_prefix.addr, sizeof(nat64_prefix.addr));
                ipaddr_copy.u_addr.ip6.addr[3] = ipaddr->u_addr.ip4.addr;
                ipaddr_copy.u_addr.ip6.zone = IP6_NO_ZONE;
            }
            resolve_entry->found(name, &ipaddr_copy, resolve_entry->callback_arg);
        }
        resolve_entry->is_using = false;
    }
}

static dns_resolve_entry_t *find_free_dns_resolve_entry(void)
{
    for (uint8_t i = 0; i < DNS_TABLE_SIZE; ++i) {
        if (s_dns_resolve_entry[i].is_using == false) {
            return &s_dns_resolve_entry[i];
        }
    }
    return NULL;
}

int lwip_hook_dns_external_resolve(const char *name, ip_addr_t *addr, dns_found_callback found, void *callback_arg,
                                   u8_t addrtype, err_t *err)
{
    if (addrtype == LWIP_DNS_ADDRTYPE_IPV4 || esp_netif_get_default_netif() != esp_openthread_get_netif()) {
        // If the DNS address type is IPv4 or the openthread netif is not the default netif, skip this hook.
        return 0;
    }
    dns_resolve_entry_t *entry = find_free_dns_resolve_entry();
    if (!entry) {
        ESP_LOGE(TAG, "Cannot find free dns resolve entry");
        *err = ERR_MEM;
        return 1;
    }

    strncpy(entry->name, name, strnlen(name, sizeof(entry->name) - 1));
    entry->name[strnlen(name, sizeof(entry->name) - 1)] = 0;
    entry->found = found;
    entry->callback_arg = callback_arg;
    entry->is_using = true;

    *err = dns_gethostbyname_addrtype(name, addr, dns_found_handler, entry, LWIP_DNS_ADDRTYPE_IPV4);
    if (*err != ERR_INPROGRESS) {
        // If dns query is not enqueued, mark the entry not being used.
        entry->is_using = false;
    }
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
