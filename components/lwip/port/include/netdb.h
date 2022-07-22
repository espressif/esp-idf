/**
 * @file
 * This file is a posix wrapper for lwip/netdb.h.
 */

/*
 * SPDX-FileCopyrightText: 2001-2004 Swedish Institute of Computer Science
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * SPDX-FileContributor: 2018-2022 Espressif Systems (Shanghai) CO LTD
 */

#include "lwip/netdb.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ESP_PLATFORM
int getnameinfo(const struct sockaddr *addr, socklen_t addrlen,
                char *host, socklen_t hostlen,
                char *serv, socklen_t servlen, int flags);

#endif

#ifdef __cplusplus
}
#endif
