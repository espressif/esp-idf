// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _ESP_PLATFORM_NET_IF_H_
#define _ESP_PLATFORM_NET_IF_H_

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

#endif // _ESP_PLATFORM_NET_IF_H_
