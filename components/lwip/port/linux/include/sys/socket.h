/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef LWIP_HDR_LINUX_SYS_SOCKETS_H
#define LWIP_HDR_LINUX_SYS_SOCKETS_H
/* Include lwip sockets by default */
#include "lwip/sockets.h"
#else
/* Otherwise use system sockets if LWIP_HDR_LINUX_SYS_SOCKETS_H already defined */
#include_next <sys/socket.h>
#endif /* LWIP_HDR_LINUX_SYS_SOCKETS_H */
