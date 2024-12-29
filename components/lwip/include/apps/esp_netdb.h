/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_NETDB_H__
#define __ESP_NETDB_H__

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Custom getaddrinfo() wrapper for lwIP that handles AF_UNSPEC correctly.
 *
 * Resolves both IPv4 and IPv6 addresses when AF_UNSPEC is specified. Works
 * even if only one protocol (IPv4 or IPv6) is enabled in lwIP. Merges results
 * if both protocols are available.
 *
 * @return 0 on success, or an error code on failure.
 *         - `EAI_FAMILY`: Address family not supported.
 *
 * @note Caller must free the result list with freeaddrinfo().
 *
 * @see getaddrinfo(), freeaddrinfo()
 */
#if CONFIG_LWIP_USE_ESP_GETADDRINFO
int esp_getaddrinfo(const char *nodename, const char *servname,
                        const struct addrinfo *hints, struct addrinfo **res);
#endif

#ifdef __cplusplus
}
#endif

#endif // __ESP_NETDB_H__
