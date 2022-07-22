/**
 * @file
 * This file is a posix wrapper for lwip/sockets.h.
 */
/*
 * SPDX-FileCopyrightText: 2001-2004 Swedish Institute of Computer Science
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * SPDX-FileContributor: 2018-2022 Espressif Systems (Shanghai) CO LTD
 */
#ifndef LWIP_HDR_SYS_SOCKETS_H
#define LWIP_HDR_SYS_SOCKETS_H

#include "lwip/sockets.h"
/*
	SOMAXCONN is expected to be found in this header too,
	while for ESP32 port is defined in net/if.h
*/
#include <net/if.h>

#endif /* LWIP_HDR_SYS_SOCKETS_H */
