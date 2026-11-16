/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IN_H_
#define IN_H_

#include "lwip/inet.h"
/* POSIX requires <netinet/in.h> to define sockaddr_in, sockaddr_in6, sa_family_t
 * and in_port_t, which lwIP declares in lwip/sockets.h. glibc and musl include
 * <sys/socket.h> here too. */
#include <sys/socket.h>

#endif /* IN_H_ */
