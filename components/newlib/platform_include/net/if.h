/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _ESP_PLATFORM_NET_IF_H_
#define _ESP_PLATFORM_NET_IF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "lwip/sockets.h"
#include "lwip/if_api.h"

#define	MSG_DONTROUTE   0x4     /* send without using routing tables */
#define	SOCK_SEQPACKET  5       /* sequenced packet stream */
#define	MSG_EOR         0x8     /* data completes record */
#define	SOCK_SEQPACKET  5       /* sequenced packet stream */
#define	SOMAXCONN       128

#define	IPV6_UNICAST_HOPS   4  /* int; IP6 hops */

#define	NI_MAXHOST  1025
#define	NI_MAXSERV  32
#define	NI_NUMERICSERV  0x00000008
#define	NI_DGRAM        0x00000010

typedef u32_t socklen_t;


unsigned int if_nametoindex(const char *ifname);

char *if_indextoname(unsigned int ifindex, char *ifname);

#ifdef __cplusplus
}
#endif

#endif // _ESP_PLATFORM_NET_IF_H_
