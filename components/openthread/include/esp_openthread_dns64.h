/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_openthread.h"
#include "lwip/netdb.h"

#ifdef __cplusplus
extern "C" {
#endif
// The network data change callback sets the dns server address of index 0, while the CLI sets the dns server address of index 1.
#define OPENTHREAD_DNS_SERVER_INDEX 0

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
 * @brief This function acquires the DNS server address.
 *
 * @param[out] dnsserver_addr      The dns server address.
 *
 * @return
 *      - ESP_OK on sussess
 *      - ESP_ERR_INVALID_ARG if dnsserver_addr is NULL
 *      - ESP_ERR_INVALID_STATE if dns sever address not available
 */
esp_err_t esp_openthread_get_dnsserver_addr(ip6_addr_t *dnsserver_addr);

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
