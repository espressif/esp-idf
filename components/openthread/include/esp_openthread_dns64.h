/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_netif_types.h"
#include "esp_openthread.h"
#include "lwip/netdb.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief This function initiizes the dns64 client.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL if OpenThread state changed callback fails to be registered
 *
 */
esp_err_t esp_openthread_dns64_client_init(void);

/**
 * @brief This function acquires the main DNS server address for OpenThread netif.
 *
 * @param[out] dnsserver_addr      The dns server address.
 *
 * @return
 *      - ESP_OK on sussess
 *      - ESP_ERR_INVALID_ARG if dnsserver_addr is NULL or obtained DNS server address is invalid
 *      - ESP_ERR_ESP_NETIF_IF_NOT_READY if openthread network interface is not initialized
 *      - ESP_ERR_ESP_NETIF_INVALID_PARAMS if failed to call esp_netif APIs getting dns info
 */
esp_err_t esp_openthread_get_dnsserver_addr(ip6_addr_t *dnsserver_addr);

/**
 * @brief This function acquires the DNS server address for OpenThread netif.
 *
 * @param[out] dnsserver_addr      The dns server address.
 * @param[in] dns_type             The type of DNS server
 *
 * @return
 *      - ESP_OK on sussess
 *      - ESP_ERR_INVALID_ARG if dnsserver_addr is NULL or obtained DNS server address is invalid
 *      - ESP_ERR_ESP_NETIF_IF_NOT_READY if openthread network interface is not initialized
 *      - ESP_ERR_ESP_NETIF_INVALID_PARAMS if failed to call esp_netif APIs getting dns info
 */
esp_err_t esp_openthread_get_dnsserver_addr_with_type(ip6_addr_t *dnsserver_addr,
                                                      esp_netif_dns_type_t dns_type);

/**
 * @brief This function configures the main DNS server address for OpenThread netif.
 *
 * @param[in] dnsserver_addr      The dns server address.
 *
 * @return
 *      - ESP_OK on sussess
 *      - ESP_ERR_INVALID_ARG if dnsserver_addr is invalid
 *      - ESP_ERR_ESP_NETIF_IF_NOT_READY if openthread network interface is not initialized
 *      - ESP_ERR_ESP_NETIF_INVALID_PARAMS if failed to call esp_netif APIs setting dns info
 */
esp_err_t esp_openthread_set_dnsserver_addr(const ip6_addr_t dnsserver_addr);

/**
 * @brief This function configures the DNS server address for OpenThread netif.
 *
 * @param[in] dnsserver_addr      The dns server address.
 * @param[in] dns_type             The type of DNS server
 *
 * @return
 *      - ESP_OK on sussess
 *      - ESP_ERR_INVALID_ARG if dnsserver_addr is invalid
 *      - ESP_ERR_ESP_NETIF_IF_NOT_READY if openthread network interface is not initialized
 *      - ESP_ERR_ESP_NETIF_INVALID_PARAMS if failed to call esp_netif APIs setting dns info
 */
esp_err_t esp_openthread_set_dnsserver_addr_with_type(const ip6_addr_t dnsserver_addr,
                                                      esp_netif_dns_type_t dns_type);

/**
 * @brief This function acquires the NAT64 prefix in the Thread network.
 *
 * @param[out]  nat64_prefix    The NAT64 prefix output.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NOT_FOUND if NAT64 prefix available
 *
 */
esp_err_t esp_openthread_get_nat64_prefix(ip6_addr_t *nat64_prefix);


/**
 * @brief The alternative function for gethostbyname and adds the NAT64 prefix.
 *
 */
struct hostent *esp_openthread_gethostbyname_dns64(const char *name);

/**
 * @brief The alternative function for getaddrinfo and adds the NAT64 prefix.
 *
 */
int esp_openthread_getaddrinfo_dns64(const char *nodename, const char *servname, const struct addrinfo *hints, struct addrinfo **res);

#ifdef __cplusplus
}
#endif
