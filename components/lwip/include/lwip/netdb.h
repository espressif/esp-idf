/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stddef.h>
#include_next "lwip/netdb.h"
#include "sdkconfig.h"
#if CONFIG_LWIP_USE_ESP_GETADDRINFO
#include_next "esp_netdb.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

static inline int gethostbyname_r(const char *name, struct hostent *ret, char *buf, size_t buflen, struct hostent **result, int *h_errnop)
{ return lwip_gethostbyname_r(name, ret, buf, buflen, result, h_errnop); }
static inline struct hostent *gethostbyname(const char *name)
{ return lwip_gethostbyname(name); }
static inline void freeaddrinfo(struct addrinfo *ai)
{ lwip_freeaddrinfo(ai); }
static inline int getaddrinfo(const char *nodename, const char *servname, const struct addrinfo *hints, struct addrinfo **res)
#ifdef CONFIG_LWIP_USE_ESP_GETADDRINFO
{ return esp_getaddrinfo(nodename, servname, hints, res); }
#else
{ return lwip_getaddrinfo(nodename, servname, hints, res); }
#endif

#ifdef __cplusplus
}
#endif
