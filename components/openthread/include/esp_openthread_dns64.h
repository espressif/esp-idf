/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_openthread.h"
#include "lwip/netdb.h"

#ifdef __cplusplus
extern "C" {
#endif

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
