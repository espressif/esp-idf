/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#ifdef LWIP_HDR_LINUX_SYS_SOCKETS_H
/* only if we prefer linux system sockets, include from system paths */
#include_next <sys/socket.h>
#else
/* Include lwip sockets by default */
#include "lwip/sockets.h"
#endif /* LWIP_HDR_LINUX_SYS_SOCKETS_H */
