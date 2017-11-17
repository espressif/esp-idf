/**
 * @file
 *
 * lwIP Options Configuration
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef LWIP_HDR_OPT_H
#define LWIP_HDR_OPT_H

/*
 * Include user defined options first. Anything not defined in these files
 * will be set to standard values. Override anything you don't like!
 */
#include "lwipopts.h"
#include "lwip/debug.h"

/*
   -----------------------------------------------
   ---------- Platform specific locking ----------
   -----------------------------------------------
*/

/**
 * SYS_LIGHTWEIGHT_PROT==1: if you want inter-task protection for certain
 * critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#ifndef SYS_LIGHTWEIGHT_PROT
#define SYS_LIGHTWEIGHT_PROT            0
#endif

/**
 * NO_SYS==1: Provides VERY minimal functionality. Otherwise,
 * use lwIP facilities.
 */
#ifndef NO_SYS
#define NO_SYS                          0
#endif

/**
 * NO_SYS_NO_TIMERS==1: Drop support for sys_timeout when NO_SYS==1
 * Mainly for compatibility to old versions.
 */
#ifndef NO_SYS_NO_TIMERS
#define NO_SYS_NO_TIMERS                0
#endif

/**
 * MEMCPY: override this if you have a faster implementation at hand than the
 * one included in your C library
 */
#ifndef MEMCPY
#define MEMCPY(dst,src,len)             memcpy(dst,src,len)
#endif

/**
 * SMEMCPY: override this with care! Some compilers (e.g. gcc) can inline a
 * call to memcpy() if the length is known at compile time and is small.
 */
#ifndef SMEMCPY
#define SMEMCPY(dst,src,len)            memcpy(dst,src,len)
#endif

/**
 * LWIP_MPU_COMPATIBLE: enables special memory management mechanism
 * which makes lwip able to work on MPU (Memory Protection Unit) system
 * by not passing stack-pointers to other threads
 * (this decreases performance)
 */
#ifndef LWIP_MPU_COMPATIBLE
#define LWIP_MPU_COMPATIBLE             0
#endif

/*
   ------------------------------------
   ---------- Memory options ----------
   ------------------------------------
*/
/**
 * MEM_LIBC_MALLOC==1: Use malloc/free/realloc provided by your C-library
 * instead of the lwip internal allocator. Can save code size if you
 * already use it.
 */
#ifndef MEM_LIBC_MALLOC
#define MEM_LIBC_MALLOC                 0
#endif

/**
* MEMP_MEM_MALLOC==1: Use mem_malloc/mem_free instead of the lwip pool allocator.
* Especially useful with MEM_LIBC_MALLOC but handle with care regarding execution
* speed and usage from interrupts!
*/
#ifndef MEMP_MEM_MALLOC
#define MEMP_MEM_MALLOC                 0
#endif

/**
 * MEM_ALIGNMENT: should be set to the alignment of the CPU
 *    4 byte alignment -> #define MEM_ALIGNMENT 4
 *    2 byte alignment -> #define MEM_ALIGNMENT 2
 */
#ifndef MEM_ALIGNMENT
#define MEM_ALIGNMENT                   1
#endif

/**
 * MEM_SIZE: the size of the heap memory. If the application will send
 * a lot of data that needs to be copied, this should be set high.
 */
#ifndef MEM_SIZE
#define MEM_SIZE                        1600
#endif

/**
 * MEMP_OVERFLOW_CHECK: memp overflow protection reserves a configurable
 * amount of bytes before and after each memp element in every pool and fills
 * it with a prominent default value.
 *    MEMP_OVERFLOW_CHECK == 0 no checking
 *    MEMP_OVERFLOW_CHECK == 1 checks each element when it is freed
 *    MEMP_OVERFLOW_CHECK >= 2 checks each element in every pool every time
 *      memp_malloc() or memp_free() is called (useful but slow!)
 */
#ifndef MEMP_OVERFLOW_CHECK
#define MEMP_OVERFLOW_CHECK             0
#endif

/**
 * MEMP_SANITY_CHECK==1: run a sanity check after each memp_free() to make
 * sure that there are no cycles in the linked lists.
 */
#ifndef MEMP_SANITY_CHECK
#define MEMP_SANITY_CHECK               0
#endif

/**
 * MEM_USE_POOLS==1: Use an alternative to malloc() by allocating from a set
 * of memory pools of various sizes. When mem_malloc is called, an element of
 * the smallest pool that can provide the length needed is returned.
 * To use this, MEMP_USE_CUSTOM_POOLS also has to be enabled.
 */
#ifndef MEM_USE_POOLS
#define MEM_USE_POOLS                   0
#endif

/**
 * MEM_USE_POOLS_TRY_BIGGER_POOL==1: if one malloc-pool is empty, try the next
 * bigger pool - WARNING: THIS MIGHT WASTE MEMORY but it can make a system more
 * reliable. */
#ifndef MEM_USE_POOLS_TRY_BIGGER_POOL
#define MEM_USE_POOLS_TRY_BIGGER_POOL   0
#endif

/**
 * MEMP_USE_CUSTOM_POOLS==1: whether to include a user file lwippools.h
 * that defines additional pools beyond the "standard" ones required
 * by lwIP. If you set this to 1, you must have lwippools.h in your
 * include path somewhere.
 */
#ifndef MEMP_USE_CUSTOM_POOLS
#define MEMP_USE_CUSTOM_POOLS           0
#endif

/**
 * Set this to 1 if you want to free PBUF_RAM pbufs (or call mem_free()) from
 * interrupt context (or another context that doesn't allow waiting for a
 * semaphore).
 * If set to 1, mem_malloc will be protected by a semaphore and SYS_ARCH_PROTECT,
 * while mem_free will only use SYS_ARCH_PROTECT. mem_malloc SYS_ARCH_UNPROTECTs
 * with each loop so that mem_free can run.
 *
 * ATTENTION: As you can see from the above description, this leads to dis-/
 * enabling interrupts often, which can be slow! Also, on low memory, mem_malloc
 * can need longer.
 *
 * If you don't want that, at least for NO_SYS=0, you can still use the following
 * functions to enqueue a deallocation call which then runs in the tcpip_thread
 * context:
 * - pbuf_free_callback(p);
 * - mem_free_callback(m);
 */
#ifndef LWIP_ALLOW_MEM_FREE_FROM_OTHER_CONTEXT
#define LWIP_ALLOW_MEM_FREE_FROM_OTHER_CONTEXT 0
#endif

/*
   ------------------------------------------------
   ---------- Internal Memory Pool Sizes ----------
   ------------------------------------------------
*/
/**
 * MEMP_NUM_PBUF: the number of memp struct pbufs (used for PBUF_ROM and PBUF_REF).
 * If the application sends a lot of data out of ROM (or other static memory),
 * this should be set high.
 */
#ifndef MEMP_NUM_PBUF
#define MEMP_NUM_PBUF                   16
#endif

/**
 * MEMP_NUM_RAW_PCB: Number of raw connection PCBs
 * (requires the LWIP_RAW option)
 */
#ifndef MEMP_NUM_RAW_PCB
#define MEMP_NUM_RAW_PCB                4
#endif

/**
 * MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
 * per active UDP "connection".
 * (requires the LWIP_UDP option)
 */
#ifndef MEMP_NUM_UDP_PCB
#define MEMP_NUM_UDP_PCB                4
#endif

/**
 * MEMP_NUM_TCP_PCB: the number of simultaneously active TCP connections.
 * (requires the LWIP_TCP option)
 */
#ifndef MEMP_NUM_TCP_PCB
#define MEMP_NUM_TCP_PCB                5
#endif

/**
 * MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP connections.
 * (requires the LWIP_TCP option)
 */
#ifndef MEMP_NUM_TCP_PCB_LISTEN
#define MEMP_NUM_TCP_PCB_LISTEN         8
#endif

/**
 * MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP segments.
 * (requires the LWIP_TCP option)
 */
#ifndef MEMP_NUM_TCP_SEG
#define MEMP_NUM_TCP_SEG                16
#endif

/**
 * MEMP_NUM_REASSDATA: the number of IP packets simultaneously queued for
 * reassembly (whole packets, not fragments!)
 */
#ifndef MEMP_NUM_REASSDATA
#define MEMP_NUM_REASSDATA              5
#endif

/**
 * MEMP_NUM_FRAG_PBUF: the number of IP fragments simultaneously sent
 * (fragments, not whole packets!).
 * This is only used with IP_FRAG_USES_STATIC_BUF==0 and
 * LWIP_NETIF_TX_SINGLE_PBUF==0 and only has to be > 1 with DMA-enabled MACs
 * where the packet is not yet sent when netif->output returns.
 */
#ifndef MEMP_NUM_FRAG_PBUF
#define MEMP_NUM_FRAG_PBUF              15
#endif

/**
 * MEMP_NUM_ARP_QUEUE: the number of simultaneously queued outgoing
 * packets (pbufs) that are waiting for an ARP request (to resolve
 * their destination address) to finish.
 * (requires the ARP_QUEUEING option)
 */
#ifndef MEMP_NUM_ARP_QUEUE
#define MEMP_NUM_ARP_QUEUE              30
#endif

/**
 * MEMP_NUM_IGMP_GROUP: The number of multicast groups whose network interfaces
 * can be members at the same time (one per netif - allsystems group -, plus one
 * per netif membership).
 * (requires the LWIP_IGMP option)
 */
#ifndef MEMP_NUM_IGMP_GROUP
#define MEMP_NUM_IGMP_GROUP             8
#endif

/**
 * MEMP_NUM_SYS_TIMEOUT: the number of simultaneously active timeouts.
 * The default number of timeouts is calculated here for all enabled modules.
 * The formula expects settings to be either '0' or '1'.
 */
#ifndef MEMP_NUM_SYS_TIMEOUT
#define MEMP_NUM_SYS_TIMEOUT            (LWIP_TCP + IP_REASSEMBLY + LWIP_ARP + (2*LWIP_DHCP) + LWIP_AUTOIP + LWIP_IGMP + LWIP_DNS + (PPP_SUPPORT*6*MEMP_NUM_PPP_PCB) + (LWIP_IPV6 ? (1 + LWIP_IPV6_REASS + LWIP_IPV6_MLD) : 0))
#endif

/**
 * MEMP_NUM_NETBUF: the number of struct netbufs.
 * (only needed if you use the sequential API, like api_lib.c)
 */
#ifndef MEMP_NUM_NETBUF
#define MEMP_NUM_NETBUF                 2
#endif

/**
 * MEMP_NUM_NETCONN: the number of struct netconns.
 * (only needed if you use the sequential API, like api_lib.c)
 */
#ifndef MEMP_NUM_NETCONN
#define MEMP_NUM_NETCONN                4
#endif

/**
 * MEMP_NUM_TCPIP_MSG_API: the number of struct tcpip_msg, which are used
 * for callback/timeout API communication.
 * (only needed if you use tcpip.c)
 */
#ifndef MEMP_NUM_TCPIP_MSG_API
#define MEMP_NUM_TCPIP_MSG_API          8
#endif

/**
 * MEMP_NUM_TCPIP_MSG_INPKT: the number of struct tcpip_msg, which are used
 * for incoming packets.
 * (only needed if you use tcpip.c)
 */
#ifndef MEMP_NUM_TCPIP_MSG_INPKT
#define MEMP_NUM_TCPIP_MSG_INPKT        8
#endif

/**
 * MEMP_NUM_NETDB: the number of concurrently running lwip_addrinfo() calls
 * (before freeing the corresponding memory using lwip_freeaddrinfo()).
 */
#ifndef MEMP_NUM_NETDB
#define MEMP_NUM_NETDB                  1
#endif

/**
 * MEMP_NUM_LOCALHOSTLIST: the number of host entries in the local host list
 * if DNS_LOCAL_HOSTLIST_IS_DYNAMIC==1.
 */
#ifndef MEMP_NUM_LOCALHOSTLIST
#define MEMP_NUM_LOCALHOSTLIST          1
#endif

/**
 * MEMP_NUM_PPP_PCB: the number of simultaneously active PPP
 * connections (requires the PPP_SUPPORT option)
 */
#ifndef MEMP_NUM_PPP_PCB
#define MEMP_NUM_PPP_PCB       1
#endif

/**
 * MEMP_NUM_PPPOS_INTERFACES: the number of concurrently active PPPoS
 * interfaces (only used with PPPOS_SUPPORT==1)
 */
#ifndef MEMP_NUM_PPPOS_INTERFACES
#define MEMP_NUM_PPPOS_INTERFACES       MEMP_NUM_PPP_PCB
#endif

/**
 * MEMP_NUM_PPPOE_INTERFACES: the number of concurrently active PPPoE
 * interfaces (only used with PPPOE_SUPPORT==1)
 */
#ifndef MEMP_NUM_PPPOE_INTERFACES
#define MEMP_NUM_PPPOE_INTERFACES       1
#endif

/**
 * MEMP_NUM_PPPOL2TP_INTERFACES: the number of concurrently active PPPoL2TP
 * interfaces (only used with PPPOL2TP_SUPPORT==1)
 */
#ifndef MEMP_NUM_PPPOL2TP_INTERFACES
#define MEMP_NUM_PPPOL2TP_INTERFACES       1
#endif

/**
 * PBUF_POOL_SIZE: the number of buffers in the pbuf pool.
 */
#ifndef PBUF_POOL_SIZE
#define PBUF_POOL_SIZE                  16
#endif

/** MEMP_NUM_API_MSG: the number of concurrently active calls to various
 * socket, netconn, and tcpip functions
 */
#ifndef MEMP_NUM_API_MSG
#define MEMP_NUM_API_MSG                MEMP_NUM_TCPIP_MSG_API
#endif

/** MEMP_NUM_DNS_API_MSG: the number of concurrently active calls to netconn_gethostbyname
 */
#ifndef MEMP_NUM_DNS_API_MSG
#define MEMP_NUM_DNS_API_MSG            MEMP_NUM_TCPIP_MSG_API
#endif

/** MEMP_NUM_SOCKET_SETGETSOCKOPT_DATA: the number of concurrently active calls
 * to getsockopt/setsockopt
 */
#ifndef MEMP_NUM_SOCKET_SETGETSOCKOPT_DATA
#define MEMP_NUM_SOCKET_SETGETSOCKOPT_DATA MEMP_NUM_TCPIP_MSG_API
#endif

/** MEMP_NUM_NETIFAPI_MSG: the number of concurrently active calls to the
 * netifapi functions
 */
#ifndef MEMP_NUM_NETIFAPI_MSG
#define MEMP_NUM_NETIFAPI_MSG           MEMP_NUM_TCPIP_MSG_API
#endif

/*
   ---------------------------------
   ---------- ARP options ----------
   ---------------------------------
*/
/**
 * LWIP_ARP==1: Enable ARP functionality.
 */
#ifndef LWIP_ARP
#define LWIP_ARP                        1
#endif

/**
 * ARP_TABLE_SIZE: Number of active MAC-IP address pairs cached.
 */
#ifndef ARP_TABLE_SIZE
#define ARP_TABLE_SIZE                  10
#endif

/** the time an ARP entry stays valid after its last update,
 *  for ARP_TMR_INTERVAL = 1000, this is
 *  (60 * 5) seconds = 5 minutes.
 */
#ifndef ARP_MAXAGE
#define ARP_MAXAGE                      300
#endif

/**
 * ARP_QUEUEING==1: Multiple outgoing packets are queued during hardware address
 * resolution. By default, only the most recent packet is queued per IP address.
 * This is sufficient for most protocols and mainly reduces TCP connection
 * startup time. Set this to 1 if you know your application sends more than one
 * packet in a row to an IP address that is not in the ARP cache.
 */
#ifndef ARP_QUEUEING
#define ARP_QUEUEING                    0
#endif

/** The maximum number of packets which may be queued for each
 *  unresolved address by other network layers. Defaults to 3, 0 means disabled.
 *  Old packets are dropped, new packets are queued.
 */
#ifndef ARP_QUEUE_LEN
#define ARP_QUEUE_LEN                   3
#endif

/**
 * ETHARP_TRUST_IP_MAC==1: Incoming IP packets cause the ARP table to be
 * updated with the source MAC and IP addresses supplied in the packet.
 * You may want to disable this if you do not trust LAN peers to have the
 * correct addresses, or as a limited approach to attempt to handle
 * spoofing. If disabled, lwIP will need to make a new ARP request if
 * the peer is not already in the ARP table, adding a little latency.
 * The peer *is* in the ARP table if it requested our address before.
 * Also notice that this slows down input processing of every IP packet!
 */
#ifndef ETHARP_TRUST_IP_MAC
#define ETHARP_TRUST_IP_MAC             0
#endif

/**
 * ETHARP_SUPPORT_VLAN==1: support receiving and sending ethernet packets with
 * VLAN header. See the description of LWIP_HOOK_VLAN_CHECK and
 * LWIP_HOOK_VLAN_SET hooks to check/set VLAN headers.
 * Additionally, you can define ETHARP_VLAN_CHECK to an u16_t VLAN ID to check.
 * If ETHARP_VLAN_CHECK is defined, only VLAN-traffic for this VLAN is accepted.
 * If ETHARP_VLAN_CHECK is not defined, all traffic is accepted.
 * Alternatively, define a function/define ETHARP_VLAN_CHECK_FN(eth_hdr, vlan)
 * that returns 1 to accept a packet or 0 to drop a packet.
 */
#ifndef ETHARP_SUPPORT_VLAN
#define ETHARP_SUPPORT_VLAN             0
#endif

/** LWIP_ETHERNET==1: enable ethernet support for PPPoE even though ARP
 * might be disabled
 */
#ifndef LWIP_ETHERNET
#define LWIP_ETHERNET                   (LWIP_ARP || PPPOE_SUPPORT)
#endif

/** ETH_PAD_SIZE: number of bytes added before the ethernet header to ensure
 * alignment of payload after that header. Since the header is 14 bytes long,
 * without this padding e.g. addresses in the IP header will not be aligned
 * on a 32-bit boundary, so setting this to 2 can speed up 32-bit-platforms.
 */
#ifndef ETH_PAD_SIZE
#define ETH_PAD_SIZE                    0
#endif

/** ETHARP_SUPPORT_STATIC_ENTRIES==1: enable code to support static ARP table
 * entries (using etharp_add_static_entry/etharp_remove_static_entry).
 */
#ifndef ETHARP_SUPPORT_STATIC_ENTRIES
#define ETHARP_SUPPORT_STATIC_ENTRIES   0
#endif

/** ETHARP_TABLE_MATCH_NETIF==1: Match netif for ARP table entries.
 * If disabled, duplicate IP address on multiple netifs are not supported
 * (but this should only occur for AutoIP).
 */
#ifndef ETHARP_TABLE_MATCH_NETIF
#define ETHARP_TABLE_MATCH_NETIF        0
#endif

/*
   --------------------------------
   ---------- IP options ----------
   --------------------------------
*/
/**
 * LWIP_IPV4==1: Enable IPv4
 */
#ifndef LWIP_IPV4
#define LWIP_IPV4                       1
#endif

/**
 * IP_FORWARD==1: Enables the ability to forward IP packets across network
 * interfaces. If you are going to run lwIP on a device with only one network
 * interface, define this to 0.
 */
#ifndef IP_FORWARD
#define IP_FORWARD                      0
#endif

/**
 * IP_REASSEMBLY==1: Reassemble incoming fragmented IP packets. Note that
 * this option does not affect outgoing packet sizes, which can be controlled
 * via IP_FRAG.
 */
#ifndef IP_REASSEMBLY
#define IP_REASSEMBLY                   1
#endif

/**
 * IP_FRAG==1: Fragment outgoing IP packets if their size exceeds MTU. Note
 * that this option does not affect incoming packet sizes, which can be
 * controlled via IP_REASSEMBLY.
 */
#ifndef IP_FRAG
#define IP_FRAG                         1
#endif

#if !LWIP_IPV4
/* disable IPv4 extensions when IPv4 is disabled */
#undef IP_FORWARD
#define IP_FORWARD                      0
#undef IP_REASSEMBLY
#define IP_REASSEMBLY                   0
#undef IP_FRAG
#define IP_FRAG                         0
#endif /* !LWIP_IPV4 */

/**
 * IP_OPTIONS_ALLOWED: Defines the behavior for IP options.
 *      IP_OPTIONS_ALLOWED==0: All packets with IP options are dropped.
 *      IP_OPTIONS_ALLOWED==1: IP options are allowed (but not parsed).
 */
#ifndef IP_OPTIONS_ALLOWED
#define IP_OPTIONS_ALLOWED              1
#endif

/**
 * IP_REASS_MAXAGE: Maximum time (in multiples of IP_TMR_INTERVAL - so seconds, normally)
 * a fragmented IP packet waits for all fragments to arrive. If not all fragments arrived
 * in this time, the whole packet is discarded.
 */
#ifndef IP_REASS_MAXAGE
#define IP_REASS_MAXAGE                 3
#endif

/**
 * IP_REASS_MAX_PBUFS: Total maximum amount of pbufs waiting to be reassembled.
 * Since the received pbufs are enqueued, be sure to configure
 * PBUF_POOL_SIZE > IP_REASS_MAX_PBUFS so that the stack is still able to receive
 * packets even if the maximum amount of fragments is enqueued for reassembly!
 */
#ifndef IP_REASS_MAX_PBUFS
#define IP_REASS_MAX_PBUFS              10
#endif

/**
 * IP_FRAG_USES_STATIC_BUF==1: Use a static MTU-sized buffer for IP
 * fragmentation. Otherwise pbufs are allocated and reference the original
 * packet data to be fragmented (or with LWIP_NETIF_TX_SINGLE_PBUF==1,
 * new PBUF_RAM pbufs are used for fragments).
 * ATTENTION: IP_FRAG_USES_STATIC_BUF==1 may not be used for DMA-enabled MACs!
 */
#ifndef IP_FRAG_USES_STATIC_BUF
#define IP_FRAG_USES_STATIC_BUF         0
#endif

/**
 * IP_FRAG_MAX_MTU: Assumed max MTU on any interface for IP frag buffer
 * (requires IP_FRAG_USES_STATIC_BUF==1)
 */
#if IP_FRAG_USES_STATIC_BUF && !defined(IP_FRAG_MAX_MTU)
#define IP_FRAG_MAX_MTU                 1500
#endif

/**
 * IP_DEFAULT_TTL: Default value for Time-To-Live used by transport layers.
 */
#ifndef IP_DEFAULT_TTL
#define IP_DEFAULT_TTL                  255
#endif

/**
 * IP_SOF_BROADCAST=1: Use the SOF_BROADCAST field to enable broadcast
 * filter per pcb on udp and raw send operations. To enable broadcast filter
 * on recv operations, you also have to set IP_SOF_BROADCAST_RECV=1.
 */
#ifndef IP_SOF_BROADCAST
#define IP_SOF_BROADCAST                0
#endif

/**
 * IP_SOF_BROADCAST_RECV (requires IP_SOF_BROADCAST=1) enable the broadcast
 * filter on recv operations.
 */
#ifndef IP_SOF_BROADCAST_RECV
#define IP_SOF_BROADCAST_RECV           0
#endif

/**
 * IP_FORWARD_ALLOW_TX_ON_RX_NETIF==1: allow ip_forward() to send packets back
 * out on the netif where it was received. This should only be used for
 * wireless networks.
 * ATTENTION: When this is 1, make sure your netif driver correctly marks incoming
 * link-layer-broadcast/multicast packets as such using the corresponding pbuf flags!
 */
#ifndef IP_FORWARD_ALLOW_TX_ON_RX_NETIF
#define IP_FORWARD_ALLOW_TX_ON_RX_NETIF 0
#endif

/**
 * LWIP_RANDOMIZE_INITIAL_LOCAL_PORTS==1: randomize the local port for the first
 * local TCP/UDP pcb (default==0). This can prevent creating predictable port
 * numbers after booting a device.
 */
#ifndef LWIP_RANDOMIZE_INITIAL_LOCAL_PORTS
#define LWIP_RANDOMIZE_INITIAL_LOCAL_PORTS 0
#endif

/*
   ----------------------------------
   ---------- ICMP options ----------
   ----------------------------------
*/
/**
 * LWIP_ICMP==1: Enable ICMP module inside the IP stack.
 * Be careful, disable that make your product non-compliant to RFC1122
 */
#ifndef LWIP_ICMP
#define LWIP_ICMP                       1
#endif

/**
 * ICMP_TTL: Default value for Time-To-Live used by ICMP packets.
 */
#ifndef ICMP_TTL
#define ICMP_TTL                       (IP_DEFAULT_TTL)
#endif

/**
 * LWIP_BROADCAST_PING==1: respond to broadcast pings (default is unicast only)
 */
#ifndef LWIP_BROADCAST_PING
#define LWIP_BROADCAST_PING             0
#endif

/**
 * LWIP_MULTICAST_PING==1: respond to multicast pings (default is unicast only)
 */
#ifndef LWIP_MULTICAST_PING
#define LWIP_MULTICAST_PING             0
#endif

/*
   ---------------------------------
   ---------- RAW options ----------
   ---------------------------------
*/
/**
 * LWIP_RAW==1: Enable application layer to hook into the IP layer itself.
 */
#ifndef LWIP_RAW
#define LWIP_RAW                        0
#endif

/**
 * LWIP_RAW==1: Enable application layer to hook into the IP layer itself.
 */
#ifndef RAW_TTL
#define RAW_TTL                        (IP_DEFAULT_TTL)
#endif

/*
   ----------------------------------
   ---------- DHCP options ----------
   ----------------------------------
*/
/**
 * LWIP_DHCP==1: Enable DHCP module.
 */
#ifndef LWIP_DHCP
#define LWIP_DHCP                       0
#endif
#if !LWIP_IPV4
/* disable DHCP when IPv4 is disabled */
#undef LWIP_DHCP
#define LWIP_DHCP                       0
#endif /* !LWIP_IPV4 */

/**
 * DHCP_DOES_ARP_CHECK==1: Do an ARP check on the offered address.
 */
#ifndef DHCP_DOES_ARP_CHECK
#define DHCP_DOES_ARP_CHECK             ((LWIP_DHCP) && (LWIP_ARP))
#endif

/**
 * LWIP_DHCP_CHECK_LINK_UP==1: dhcp_start() only really starts if the netif has
 * NETIF_FLAG_LINK_UP set in its flags. As this is only an optimization and
 * netif drivers might not set this flag, the default is off. If enabled,
 * netif_set_link_up() must be called to continue dhcp starting.
 */
#ifndef LWIP_DHCP_CHECK_LINK_UP
#define LWIP_DHCP_CHECK_LINK_UP         0
#endif

/**
 * LWIP_DHCP_BOOTP_FILE==1: Store offered_si_addr and boot_file_name.
 */
#ifndef LWIP_DHCP_BOOTP_FILE
#define LWIP_DHCP_BOOTP_FILE            0
#endif

/**
 * LWIP_DHCP_GETS_NTP==1: Request NTP servers with discover/select. For each
 * response packet, an callback is called, which has to be provided by the port:
 * void dhcp_set_ntp_servers(u8_t num_ntp_servers, ip_addr_t* ntp_server_addrs);
*/
#ifndef LWIP_DHCP_GET_NTP_SRV
#define LWIP_DHCP_GET_NTP_SRV           0
#endif

/**
 * The maximum of NTP servers requested
 */
#ifndef LWIP_DHCP_MAX_NTP_SERVERS
#define LWIP_DHCP_MAX_NTP_SERVERS       1
#endif

/*
   ------------------------------------
   ---------- AUTOIP options ----------
   ------------------------------------
*/
/**
 * LWIP_AUTOIP==1: Enable AUTOIP module.
 */
#ifndef LWIP_AUTOIP
#define LWIP_AUTOIP                     0
#endif
#if !LWIP_IPV4
/* disable AUTOIP when IPv4 is disabled */
#undef LWIP_AUTOIP
#define LWIP_AUTOIP                     0
#endif /* !LWIP_IPV4 */

/**
 * LWIP_DHCP_AUTOIP_COOP==1: Allow DHCP and AUTOIP to be both enabled on
 * the same interface at the same time.
 */
#ifndef LWIP_DHCP_AUTOIP_COOP
#define LWIP_DHCP_AUTOIP_COOP           0
#endif

/**
 * LWIP_DHCP_AUTOIP_COOP_TRIES: Set to the number of DHCP DISCOVER probes
 * that should be sent before falling back on AUTOIP. This can be set
 * as low as 1 to get an AutoIP address very quickly, but you should
 * be prepared to handle a changing IP address when DHCP overrides
 * AutoIP.
 */
#ifndef LWIP_DHCP_AUTOIP_COOP_TRIES
#define LWIP_DHCP_AUTOIP_COOP_TRIES     9
#endif

/**
 * LWIP_AUTOIP_MAX_CONFLICTS:
 * Maximum number of AutoIP IP conflicts before rate limiting is enabled.
 */
#ifndef LWIP_AUTOIP_MAX_CONFLICTS
#define LWIP_AUTOIP_MAX_CONFLICTS            10
#endif

/**
 * LWIP_AUTOIP_RATE_LIMIT_INTERVAL:
 * Rate limited request interval, in seconds.
 */
#ifndef LWIP_AUTOIP_RATE_LIMIT_INTERVAL
#define LWIP_AUTOIP_RATE_LIMIT_INTERVAL      60
#endif

/*
   ----------------------------------
   ----- SNMP MIB2 support      -----
   ----------------------------------
*/
/**
 * LWIP_MIB2_CALLBACKS==1: Turn on SNMP MIB2 callbacks.
 * Turn this on to get callbacks needed to implement MIB2.
 * Usually MIB2_STATS should be enabled, too.
 */
#ifndef LWIP_MIB2_CALLBACKS
#define LWIP_MIB2_CALLBACKS             0
#endif

/*
   ----------------------------------
   ----- Multicast/IGMP options -----
   ----------------------------------
*/
/**
 * LWIP_IGMP==1: Turn on IGMP module.
 */
#ifndef LWIP_IGMP
#define LWIP_IGMP                       1
#endif
#if !LWIP_IPV4
#undef LWIP_IGMP
#define LWIP_IGMP                       0
#endif

/**
 * LWIP_MULTICAST_TX_OPTIONS==1: Enable multicast TX support like the socket options
 * IP_MULTICAST_TTL/IP_MULTICAST_IF/IP_MULTICAST_LOOP
 */
#ifndef LWIP_MULTICAST_TX_OPTIONS
#define LWIP_MULTICAST_TX_OPTIONS       LWIP_IGMP
#endif

/*
   ----------------------------------
   ---------- DNS options -----------
   ----------------------------------
*/
/**
 * LWIP_DNS==1: Turn on DNS module. UDP must be available for DNS
 * transport.
 */
#ifndef LWIP_DNS
#define LWIP_DNS                        0
#endif

/** DNS maximum number of entries to maintain locally. */
#ifndef DNS_TABLE_SIZE
#define DNS_TABLE_SIZE                  4
#endif

/** DNS maximum host name length supported in the name table. */
#ifndef DNS_MAX_NAME_LENGTH
#define DNS_MAX_NAME_LENGTH             256
#endif

/** The maximum of DNS servers
 * The first server can be initialized automatically by defining
 * DNS_SERVER_ADDRESS(ipaddr), where 'ipaddr' is an 'ip_addr_t*'
 */
#ifndef DNS_MAX_SERVERS
#define DNS_MAX_SERVERS                 2
#endif

/** DNS do a name checking between the query and the response. */
#ifndef DNS_DOES_NAME_CHECK
#define DNS_DOES_NAME_CHECK             1
#endif

/** LWIP_DNS_SECURE: controls the security level of the DNS implementation
 * Use all DNS security features by default.
 * This is overridable but should only be needed by very small targets
 * or when using against non standard DNS servers. */
#ifndef LWIP_DNS_SECURE
#define LWIP_DNS_SECURE (LWIP_DNS_SECURE_RAND_XID | LWIP_DNS_SECURE_NO_MULTIPLE_OUTSTANDING | LWIP_DNS_SECURE_RAND_SRC_PORT)
#endif
/* A list of DNS security features follows */
#define LWIP_DNS_SECURE_RAND_XID                1
#define LWIP_DNS_SECURE_NO_MULTIPLE_OUTSTANDING 2
#define LWIP_DNS_SECURE_RAND_SRC_PORT           4

/** DNS_LOCAL_HOSTLIST: Implements a local host-to-address list. If enabled,
 *  you have to define
 *    #define DNS_LOCAL_HOSTLIST_INIT {{"host1", 0x123}, {"host2", 0x234}}
 *  (an array of structs name/address, where address is an u32_t in network
 *  byte order).
 *
 *  Instead, you can also use an external function:
 *  #define DNS_LOOKUP_LOCAL_EXTERN(x) extern u32_t my_lookup_function(const char *name)
 *  that returns the IP address or INADDR_NONE if not found.
 */
#ifndef DNS_LOCAL_HOSTLIST
#define DNS_LOCAL_HOSTLIST              0
#endif /* DNS_LOCAL_HOSTLIST */

/** If this is turned on, the local host-list can be dynamically changed
 *  at runtime. */
#ifndef DNS_LOCAL_HOSTLIST_IS_DYNAMIC
#define DNS_LOCAL_HOSTLIST_IS_DYNAMIC   0
#endif /* DNS_LOCAL_HOSTLIST_IS_DYNAMIC */

/*
   ---------------------------------
   ---------- UDP options ----------
   ---------------------------------
*/
/**
 * LWIP_UDP==1: Turn on UDP.
 */
#ifndef LWIP_UDP
#define LWIP_UDP                        1
#endif

/**
 * LWIP_UDPLITE==1: Turn on UDP-Lite. (Requires LWIP_UDP)
 */
#ifndef LWIP_UDPLITE
#define LWIP_UDPLITE                    0
#endif

/**
 * UDP_TTL: Default Time-To-Live value.
 */
#ifndef UDP_TTL
#define UDP_TTL                         (IP_DEFAULT_TTL)
#endif

/**
 * LWIP_NETBUF_RECVINFO==1: append destination addr and port to every netbuf.
 */
#ifndef LWIP_NETBUF_RECVINFO
#define LWIP_NETBUF_RECVINFO            0
#endif

/*
   ---------------------------------
   ---------- TCP options ----------
   ---------------------------------
*/
/**
 * LWIP_TCP==1: Turn on TCP.
 */
#ifndef LWIP_TCP
#define LWIP_TCP                        1
#endif

/**
 * TCP_TTL: Default Time-To-Live value.
 */
#ifndef TCP_TTL
#define TCP_TTL                         (IP_DEFAULT_TTL)
#endif

/**
 * TCP_WND: The size of a TCP window.  This must be at least
 * (2 * TCP_MSS) for things to work well
 */
#ifndef TCP_WND
#define TCP_WND(pcb)                         (4 * TCP_MSS)
#endif

/**
 * TCP_MAXRTX: Maximum number of retransmissions of data segments.
 */
#ifndef TCP_MAXRTX
#define TCP_MAXRTX                      12
#endif

/**
 * TCP_SYNMAXRTX: Maximum number of retransmissions of SYN segments.
 */
#ifndef TCP_SYNMAXRTX
#define TCP_SYNMAXRTX                   6
#endif

/**
 * TCP_QUEUE_OOSEQ==1: TCP will queue segments that arrive out of order.
 * Define to 0 if your device is low on memory.
 */
#ifndef TCP_QUEUE_OOSEQ
#define TCP_QUEUE_OOSEQ                 (LWIP_TCP)
#endif

/**
 * TCP_MSS: TCP Maximum segment size. (default is 536, a conservative default,
 * you might want to increase this.)
 * For the receive side, this MSS is advertised to the remote side
 * when opening a connection. For the transmit size, this MSS sets
 * an upper limit on the MSS advertised by the remote host.
 */
#ifndef TCP_MSS
#define TCP_MSS                         536
#endif

/**
 * TCP_CALCULATE_EFF_SEND_MSS: "The maximum size of a segment that TCP really
 * sends, the 'effective send MSS,' MUST be the smaller of the send MSS (which
 * reflects the available reassembly buffer size at the remote host) and the
 * largest size permitted by the IP layer" (RFC 1122)
 * Setting this to 1 enables code that checks TCP_MSS against the MTU of the
 * netif used for a connection and limits the MSS if it would be too big otherwise.
 */
#ifndef TCP_CALCULATE_EFF_SEND_MSS
#define TCP_CALCULATE_EFF_SEND_MSS      1
#endif


/**
 * TCP_SND_BUF: TCP sender buffer space (bytes).
 * To achieve good performance, this should be at least 2 * TCP_MSS.
 */
#ifndef TCP_SND_BUF
#define TCP_SND_BUF(pcb)                     (2 * TCP_MSS)
#endif

/**
 * TCP_SND_QUEUELEN: TCP sender buffer space (pbufs). This must be at least
 * as much as (2 * TCP_SND_BUF/TCP_MSS) for things to work.
 */
#ifndef TCP_SND_QUEUELEN
#define TCP_SND_QUEUELEN(pcb)                ((4 * (TCP_SND_BUF((pcb))) + (TCP_MSS - 1))/(TCP_MSS))
#endif

/**
 * TCP_SNDLOWAT: TCP writable space (bytes). This must be less than
 * TCP_SND_BUF. It is the amount of space which must be available in the
 * TCP snd_buf for select to return writable (combined with TCP_SNDQUEUELOWAT).
 */
#ifndef TCP_SNDLOWAT
#define TCP_SNDLOWAT(pcb)                    LWIP_MIN(LWIP_MAX(((TCP_SND_BUF((pcb)))/2), (2 * TCP_MSS) + 1), (TCP_SND_BUF((pcb))) - 1)
#endif

/**
 * TCP_SNDQUEUELOWAT: TCP writable bufs (pbuf count). This must be less
 * than TCP_SND_QUEUELEN. If the number of pbufs queued on a pcb drops below
 * this number, select returns writable (combined with TCP_SNDLOWAT).
 */
#ifndef TCP_SNDQUEUELOWAT
#define TCP_SNDQUEUELOWAT(pcb)               LWIP_MAX(((TCP_SND_QUEUELEN((pcb)))/2), 5)
#endif

/**
 * TCP_OOSEQ_MAX_BYTES: The maximum number of bytes queued on ooseq per pcb.
 * Default is 0 (no limit). Only valid for TCP_QUEUE_OOSEQ==0.
 */
#ifndef TCP_OOSEQ_MAX_BYTES
#define TCP_OOSEQ_MAX_BYTES             0
#endif

/**
 * TCP_OOSEQ_MAX_PBUFS: The maximum number of pbufs queued on ooseq per pcb.
 * Default is 0 (no limit). Only valid for TCP_QUEUE_OOSEQ==0.
 */
#ifndef TCP_OOSEQ_MAX_PBUFS
#define TCP_OOSEQ_MAX_PBUFS             0
#endif

/**
 * TCP_LISTEN_BACKLOG: Enable the backlog option for tcp listen pcb.
 */
#ifndef TCP_LISTEN_BACKLOG
#define TCP_LISTEN_BACKLOG              0
#endif

/**
 * The maximum allowed backlog for TCP listen netconns.
 * This backlog is used unless another is explicitly specified.
 * 0xff is the maximum (u8_t).
 */
#ifndef TCP_DEFAULT_LISTEN_BACKLOG
#define TCP_DEFAULT_LISTEN_BACKLOG      0xff
#endif

/**
 * TCP_OVERSIZE: The maximum number of bytes that tcp_write may
 * allocate ahead of time in an attempt to create shorter pbuf chains
 * for transmission. The meaningful range is 0 to TCP_MSS. Some
 * suggested values are:
 *
 * 0:         Disable oversized allocation. Each tcp_write() allocates a new
              pbuf (old behaviour).
 * 1:         Allocate size-aligned pbufs with minimal excess. Use this if your
 *            scatter-gather DMA requires aligned fragments.
 * 128:       Limit the pbuf/memory overhead to 20%.
 * TCP_MSS:   Try to create unfragmented TCP packets.
 * TCP_MSS/4: Try to create 4 fragments or less per TCP packet.
 */
#ifndef TCP_OVERSIZE
#define TCP_OVERSIZE                    TCP_MSS
#endif

/**
 * LWIP_TCP_TIMESTAMPS==1: support the TCP timestamp option.
 * The timestamp option is currently only used to help remote hosts, it is not
 * really used locally. Therefore, it is only enabled when a TS option is
 * received in the initial SYN packet from a remote host.
 */
#ifndef LWIP_TCP_TIMESTAMPS
#define LWIP_TCP_TIMESTAMPS             0
#endif

/**
 * TCP_WND_UPDATE_THRESHOLD: difference in window to trigger an
 * explicit window update
 */
#ifndef TCP_WND_UPDATE_THRESHOLD
#define TCP_WND_UPDATE_THRESHOLD(pcb)   LWIP_MIN((TCP_WND((pcb)) / 4), (TCP_MSS * 4))
#endif

/**
 * LWIP_EVENT_API and LWIP_CALLBACK_API: Only one of these should be set to 1.
 *     LWIP_EVENT_API==1: The user defines lwip_tcp_event() to receive all
 *         events (accept, sent, etc) that happen in the system.
 *     LWIP_CALLBACK_API==1: The PCB callback function is called directly
 *         for the event. This is the default.
 */
#if !defined(LWIP_EVENT_API) && !defined(LWIP_CALLBACK_API)
#define LWIP_EVENT_API                  0
#define LWIP_CALLBACK_API               1
#endif

/**
 * LWIP_WND_SCALE and TCP_RCV_SCALE:
 * Set LWIP_WND_SCALE to 1 to enable window scaling.
 * Set TCP_RCV_SCALE to the desired scaling factor (shift count in the
 * range of [0..14]).
 * When LWIP_WND_SCALE is enabled but TCP_RCV_SCALE is 0, we can use a large
 * send window while having a small receive window only.
 */
#ifndef LWIP_WND_SCALE
#define LWIP_WND_SCALE                  0
#define TCP_RCV_SCALE                   0
#endif


/*
   ----------------------------------
   ---------- Pbuf options ----------
   ----------------------------------
*/
/**
 * PBUF_LINK_HLEN: the number of bytes that should be allocated for a
 * link level header. The default is 14, the standard value for
 * Ethernet.
 */
#ifndef PBUF_LINK_HLEN
#ifdef LWIP_HOOK_VLAN_SET
#define PBUF_LINK_HLEN                  (18 + ETH_PAD_SIZE)
#else /* LWIP_HOOK_VLAN_SET */
#define PBUF_LINK_HLEN                  (14 + ETH_PAD_SIZE)
#endif /* LWIP_HOOK_VLAN_SET */
#endif

/**
 * PBUF_LINK_ENCAPSULATION_HLEN: the number of bytes that should be allocated
 * for an additional encapsulation header before ethernet headers (e.g. 802.11)
 */
#ifndef PBUF_LINK_ENCAPSULATION_HLEN
#define PBUF_LINK_ENCAPSULATION_HLEN    0
#endif

/**
 * PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool. The default is
 * designed to accommodate single full size TCP frame in one pbuf, including
 * TCP_MSS, IP header, and link header.
 */
#ifndef PBUF_POOL_BUFSIZE
#define PBUF_POOL_BUFSIZE               LWIP_MEM_ALIGN_SIZE(TCP_MSS+40+PBUF_LINK_ENCAPSULATION_HLEN+PBUF_LINK_HLEN)
#endif

/*
   ------------------------------------------------
   ---------- Network Interfaces options ----------
   ------------------------------------------------
*/
/**
 * LWIP_NETIF_HOSTNAME==1: use DHCP_OPTION_HOSTNAME with netif's hostname
 * field.
 */
#ifndef LWIP_NETIF_HOSTNAME
#define LWIP_NETIF_HOSTNAME             0
#endif

/**
 * LWIP_NETIF_API==1: Support netif api (in netifapi.c)
 */
#ifndef LWIP_NETIF_API
#define LWIP_NETIF_API                  0
#endif

/**
 * LWIP_NETIF_STATUS_CALLBACK==1: Support a callback function whenever an interface
 * changes its up/down status (i.e., due to DHCP IP acquisition)
 */
#ifndef LWIP_NETIF_STATUS_CALLBACK
#define LWIP_NETIF_STATUS_CALLBACK      0
#endif

/**
 * LWIP_NETIF_LINK_CALLBACK==1: Support a callback function from an interface
 * whenever the link changes (i.e., link down)
 */
#ifndef LWIP_NETIF_LINK_CALLBACK
#define LWIP_NETIF_LINK_CALLBACK        0
#endif

/**
 * LWIP_NETIF_REMOVE_CALLBACK==1: Support a callback function that is called
 * when a netif has been removed
 */
#ifndef LWIP_NETIF_REMOVE_CALLBACK
#define LWIP_NETIF_REMOVE_CALLBACK      0
#endif

/**
 * LWIP_NETIF_HWADDRHINT==1: Cache link-layer-address hints (e.g. table
 * indices) in struct netif. TCP and UDP can make use of this to prevent
 * scanning the ARP table for every sent packet. While this is faster for big
 * ARP tables or many concurrent connections, it might be counterproductive
 * if you have a tiny ARP table or if there never are concurrent connections.
 */
#ifndef LWIP_NETIF_HWADDRHINT
#define LWIP_NETIF_HWADDRHINT           0
#endif

/**
 * LWIP_NETIF_LOOPBACK==1: Support sending packets with a destination IP
 * address equal to the netif IP address, looping them back up the stack.
 */
#ifndef LWIP_NETIF_LOOPBACK
#define LWIP_NETIF_LOOPBACK             0
#endif

/**
 * LWIP_LOOPBACK_MAX_PBUFS: Maximum number of pbufs on queue for loopback
 * sending for each netif (0 = disabled)
 */
#ifndef LWIP_LOOPBACK_MAX_PBUFS
#define LWIP_LOOPBACK_MAX_PBUFS         0
#endif

/**
 * LWIP_NETIF_LOOPBACK_MULTITHREADING: Indicates whether threading is enabled in
 * the system, as netifs must change how they behave depending on this setting
 * for the LWIP_NETIF_LOOPBACK option to work.
 * Setting this is needed to avoid reentering non-reentrant functions like
 * tcp_input().
 *    LWIP_NETIF_LOOPBACK_MULTITHREADING==1: Indicates that the user is using a
 *       multithreaded environment like tcpip.c. In this case, netif->input()
 *       is called directly.
 *    LWIP_NETIF_LOOPBACK_MULTITHREADING==0: Indicates a polling (or NO_SYS) setup.
 *       The packets are put on a list and netif_poll() must be called in
 *       the main application loop.
 */
#ifndef LWIP_NETIF_LOOPBACK_MULTITHREADING
#define LWIP_NETIF_LOOPBACK_MULTITHREADING    (!NO_SYS)
#endif

/**
 * LWIP_NETIF_TX_SINGLE_PBUF: if this is set to 1, lwIP tries to put all data
 * to be sent into one single pbuf. This is for compatibility with DMA-enabled
 * MACs that do not support scatter-gather.
 * Beware that this might involve CPU-memcpy before transmitting that would not
 * be needed without this flag! Use this only if you need to!
 *
 * @todo: TCP and IP-frag do not work with this, yet:
 */
#ifndef LWIP_NETIF_TX_SINGLE_PBUF
#define LWIP_NETIF_TX_SINGLE_PBUF             0
#endif /* LWIP_NETIF_TX_SINGLE_PBUF */

/*
   ------------------------------------
   ---------- LOOPIF options ----------
   ------------------------------------
*/
/**
 * LWIP_HAVE_LOOPIF==1: Support loop interface (127.0.0.1).
 * This is only needed when no real netifs are available. If at least one other
 * netif is available, loopback traffic uses this netif.
 */
#ifndef LWIP_HAVE_LOOPIF
#define LWIP_HAVE_LOOPIF                LWIP_NETIF_LOOPBACK
#endif

/**
 * LWIP_LOOPIF_MULTICAST==1: Support multicast/IGMP on loop interface (127.0.0.1).
 */
#ifndef LWIP_LOOPIF_MULTICAST
#define LWIP_LOOPIF_MULTICAST               0
#endif

/*
   ------------------------------------
   ---------- SLIPIF options ----------
   ------------------------------------
*/
/**
 * LWIP_HAVE_SLIPIF==1: Support slip interface and slipif.c
 */
#ifndef LWIP_HAVE_SLIPIF
#define LWIP_HAVE_SLIPIF                0
#endif

/*
   ------------------------------------
   ---------- Thread options ----------
   ------------------------------------
*/
/**
 * TCPIP_THREAD_NAME: The name assigned to the main tcpip thread.
 */
#ifndef TCPIP_THREAD_NAME
#define TCPIP_THREAD_NAME              "tcpip_thread"
#endif

/**
 * TCPIP_THREAD_STACKSIZE: The stack size used by the main tcpip thread.
 * The stack size value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#ifndef TCPIP_THREAD_STACKSIZE
#define TCPIP_THREAD_STACKSIZE          0
#endif

/**
 * TCPIP_THREAD_PRIO: The priority assigned to the main tcpip thread.
 * The priority value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#ifndef TCPIP_THREAD_PRIO
#define TCPIP_THREAD_PRIO               1
#endif

/**
 * TCPIP_MBOX_SIZE: The mailbox size for the tcpip thread messages
 * The queue size value itself is platform-dependent, but is passed to
 * sys_mbox_new() when tcpip_init is called.
 */
#ifndef TCPIP_MBOX_SIZE
#define TCPIP_MBOX_SIZE                 0
#endif

/**
 * SLIPIF_THREAD_NAME: The name assigned to the slipif_loop thread.
 */
#ifndef SLIPIF_THREAD_NAME
#define SLIPIF_THREAD_NAME             "slipif_loop"
#endif

/**
 * SLIP_THREAD_STACKSIZE: The stack size used by the slipif_loop thread.
 * The stack size value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#ifndef SLIPIF_THREAD_STACKSIZE
#define SLIPIF_THREAD_STACKSIZE         0
#endif

/**
 * SLIPIF_THREAD_PRIO: The priority assigned to the slipif_loop thread.
 * The priority value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#ifndef SLIPIF_THREAD_PRIO
#define SLIPIF_THREAD_PRIO              1
#endif

/**
 * DEFAULT_THREAD_NAME: The name assigned to any other lwIP thread.
 */
#ifndef DEFAULT_THREAD_NAME
#define DEFAULT_THREAD_NAME            "lwIP"
#endif

/**
 * DEFAULT_THREAD_STACKSIZE: The stack size used by any other lwIP thread.
 * The stack size value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#ifndef DEFAULT_THREAD_STACKSIZE
#define DEFAULT_THREAD_STACKSIZE        0
#endif

/**
 * DEFAULT_THREAD_PRIO: The priority assigned to any other lwIP thread.
 * The priority value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#ifndef DEFAULT_THREAD_PRIO
#define DEFAULT_THREAD_PRIO             1
#endif

/**
 * DEFAULT_RAW_RECVMBOX_SIZE: The mailbox size for the incoming packets on a
 * NETCONN_RAW. The queue size value itself is platform-dependent, but is passed
 * to sys_mbox_new() when the recvmbox is created.
 */
#ifndef DEFAULT_RAW_RECVMBOX_SIZE
#define DEFAULT_RAW_RECVMBOX_SIZE       0
#endif

/**
 * DEFAULT_UDP_RECVMBOX_SIZE: The mailbox size for the incoming packets on a
 * NETCONN_UDP. The queue size value itself is platform-dependent, but is passed
 * to sys_mbox_new() when the recvmbox is created.
 */
#ifndef DEFAULT_UDP_RECVMBOX_SIZE
#define DEFAULT_UDP_RECVMBOX_SIZE       0
#endif

/**
 * DEFAULT_TCP_RECVMBOX_SIZE: The mailbox size for the incoming packets on a
 * NETCONN_TCP. The queue size value itself is platform-dependent, but is passed
 * to sys_mbox_new() when the recvmbox is created.
 */
#ifndef DEFAULT_TCP_RECVMBOX_SIZE
#define DEFAULT_TCP_RECVMBOX_SIZE       0
#endif

/**
 * DEFAULT_ACCEPTMBOX_SIZE: The mailbox size for the incoming connections.
 * The queue size value itself is platform-dependent, but is passed to
 * sys_mbox_new() when the acceptmbox is created.
 */
#ifndef DEFAULT_ACCEPTMBOX_SIZE
#define DEFAULT_ACCEPTMBOX_SIZE         0
#endif

/*
   ----------------------------------------------
   ---------- Sequential layer options ----------
   ----------------------------------------------
*/
/**
 * LWIP_TCPIP_CORE_LOCKING: (EXPERIMENTAL!)
 * Don't use it if you're not an active lwIP project member
 */
#ifndef LWIP_TCPIP_CORE_LOCKING
#define LWIP_TCPIP_CORE_LOCKING         0
#endif

/**
 * LWIP_TCPIP_CORE_LOCKING_INPUT: (EXPERIMENTAL!)
 * Don't use it if you're not an active lwIP project member
 */
#ifndef LWIP_TCPIP_CORE_LOCKING_INPUT
#define LWIP_TCPIP_CORE_LOCKING_INPUT   0
#endif

/**
 * LWIP_NETCONN==1: Enable Netconn API (require to use api_lib.c)
 */
#ifndef LWIP_NETCONN
#define LWIP_NETCONN                    1
#endif

/** LWIP_TCPIP_TIMEOUT==1: Enable tcpip_timeout/tcpip_untimeout to create
 * timers running in tcpip_thread from another thread.
 */
#ifndef LWIP_TCPIP_TIMEOUT
#define LWIP_TCPIP_TIMEOUT              0
#endif

/** LWIP_NETCONN_SEM_PER_THREAD==1: Use one (thread-local) semaphore per
 * thread calling socket/netconn functions instead of allocating one
 * semaphore per netconn (and per select etc.)
 * ATTENTION: a thread-local semaphore for API calls is needed:
 * - LWIP_NETCONN_THREAD_SEM_GET() returning a sys_sem_t*
 * - LWIP_NETCONN_THREAD_SEM_ALLOC() creating the semaphore
 * - LWIP_NETCONN_THREAD_SEM_FREE() freeing the semaphore
 * The latter 2 can be invoked up by calling netconn_thread_init()/netconn_thread_cleanup().
 * Ports may call these for threads created with sys_thread_new().
 */
#ifndef LWIP_NETCONN_SEM_PER_THREAD
#define LWIP_NETCONN_SEM_PER_THREAD     0
#endif

/** LWIP_NETCONN_FULLDUPLEX==1: Enable code that allows reading from one thread,
 * writing from a 2nd thread and closing from a 3rd thread at the same time.
 * ATTENTION: This is currently really alpha! Some requirements:
 * - LWIP_NETCONN_SEM_PER_THREAD==1 is required to use one socket/netconn from
 *   multiple threads at once
 * - sys_mbox_free() has to unblock receive tasks waiting on recvmbox/acceptmbox
 *   and prevent a task pending on this during/after deletion
 */
#ifndef LWIP_NETCONN_FULLDUPLEX
#define LWIP_NETCONN_FULLDUPLEX         0
#endif

/*
   ------------------------------------
   ---------- Socket options ----------
   ------------------------------------
*/
/**
 * LWIP_SOCKET==1: Enable Socket API (require to use sockets.c)
 */
#ifndef LWIP_SOCKET
#define LWIP_SOCKET                     1
#endif

/* LWIP_SOCKET_SET_ERRNO==1: Set errno when socket functions cannot complete
 * successfully, as required by POSIX. Default is POSIX-compliant.
 */
#ifndef LWIP_SOCKET_SET_ERRNO
#define LWIP_SOCKET_SET_ERRNO           1
#endif

/**
 * LWIP_COMPAT_SOCKETS==1: Enable BSD-style sockets functions names through defines.
 * LWIP_COMPAT_SOCKETS==2: Same as ==1 but correctly named functions are created.
 * While this helps code completion, it might conflict with existing libraries.
 * (only used if you use sockets.c)
 */
#ifndef LWIP_COMPAT_SOCKETS
#define LWIP_COMPAT_SOCKETS             1
#endif

/**
 * LWIP_POSIX_SOCKETS_IO_NAMES==1: Enable POSIX-style sockets functions names.
 * Disable this option if you use a POSIX operating system that uses the same
 * names (read, write & close). (only used if you use sockets.c)
 */
#ifndef LWIP_POSIX_SOCKETS_IO_NAMES
#define LWIP_POSIX_SOCKETS_IO_NAMES     1
#endif

/**
 * LWIP_SOCKET_OFFSET==n: Increases the file descriptor number created by LwIP with n.
 * This can be useful when there are multiple APIs which create file descriptors.
 * When they all start with a different offset and you won't make them overlap you can
 * re implement read/write/close/ioctl/fnctl to send the requested action to the right
 * library (sharing select will need more work though).
 */
#ifndef LWIP_SOCKET_OFFSET
#define LWIP_SOCKET_OFFSET              0
#endif

/**
 * LWIP_TCP_KEEPALIVE==1: Enable TCP_KEEPIDLE, TCP_KEEPINTVL and TCP_KEEPCNT
 * options processing. Note that TCP_KEEPIDLE and TCP_KEEPINTVL have to be set
 * in seconds. (does not require sockets.c, and will affect tcp.c)
 */
#ifndef LWIP_TCP_KEEPALIVE
#define LWIP_TCP_KEEPALIVE              0
#endif

/**
 * LWIP_SO_SNDTIMEO==1: Enable send timeout for sockets/netconns and
 * SO_SNDTIMEO processing.
 */
#ifndef LWIP_SO_SNDTIMEO
#define LWIP_SO_SNDTIMEO                0
#endif

/**
 * LWIP_SO_RCVTIMEO==1: Enable receive timeout for sockets/netconns and
 * SO_RCVTIMEO processing.
 */
#ifndef LWIP_SO_RCVTIMEO
#define LWIP_SO_RCVTIMEO                0
#endif

/**
 * LWIP_SO_SNDRCVTIMEO_NONSTANDARD==1: SO_RCVTIMEO/SO_SNDTIMEO take an int
 * (milliseconds, much like winsock does) instead of a struct timeval (default).
 */
#ifndef LWIP_SO_SNDRCVTIMEO_NONSTANDARD
#define LWIP_SO_SNDRCVTIMEO_NONSTANDARD 0
#endif

/**
 * LWIP_SO_RCVBUF==1: Enable SO_RCVBUF processing.
 */
#ifndef LWIP_SO_RCVBUF
#define LWIP_SO_RCVBUF                  0
#endif

/**
 * LWIP_SO_LINGER==1: Enable SO_LINGER processing.
 */
#ifndef LWIP_SO_LINGER
#define LWIP_SO_LINGER                  0
#endif

/**
 * If LWIP_SO_RCVBUF is used, this is the default value for recv_bufsize.
 */
#ifndef RECV_BUFSIZE_DEFAULT
#define RECV_BUFSIZE_DEFAULT            INT_MAX
#endif

/**
 * By default, TCP socket/netconn close waits 20 seconds max to send the FIN
 */
#ifndef LWIP_TCP_CLOSE_TIMEOUT_MS_DEFAULT
#define LWIP_TCP_CLOSE_TIMEOUT_MS_DEFAULT 20000
#endif

/**
 * SO_REUSE==1: Enable SO_REUSEADDR option.
 */
#ifndef SO_REUSE
#define SO_REUSE                        0
#endif

/**
 * SO_REUSE_RXTOALL==1: Pass a copy of incoming broadcast/multicast packets
 * to all local matches if SO_REUSEADDR is turned on.
 * WARNING: Adds a memcpy for every packet if passing to more than one pcb!
 */
#ifndef SO_REUSE_RXTOALL
#define SO_REUSE_RXTOALL                0
#endif

/**
 * LWIP_FIONREAD_LINUXMODE==0 (default): ioctl/FIONREAD returns the amount of
 * pending data in the network buffer. This is the way windows does it. It's
 * the default for lwIP since it is smaller.
 * LWIP_FIONREAD_LINUXMODE==1: ioctl/FIONREAD returns the size of the next
 * pending datagram in bytes. This is the way linux does it. This code is only
 * here for compatibility.
 */
#ifndef LWIP_FIONREAD_LINUXMODE
#define LWIP_FIONREAD_LINUXMODE         0
#endif

/*
   ----------------------------------------
   ---------- Statistics options ----------
   ----------------------------------------
*/
/**
 * LWIP_STATS==1: Enable statistics collection in lwip_stats.
 */
#ifndef LWIP_STATS
#define LWIP_STATS                      0
#endif

#if LWIP_STATS

/**
 * LWIP_STATS_DISPLAY==1: Compile in the statistics output functions.
 */
#ifndef LWIP_STATS_DISPLAY
#define LWIP_STATS_DISPLAY              0
#endif

/**
 * LINK_STATS==1: Enable link stats.
 */
#ifndef LINK_STATS
#define LINK_STATS                      1
#endif

/**
 * ETHARP_STATS==1: Enable etharp stats.
 */
#ifndef ETHARP_STATS
#define ETHARP_STATS                    (LWIP_ARP)
#endif

/**
 * IP_STATS==1: Enable IP stats.
 */
#ifndef IP_STATS
#define IP_STATS                        1
#endif

/**
 * IPFRAG_STATS==1: Enable IP fragmentation stats. Default is
 * on if using either frag or reass.
 */
#ifndef IPFRAG_STATS
#define IPFRAG_STATS                    (IP_REASSEMBLY || IP_FRAG)
#endif

/**
 * ICMP_STATS==1: Enable ICMP stats.
 */
#ifndef ICMP_STATS
#define ICMP_STATS                      1
#endif

/**
 * IGMP_STATS==1: Enable IGMP stats.
 */
#ifndef IGMP_STATS
#define IGMP_STATS                      (LWIP_IGMP)
#endif

/**
 * UDP_STATS==1: Enable UDP stats. Default is on if
 * UDP enabled, otherwise off.
 */
#ifndef UDP_STATS
#define UDP_STATS                       (LWIP_UDP)
#endif

/**
 * TCP_STATS==1: Enable TCP stats. Default is on if TCP
 * enabled, otherwise off.
 */
#ifndef TCP_STATS
#define TCP_STATS                       (LWIP_TCP)
#endif

/**
 * MEM_STATS==1: Enable mem.c stats.
 */
#ifndef MEM_STATS
#define MEM_STATS                       ((MEM_LIBC_MALLOC == 0) && (MEM_USE_POOLS == 0))
#endif

/**
 * MEMP_STATS==1: Enable memp.c pool stats.
 */
#ifndef MEMP_STATS
#define MEMP_STATS                      (MEMP_MEM_MALLOC == 0)
#endif

/**
 * SYS_STATS==1: Enable system stats (sem and mbox counts, etc).
 */
#ifndef SYS_STATS
#define SYS_STATS                       (NO_SYS == 0)
#endif

/**
 * IP6_STATS==1: Enable IPv6 stats.
 */
#ifndef IP6_STATS
#define IP6_STATS                       (LWIP_IPV6)
#endif

/**
 * ICMP6_STATS==1: Enable ICMP for IPv6 stats.
 */
#ifndef ICMP6_STATS
#define ICMP6_STATS                     (LWIP_IPV6 && LWIP_ICMP6)
#endif

/**
 * IP6_FRAG_STATS==1: Enable IPv6 fragmentation stats.
 */
#ifndef IP6_FRAG_STATS
#define IP6_FRAG_STATS                  (LWIP_IPV6 && (LWIP_IPV6_FRAG || LWIP_IPV6_REASS))
#endif

/**
 * MLD6_STATS==1: Enable MLD for IPv6 stats.
 */
#ifndef MLD6_STATS
#define MLD6_STATS                      (LWIP_IPV6 && LWIP_IPV6_MLD)
#endif

/**
 * ND6_STATS==1: Enable Neighbor discovery for IPv6 stats.
 */
#ifndef ND6_STATS
#define ND6_STATS                       (LWIP_IPV6)
#endif

/**
 * MIB2_STATS==1: Stats for SNMP MIB2.
 */
#ifndef MIB2_STATS
#define MIB2_STATS                      0
#endif

#else

#define LINK_STATS                      0
#define ETHARP_STATS                    0
#define IP_STATS                        0
#define IPFRAG_STATS                    0
#define ICMP_STATS                      0
#define IGMP_STATS                      0
#define UDP_STATS                       0
#define TCP_STATS                       0
#define MEM_STATS                       0
#define MEMP_STATS                      0
#define SYS_STATS                       0
#define LWIP_STATS_DISPLAY              0
#define IP6_STATS                       0
#define ICMP6_STATS                     0
#define IP6_FRAG_STATS                  0
#define MLD6_STATS                      0
#define ND6_STATS                       0
#define MIB2_STATS                      0

#endif /* LWIP_STATS */

/*
   ---------------------------------
   ---------- PPP options ----------
   ---------------------------------
*/

/**
 * PPP_SUPPORT==1: Enable PPP.
 */
#ifndef PPP_SUPPORT
#define PPP_SUPPORT                     0
#endif

/**
 * PPPOE_SUPPORT==1: Enable PPP Over Ethernet
 */
#ifndef PPPOE_SUPPORT
#define PPPOE_SUPPORT                   0
#endif

/**
 * PPPOL2TP_SUPPORT==1: Enable PPP Over L2TP
 */
#ifndef PPPOL2TP_SUPPORT
#define PPPOL2TP_SUPPORT                0
#endif

/**
 * PPPOL2TP_AUTH_SUPPORT==1: Enable PPP Over L2TP Auth (enable MD5 support)
 */
#ifndef PPPOL2TP_AUTH_SUPPORT
#define PPPOL2TP_AUTH_SUPPORT           PPPOL2TP_SUPPORT
#endif

/**
 * PPPOS_SUPPORT==1: Enable PPP Over Serial
 */
#ifndef PPPOS_SUPPORT
#define PPPOS_SUPPORT                   PPP_SUPPORT
#endif

/**
 * LWIP_PPP_API==1: Enable PPP API (in pppapi.c)
 */
#ifndef LWIP_PPP_API
#define LWIP_PPP_API                    (PPP_SUPPORT && (NO_SYS == 0))
#endif

#if PPP_SUPPORT

/**
 * PPP_INPROC_IRQ_SAFE==1 call pppos_input() using tcpip_callback().
 *
 * Please read the "PPPoS input path" chapter in the PPP documentation about this option.
 */
#ifndef PPP_INPROC_IRQ_SAFE
#define PPP_INPROC_IRQ_SAFE             0
#endif

/**
 * PRINTPKT_SUPPORT==1: Enable PPP print packet support
 *
 * Mandatory for debugging, it displays exchanged packet content in debug trace.
 */
#ifndef PRINTPKT_SUPPORT
#define PRINTPKT_SUPPORT                0
#endif

/**
 * PPP_IPV4_SUPPORT==1: Enable PPP IPv4 support
 */
#ifndef PPP_IPV4_SUPPORT
#define PPP_IPV4_SUPPORT                (LWIP_IPV4)
#endif

/**
 * PPP_IPV6_SUPPORT==1: Enable PPP IPv6 support
 */
#ifndef PPP_IPV6_SUPPORT
#define PPP_IPV6_SUPPORT                (LWIP_IPV6)
#endif

/**
 * PPP_NOTIFY_PHASE==1: Support PPP notify phase support
 *
 * PPP notify phase support allows you to set a callback which is
 * called on change of the internal PPP state machine.
 *
 * This can be used for example to set a LED pattern depending on the
 * current phase of the PPP session.
 */
#ifndef PPP_NOTIFY_PHASE
#define PPP_NOTIFY_PHASE                0
#endif

/**
 * pbuf_type PPP is using for LCP, PAP, CHAP, EAP, CCP, IPCP and IP6CP packets.
 *
 * Memory allocated must be single buffered for PPP to works, it requires pbuf
 * that are not going to be chained when allocated. This requires setting
 * PBUF_POOL_BUFSIZE to at least 512 bytes, which is quite huge for small systems.
 *
 * Setting PPP_USE_PBUF_RAM to 1 makes PPP use memory from heap where continuous
 * buffers are required, allowing you to use a smaller PBUF_POOL_BUFSIZE.
 */
#ifndef PPP_USE_PBUF_RAM
#define PPP_USE_PBUF_RAM                0
#endif

/**
 * PPP_FCS_TABLE: Keep a 256*2 byte table to speed up FCS calculation for PPPoS
 */
#ifndef PPP_FCS_TABLE
#define PPP_FCS_TABLE                   1
#endif

/**
 * PAP_SUPPORT==1: Support PAP.
 */
#ifndef PAP_SUPPORT
#define PAP_SUPPORT                     0
#endif

/**
 * CHAP_SUPPORT==1: Support CHAP.
 */
#ifndef CHAP_SUPPORT
#define CHAP_SUPPORT                    0
#endif

/**
 * MSCHAP_SUPPORT==1: Support MSCHAP.
 */
#ifndef MSCHAP_SUPPORT
#define MSCHAP_SUPPORT                  0
#endif
#if MSCHAP_SUPPORT
/* MSCHAP requires CHAP support */
#undef CHAP_SUPPORT
#define CHAP_SUPPORT                    1
#endif /* MSCHAP_SUPPORT */

/**
 * EAP_SUPPORT==1: Support EAP.
 */
#ifndef EAP_SUPPORT
#define EAP_SUPPORT                     0
#endif

/**
 * CCP_SUPPORT==1: Support CCP.
 */
#ifndef CCP_SUPPORT
#define CCP_SUPPORT                     0
#endif

/**
 * MPPE_SUPPORT==1: Support MPPE.
 */
#ifndef MPPE_SUPPORT
#define MPPE_SUPPORT                    0
#endif
#if MPPE_SUPPORT
/* MPPE requires CCP support */
#undef CCP_SUPPORT
#define CCP_SUPPORT                     1
/* MPPE requires MSCHAP support */
#undef MSCHAP_SUPPORT
#define MSCHAP_SUPPORT                  1
/* MSCHAP requires CHAP support */
#undef CHAP_SUPPORT
#define CHAP_SUPPORT                    1
#endif /* MPPE_SUPPORT */

/**
 * CBCP_SUPPORT==1: Support CBCP. CURRENTLY NOT SUPPORTED! DO NOT SET!
 */
#ifndef CBCP_SUPPORT
#define CBCP_SUPPORT                    0
#endif

/**
 * ECP_SUPPORT==1: Support ECP. CURRENTLY NOT SUPPORTED! DO NOT SET!
 */
#ifndef ECP_SUPPORT
#define ECP_SUPPORT                     0
#endif

/**
 * DEMAND_SUPPORT==1: Support dial on demand. CURRENTLY NOT SUPPORTED! DO NOT SET!
 */
#ifndef DEMAND_SUPPORT
#define DEMAND_SUPPORT                  0
#endif

/**
 * LQR_SUPPORT==1: Support Link Quality Report. Do nothing except exchanging some LCP packets.
 */
#ifndef LQR_SUPPORT
#define LQR_SUPPORT                     0
#endif

/**
 * PPP_SERVER==1: Enable PPP server support (waiting for incoming PPP session).
 *
 * Currently only supported for PPPoS.
 */
#ifndef PPP_SERVER
#define PPP_SERVER                      0
#endif

#if PPP_SERVER
/*
 * PPP_OUR_NAME: Our name for authentication purposes
 */
#ifndef PPP_OUR_NAME
#define PPP_OUR_NAME                    "lwIP"
#endif
#endif /* PPP_SERVER */

/**
 * VJ_SUPPORT==1: Support VJ header compression.
 */
#ifndef VJ_SUPPORT
#define VJ_SUPPORT                      1
#endif
/* VJ compression is only supported for IPv4 over PPPoS. */
#if !PPPOS_SUPPORT || !PPP_IPV4_SUPPORT
#undef VJ_SUPPORT
#define VJ_SUPPORT                      0
#endif /* !PPPOS_SUPPORT */

/**
 * PPP_MD5_RANDM==1: Use MD5 for better randomness.
 * Enabled by default if CHAP, EAP, or L2TP AUTH support is enabled.
 */
#ifndef PPP_MD5_RANDM
#define PPP_MD5_RANDM                   (CHAP_SUPPORT || EAP_SUPPORT || PPPOL2TP_AUTH_SUPPORT)
#endif

/**
 * PolarSSL library, used if necessary and not previously disabled
 *
 *
 * lwIP contains some files fetched from the latest BSD release of
 * the PolarSSL project for ciphers and encryption methods we need for lwIP
 * PPP support.
 *
 * The PolarSSL files were cleaned to contain only the necessary struct
 * fields and functions needed for lwIP.
 *
 * The PolarSSL API was not changed at all, so if you are already using
 * PolarSSL you can choose to skip the compilation of the included PolarSSL
 * library into lwIP:
 *
 * The following defines are available for flexibility:
 *
 * LWIP_INCLUDED_POLARSSL_MD4   ; Use lwIP internal PolarSSL for MD4
 * LWIP_INCLUDED_POLARSSL_MD5   ; Use lwIP internal PolarSSL for MD5
 * LWIP_INCLUDED_POLARSSL_SHA1  ; Use lwIP internal PolarSSL for SHA1
 * LWIP_INCLUDED_POLARSSL_DES   ; Use lwIP internal PolarSSL for DES
 *
 * If set (=1), the default if required by another enabled PPP feature unless
 * explicitly set to 0, using included lwIP PolarSSL.
 *
 * If clear (=0), not needed or using external PolarSSL.
 *
 * Beware of the stack requirements which can be a lot larger if you are not
 * using our cleaned PolarSSL library.
 */

/* CHAP, EAP, L2TP AUTH and MD5 Random require MD5 support */
#if CHAP_SUPPORT || EAP_SUPPORT || PPPOL2TP_AUTH_SUPPORT || PPP_MD5_RANDM
#ifndef LWIP_INCLUDED_POLARSSL_MD5
#define LWIP_INCLUDED_POLARSSL_MD5        1
#endif /* LWIP_INCLUDED_POLARSSL_MD5 */
#endif /* CHAP_SUPPORT || EAP_SUPPORT || PPPOL2TP_AUTH_SUPPORT || PPP_MD5_RANDM */

#if MSCHAP_SUPPORT
/* MSCHAP require MD4 support */
#ifndef LWIP_INCLUDED_POLARSSL_MD4
#define LWIP_INCLUDED_POLARSSL_MD4      1
#endif /* LWIP_INCLUDED_POLARSSL_MD4 */
/* MSCHAP require SHA1 support */
#ifndef LWIP_INCLUDED_POLARSSL_SHA1
#define LWIP_INCLUDED_POLARSSL_SHA1     1
#endif /* LWIP_INCLUDED_POLARSSL_SHA1 */
/* MSCHAP require DES support */
#ifndef LWIP_INCLUDED_POLARSSL_DES
#define LWIP_INCLUDED_POLARSSL_DES      1
#endif /* LWIP_INCLUDED_POLARSSL_DES */
/* MS-CHAP support is required for MPPE */
#if MPPE_SUPPORT
/* MPPE require ARC4 support */
#ifndef LWIP_INCLUDED_POLARSSL_ARC4
#define LWIP_INCLUDED_POLARSSL_ARC4     1
#endif /* LWIP_INCLUDED_POLARSSL_ARC4*/
#endif /* MPPE_SUPPORT */
#endif /* MSCHAP_SUPPORT */

/* Default value if unset */
#ifndef LWIP_INCLUDED_POLARSSL_MD4
#define LWIP_INCLUDED_POLARSSL_MD4      0
#endif /* LWIP_INCLUDED_POLARSSL_MD4 */
#ifndef LWIP_INCLUDED_POLARSSL_MD5
#define LWIP_INCLUDED_POLARSSL_MD5      0
#endif /* LWIP_INCLUDED_POLARSSL_MD5 */
#ifndef LWIP_INCLUDED_POLARSSL_SHA1
#define LWIP_INCLUDED_POLARSSL_SHA1     0
#endif /* LWIP_INCLUDED_POLARSSL_SHA1 */
#ifndef LWIP_INCLUDED_POLARSSL_DES
#define LWIP_INCLUDED_POLARSSL_DES      0
#endif /* LWIP_INCLUDED_POLARSSL_DES */
#ifndef LWIP_INCLUDED_POLARSSL_ARC4
#define LWIP_INCLUDED_POLARSSL_ARC4     0
#endif /* LWIP_INCLUDED_POLARSSL_ARC4 */

/*
 * PPP Timeouts
 */

/**
 * FSM_DEFTIMEOUT: Timeout time in seconds
 */
#ifndef FSM_DEFTIMEOUT
#define FSM_DEFTIMEOUT                  6
#endif

/**
 * FSM_DEFMAXTERMREQS: Maximum Terminate-Request transmissions
 */
#ifndef FSM_DEFMAXTERMREQS
#define FSM_DEFMAXTERMREQS              2
#endif

/**
 * FSM_DEFMAXCONFREQS: Maximum Configure-Request transmissions
 */
#ifndef FSM_DEFMAXCONFREQS
#define FSM_DEFMAXCONFREQS              10
#endif

/**
 * FSM_DEFMAXNAKLOOPS: Maximum number of nak loops
 */
#ifndef FSM_DEFMAXNAKLOOPS
#define FSM_DEFMAXNAKLOOPS              5
#endif

/**
 * UPAP_DEFTIMEOUT: Timeout (seconds) for retransmitting req
 */
#ifndef UPAP_DEFTIMEOUT
#define UPAP_DEFTIMEOUT                 6
#endif

/**
 * UPAP_DEFTRANSMITS: Maximum number of auth-reqs to send
 */
#ifndef UPAP_DEFTRANSMITS
#define UPAP_DEFTRANSMITS               10
#endif

#if PPP_SERVER
/**
 * UPAP_DEFREQTIME: Time to wait for auth-req from peer
 */
#ifndef UPAP_DEFREQTIME
#define UPAP_DEFREQTIME                 30
#endif
#endif /* PPP_SERVER */

/**
 * CHAP_DEFTIMEOUT: Timeout (seconds) for retransmitting req
 */
#ifndef CHAP_DEFTIMEOUT
#define CHAP_DEFTIMEOUT                 6
#endif

/**
 * CHAP_DEFTRANSMITS: max # times to send challenge
 */
#ifndef CHAP_DEFTRANSMITS
#define CHAP_DEFTRANSMITS               10
#endif

#if PPP_SERVER
/**
 * CHAP_DEFRECHALLENGETIME: If this option is > 0, rechallenge the peer every n seconds
 */
#ifndef CHAP_DEFRECHALLENGETIME
#define CHAP_DEFRECHALLENGETIME         0
#endif
#endif /* PPP_SERVER */

/**
 * EAP_DEFREQTIME: Time to wait for peer request
 */
#ifndef EAP_DEFREQTIME
#define EAP_DEFREQTIME                  6
#endif

/**
 * EAP_DEFALLOWREQ: max # times to accept requests
 */
#ifndef EAP_DEFALLOWREQ
#define EAP_DEFALLOWREQ                 10
#endif

#if PPP_SERVER
/**
 * EAP_DEFTIMEOUT: Timeout (seconds) for rexmit
 */
#ifndef EAP_DEFTIMEOUT
#define EAP_DEFTIMEOUT                  6
#endif

/**
 * EAP_DEFTRANSMITS: max # times to transmit
 */
#ifndef EAP_DEFTRANSMITS
#define EAP_DEFTRANSMITS                10
#endif
#endif /* PPP_SERVER */

/**
 * LCP_DEFLOOPBACKFAIL: Default number of times we receive our magic number from the peer
 * before deciding the link is looped-back.
 */
#ifndef LCP_DEFLOOPBACKFAIL
#define LCP_DEFLOOPBACKFAIL             10
#endif

/**
 * LCP_ECHOINTERVAL: Interval in seconds between keepalive echo requests, 0 to disable.
 */
#ifndef LCP_ECHOINTERVAL
#define LCP_ECHOINTERVAL                0
#endif

/**
 * LCP_MAXECHOFAILS: Number of unanswered echo requests before failure.
 */
#ifndef LCP_MAXECHOFAILS
#define LCP_MAXECHOFAILS                3
#endif

/**
 * PPP_MAXIDLEFLAG: Max Xmit idle time (in ms) before resend flag char.
 */
#ifndef PPP_MAXIDLEFLAG
#define PPP_MAXIDLEFLAG                 100
#endif

/**
 * PPP Packet sizes
 */

/**
 * PPP_MRU: Default MRU
 */
#ifndef PPP_MRU
#define PPP_MRU                         1500
#endif

/**
 * PPP_DEFMRU: Default MRU to try
 */
#ifndef PPP_DEFMRU
#define PPP_DEFMRU                      1500
#endif

/**
 * PPP_MAXMRU: Normally limit MRU to this (pppd default = 16384)
 */
#ifndef PPP_MAXMRU
#define PPP_MAXMRU                      1500
#endif

/**
 * PPP_MINMRU: No MRUs below this
 */
#ifndef PPP_MINMRU
#define PPP_MINMRU                      128
#endif

/**
 * PPPOL2TP_DEFMRU: Default MTU and MRU for L2TP
 * Default = 1500 - PPPoE(6) - PPP Protocol(2) - IPv4 header(20) - UDP Header(8)
 * - L2TP Header(6) - HDLC Header(2) - PPP Protocol(2) - MPPE Header(2) - PPP Protocol(2)
 */
#if PPPOL2TP_SUPPORT
#ifndef PPPOL2TP_DEFMRU
#define PPPOL2TP_DEFMRU                 1450
#endif
#endif /* PPPOL2TP_SUPPORT */

/**
 * MAXNAMELEN: max length of hostname or name for auth
 */
#ifndef MAXNAMELEN
#define MAXNAMELEN                      256
#endif

/**
 * MAXSECRETLEN: max length of password or secret
 */
#ifndef MAXSECRETLEN
#define MAXSECRETLEN                    256
#endif

#endif /* PPP_SUPPORT */

/*
   --------------------------------------
   ---------- Checksum options ----------
   --------------------------------------
*/

/**
 * LWIP_CHECKSUM_CTRL_PER_NETIF==1: Checksum generation/check can be enabled/disabled
 * per netif.
 * ATTENTION: if enabled, the CHECKSUM_GEN_* and CHECKSUM_CHECK_* defines must be enabled!
 */
#ifndef LWIP_CHECKSUM_CTRL_PER_NETIF
#define LWIP_CHECKSUM_CTRL_PER_NETIF    0
#endif

/**
 * CHECKSUM_GEN_IP==1: Generate checksums in software for outgoing IP packets.
 */
#ifndef CHECKSUM_GEN_IP
#define CHECKSUM_GEN_IP                 1
#endif

/**
 * CHECKSUM_GEN_UDP==1: Generate checksums in software for outgoing UDP packets.
 */
#ifndef CHECKSUM_GEN_UDP
#define CHECKSUM_GEN_UDP                1
#endif

/**
 * CHECKSUM_GEN_TCP==1: Generate checksums in software for outgoing TCP packets.
 */
#ifndef CHECKSUM_GEN_TCP
#define CHECKSUM_GEN_TCP                1
#endif

/**
 * CHECKSUM_GEN_ICMP==1: Generate checksums in software for outgoing ICMP packets.
 */
#ifndef CHECKSUM_GEN_ICMP
#define CHECKSUM_GEN_ICMP               1
#endif

/**
 * CHECKSUM_GEN_ICMP6==1: Generate checksums in software for outgoing ICMP6 packets.
 */
#ifndef CHECKSUM_GEN_ICMP6
#define CHECKSUM_GEN_ICMP6              1
#endif

/**
 * CHECKSUM_CHECK_IP==1: Check checksums in software for incoming IP packets.
 */
#ifndef CHECKSUM_CHECK_IP
#define CHECKSUM_CHECK_IP               1
#endif

/**
 * CHECKSUM_CHECK_UDP==1: Check checksums in software for incoming UDP packets.
 */
#ifndef CHECKSUM_CHECK_UDP
#define CHECKSUM_CHECK_UDP              1
#endif

/**
 * CHECKSUM_CHECK_TCP==1: Check checksums in software for incoming TCP packets.
 */
#ifndef CHECKSUM_CHECK_TCP
#define CHECKSUM_CHECK_TCP              1
#endif

/**
 * CHECKSUM_CHECK_ICMP==1: Check checksums in software for incoming ICMP packets.
 */
#ifndef CHECKSUM_CHECK_ICMP
#define CHECKSUM_CHECK_ICMP             1
#endif

/**
 * CHECKSUM_CHECK_ICMP6==1: Check checksums in software for incoming ICMPv6 packets
 */
#ifndef CHECKSUM_CHECK_ICMP6
#define CHECKSUM_CHECK_ICMP6            1
#endif

/**
 * LWIP_CHECKSUM_ON_COPY==1: Calculate checksum when copying data from
 * application buffers to pbufs.
 */
#ifndef LWIP_CHECKSUM_ON_COPY
#define LWIP_CHECKSUM_ON_COPY           0
#endif

/*
   ---------------------------------------
   ---------- IPv6 options ---------------
   ---------------------------------------
*/
/**
 * LWIP_IPV6==1: Enable IPv6
 */
#ifndef LWIP_IPV6
#define LWIP_IPV6                       0
#endif

/**
 * LWIP_IPV6_NUM_ADDRESSES: Number of IPv6 addresses per netif.
 */
#ifndef LWIP_IPV6_NUM_ADDRESSES
#define LWIP_IPV6_NUM_ADDRESSES         3
#endif

/**
 * LWIP_IPV6_FORWARD==1: Forward IPv6 packets across netifs
 */
#ifndef LWIP_IPV6_FORWARD
#define LWIP_IPV6_FORWARD               0
#endif

/**
 * LWIP_ICMP6==1: Enable ICMPv6 (mandatory per RFC)
 */
#ifndef LWIP_ICMP6
#define LWIP_ICMP6                      (LWIP_IPV6)
#endif

/**
 * LWIP_ICMP6_DATASIZE: bytes from original packet to send back in
 * ICMPv6 error messages.
 */
#ifndef LWIP_ICMP6_DATASIZE
#define LWIP_ICMP6_DATASIZE             8
#endif

/**
 * LWIP_ICMP6_HL: default hop limit for ICMPv6 messages
 */
#ifndef LWIP_ICMP6_HL
#define LWIP_ICMP6_HL                   255
#endif

/**
 * LWIP_IPV6_MLD==1: Enable multicast listener discovery protocol.
 */
#ifndef LWIP_IPV6_MLD
#define LWIP_IPV6_MLD                   (LWIP_IPV6)
#endif

/**
 * MEMP_NUM_MLD6_GROUP: Max number of IPv6 multicast that can be joined.
 */
#ifndef MEMP_NUM_MLD6_GROUP
#define MEMP_NUM_MLD6_GROUP             4
#endif

/**
 * LWIP_IPV6_FRAG==1: Fragment outgoing IPv6 packets that are too big.
 */
#ifndef LWIP_IPV6_FRAG
#define LWIP_IPV6_FRAG                  0
#endif

/**
 * LWIP_IPV6_REASS==1: reassemble incoming IPv6 packets that fragmented
 */
#ifndef LWIP_IPV6_REASS
#define LWIP_IPV6_REASS                 (LWIP_IPV6)
#endif

/**
 * LWIP_ND6_QUEUEING==1: queue outgoing IPv6 packets while MAC address
 * is being resolved.
 */
#ifndef LWIP_ND6_QUEUEING
#define LWIP_ND6_QUEUEING               (LWIP_IPV6)
#endif

/**
 * MEMP_NUM_ND6_QUEUE: Max number of IPv6 packets to queue during MAC resolution.
 */
#ifndef MEMP_NUM_ND6_QUEUE
#define MEMP_NUM_ND6_QUEUE              20
#endif

/**
 * LWIP_ND6_NUM_NEIGHBORS: Number of entries in IPv6 neighbor cache
 */
#ifndef LWIP_ND6_NUM_NEIGHBORS
#define LWIP_ND6_NUM_NEIGHBORS          10
#endif

/**
 * LWIP_ND6_NUM_DESTINATIONS: number of entries in IPv6 destination cache
 */
#ifndef LWIP_ND6_NUM_DESTINATIONS
#define LWIP_ND6_NUM_DESTINATIONS       10
#endif

/**
 * LWIP_ND6_NUM_PREFIXES: number of entries in IPv6 on-link prefixes cache
 */
#ifndef LWIP_ND6_NUM_PREFIXES
#define LWIP_ND6_NUM_PREFIXES           5
#endif

/**
 * LWIP_ND6_NUM_ROUTERS: number of entries in IPv6 default router cache
 */
#ifndef LWIP_ND6_NUM_ROUTERS
#define LWIP_ND6_NUM_ROUTERS            3
#endif

/**
 * LWIP_ND6_MAX_MULTICAST_SOLICIT: max number of multicast solicit messages to send
 * (neighbor solicit and router solicit)
 */
#ifndef LWIP_ND6_MAX_MULTICAST_SOLICIT
#define LWIP_ND6_MAX_MULTICAST_SOLICIT  3
#endif

/**
 * LWIP_ND6_MAX_UNICAST_SOLICIT: max number of unicast neighbor solicitation messages
 * to send during neighbor reachability detection.
 */
#ifndef LWIP_ND6_MAX_UNICAST_SOLICIT
#define LWIP_ND6_MAX_UNICAST_SOLICIT    3
#endif

/**
 * Unused: See ND RFC (time in milliseconds).
 */
#ifndef LWIP_ND6_MAX_ANYCAST_DELAY_TIME
#define LWIP_ND6_MAX_ANYCAST_DELAY_TIME 1000
#endif

/**
 * Unused: See ND RFC
 */
#ifndef LWIP_ND6_MAX_NEIGHBOR_ADVERTISEMENT
#define LWIP_ND6_MAX_NEIGHBOR_ADVERTISEMENT  3
#endif

/**
 * LWIP_ND6_REACHABLE_TIME: default neighbor reachable time (in milliseconds).
 * May be updated by router advertisement messages.
 */
#ifndef LWIP_ND6_REACHABLE_TIME
#define LWIP_ND6_REACHABLE_TIME         30000
#endif

/**
 * LWIP_ND6_RETRANS_TIMER: default retransmission timer for solicitation messages
 */
#ifndef LWIP_ND6_RETRANS_TIMER
#define LWIP_ND6_RETRANS_TIMER          1000
#endif

/**
 * LWIP_ND6_DELAY_FIRST_PROBE_TIME: Delay before first unicast neighbor solicitation
 * message is sent, during neighbor reachability detection.
 */
#ifndef LWIP_ND6_DELAY_FIRST_PROBE_TIME
#define LWIP_ND6_DELAY_FIRST_PROBE_TIME 5000
#endif

/**
 * LWIP_ND6_ALLOW_RA_UPDATES==1: Allow Router Advertisement messages to update
 * Reachable time and retransmission timers, and netif MTU.
 */
#ifndef LWIP_ND6_ALLOW_RA_UPDATES
#define LWIP_ND6_ALLOW_RA_UPDATES       1
#endif

/**
 * LWIP_IPV6_SEND_ROUTER_SOLICIT==1: Send router solicitation messages during
 * network startup.
 */
#ifndef LWIP_IPV6_SEND_ROUTER_SOLICIT
#define LWIP_IPV6_SEND_ROUTER_SOLICIT   1
#endif

/**
 * LWIP_ND6_TCP_REACHABILITY_HINTS==1: Allow TCP to provide Neighbor Discovery
 * with reachability hints for connected destinations. This helps avoid sending
 * unicast neighbor solicitation messages.
 */
#ifndef LWIP_ND6_TCP_REACHABILITY_HINTS
#define LWIP_ND6_TCP_REACHABILITY_HINTS 1
#endif

/**
 * LWIP_IPV6_AUTOCONFIG==1: Enable stateless address autoconfiguration as per RFC 4862.
 */
#ifndef LWIP_IPV6_AUTOCONFIG
#define LWIP_IPV6_AUTOCONFIG            (LWIP_IPV6)
#endif

/**
 * LWIP_IPV6_DUP_DETECT_ATTEMPTS: Number of duplicate address detection attempts.
 */
#ifndef LWIP_IPV6_DUP_DETECT_ATTEMPTS
#define LWIP_IPV6_DUP_DETECT_ATTEMPTS   1
#endif

/**
 * LWIP_IPV6_DHCP6==1: enable DHCPv6 stateful address autoconfiguration.
 */
#ifndef LWIP_IPV6_DHCP6
#define LWIP_IPV6_DHCP6                 0
#endif

/*
   ---------------------------------------
   ---------- Hook options ---------------
   ---------------------------------------
*/

/* Hooks are undefined by default, define them to a function if you need them. */

/**
 * LWIP_HOOK_IP4_INPUT(pbuf, input_netif):
 * - called from ip_input() (IPv4)
 * - pbuf: received struct pbuf passed to ip_input()
 * - input_netif: struct netif on which the packet has been received
 * Return values:
 * - 0: Hook has not consumed the packet, packet is processed as normal
 * - != 0: Hook has consumed the packet.
 * If the hook consumed the packet, 'pbuf' is in the responsibility of the hook
 * (i.e. free it when done).
 */

/**
 * LWIP_HOOK_IP4_ROUTE(dest):
 * - called from ip_route() (IPv4)
 * - dest: destination IPv4 address
 * Returns the destination netif or NULL if no destination netif is found. In
 * that case, ip_route() continues as normal.
 */

/**
 * LWIP_HOOK_IP4_ROUTE_SRC(dest, src):
 * - source-based routing for IPv4 (see LWIP_HOOK_IP4_ROUTE(), src may be NULL)
 */

/**
 * LWIP_HOOK_ETHARP_GET_GW(netif, dest):
 * - called from etharp_output() (IPv4)
 * - netif: the netif used for sending
 * - dest: the destination IPv4 address
 * Returns the IPv4 address of the gateway to handle the specified destination
 * IPv4 address. If NULL is returned, the netif's default gateway is used.
 * The returned address MUST be reachable on the specified netif!
 * This function is meant to implement advanced IPv4 routing together with
 * LWIP_HOOK_IP4_ROUTE(). The actual routing/gateway table implementation is
 * not part of lwIP but can e.g. be hidden in the netif's state argument.
*/

/**
 * LWIP_HOOK_IP6_INPUT(pbuf, input_netif):
 * - called from ip6_input() (IPv6)
 * - pbuf: received struct pbuf passed to ip6_input()
 * - input_netif: struct netif on which the packet has been received
 * Return values:
 * - 0: Hook has not consumed the packet, packet is processed as normal
 * - != 0: Hook has consumed the packet.
 * If the hook consumed the packet, 'pbuf' is in the responsibility of the hook
 * (i.e. free it when done).
 */

/**
 * LWIP_HOOK_IP6_ROUTE(src, dest):
 * - called from ip6_route() (IPv6)
 * - src: sourc IPv6 address
 * - dest: destination IPv6 address
 * Returns the destination netif or NULL if no destination netif is found. In
 * that case, ip6_route() continues as normal.
 */

/**
 * LWIP_HOOK_VLAN_CHECK(netif, eth_hdr, vlan_hdr):
 * - called from ethernet_input() if VLAN support is enabled
 * - netif: struct netif on which the packet has been received
 * - eth_hdr: struct eth_hdr of the packet
 * - vlan_hdr: struct eth_vlan_hdr of the packet
 * Return values:
 * - 0: Packet must be dropped.
 * - != 0: Packet must be accepted.
 */

/**
 * LWIP_HOOK_VLAN_SET(netif, eth_hdr, vlan_hdr):
 * - called from etharp_raw() and etharp_send_ip() if VLAN support is enabled
 * - netif: struct netif that the packet will be sent through
 * - eth_hdr: struct eth_hdr of the packet
 * - vlan_hdr: struct eth_vlan_hdr of the packet
 * Return values:
 * - 0: Packet shall not contain VLAN header.
 * - != 0: Packet shall contain VLAN header.
 * Hook can be used to set prio_vid field of vlan_hdr.
 */

/**
 * LWIP_HOOK_MEMP_AVAILABLE(memp_t_type):
 * - called from memp_free() when a memp pool was empty and an item is now available
 */

/*
   ---------------------------------------
   ---------- Debugging options ----------
   ---------------------------------------
*/
/**
 * LWIP_DBG_MIN_LEVEL: After masking, the value of the debug is
 * compared against this value. If it is smaller, then debugging
 * messages are written.
 */
#ifndef LWIP_DBG_MIN_LEVEL
#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_ALL
#endif

/**
 * LWIP_DBG_TYPES_ON: A mask that can be used to globally enable/disable
 * debug messages of certain types.
 */
#ifndef LWIP_DBG_TYPES_ON
#define LWIP_DBG_TYPES_ON               LWIP_DBG_ON
#endif

/**
 * ETHARP_DEBUG: Enable debugging in etharp.c.
 */
#ifndef ETHARP_DEBUG
#define ETHARP_DEBUG                    LWIP_DBG_OFF
#endif

/**
 * NETIF_DEBUG: Enable debugging in netif.c.
 */
#ifndef NETIF_DEBUG
#define NETIF_DEBUG                     LWIP_DBG_OFF
#endif

/**
 * PBUF_DEBUG: Enable debugging in pbuf.c.
 */
#ifndef PBUF_DEBUG
#define PBUF_DEBUG                      LWIP_DBG_OFF
#endif

/**
 * API_LIB_DEBUG: Enable debugging in api_lib.c.
 */
#ifndef API_LIB_DEBUG
#define API_LIB_DEBUG                   LWIP_DBG_OFF
#endif

/**
 * API_MSG_DEBUG: Enable debugging in api_msg.c.
 */
#ifndef API_MSG_DEBUG
#define API_MSG_DEBUG                   LWIP_DBG_OFF
#endif

/**
 * SOCKETS_DEBUG: Enable debugging in sockets.c.
 */
#ifndef SOCKETS_DEBUG
#define SOCKETS_DEBUG                   LWIP_DBG_OFF
#endif

/**
 * ICMP_DEBUG: Enable debugging in icmp.c.
 */
#ifndef ICMP_DEBUG
#define ICMP_DEBUG                      LWIP_DBG_OFF
#endif

/**
 * IGMP_DEBUG: Enable debugging in igmp.c.
 */
#ifndef IGMP_DEBUG
#define IGMP_DEBUG                      LWIP_DBG_OFF
#endif

/**
 * INET_DEBUG: Enable debugging in inet.c.
 */
#ifndef INET_DEBUG
#define INET_DEBUG                      LWIP_DBG_OFF
#endif

/**
 * IP_DEBUG: Enable debugging for IP.
 */
#ifndef IP_DEBUG
#define IP_DEBUG                        LWIP_DBG_OFF
#endif

/**
 * IP_REASS_DEBUG: Enable debugging in ip_frag.c for both frag & reass.
 */
#ifndef IP_REASS_DEBUG
#define IP_REASS_DEBUG                  LWIP_DBG_OFF
#endif

/**
 * RAW_DEBUG: Enable debugging in raw.c.
 */
#ifndef RAW_DEBUG
#define RAW_DEBUG                       LWIP_DBG_OFF
#endif

/**
 * MEM_DEBUG: Enable debugging in mem.c.
 */
#ifndef MEM_DEBUG
#define MEM_DEBUG                       LWIP_DBG_OFF
#endif

/**
 * MEMP_DEBUG: Enable debugging in memp.c.
 */
#ifndef MEMP_DEBUG
#define MEMP_DEBUG                      LWIP_DBG_OFF
#endif

/**
 * SYS_DEBUG: Enable debugging in sys.c.
 */
#ifndef SYS_DEBUG
#define SYS_DEBUG                       LWIP_DBG_OFF
#endif

/**
 * TIMERS_DEBUG: Enable debugging in timers.c.
 */
#ifndef TIMERS_DEBUG
#define TIMERS_DEBUG                    LWIP_DBG_OFF
#endif

/**
 * TCP_DEBUG: Enable debugging for TCP.
 */
#ifndef TCP_DEBUG
#define TCP_DEBUG                       LWIP_DBG_OFF
#endif

/**
 * TCP_INPUT_DEBUG: Enable debugging in tcp_in.c for incoming debug.
 */
#ifndef TCP_INPUT_DEBUG
#define TCP_INPUT_DEBUG                 LWIP_DBG_OFF
#endif

/**
 * TCP_FR_DEBUG: Enable debugging in tcp_in.c for fast retransmit.
 */
#ifndef TCP_FR_DEBUG
#define TCP_FR_DEBUG                    LWIP_DBG_OFF
#endif

/**
 * TCP_RTO_DEBUG: Enable debugging in TCP for retransmit
 * timeout.
 */
#ifndef TCP_RTO_DEBUG
#define TCP_RTO_DEBUG                   LWIP_DBG_OFF
#endif

/**
 * TCP_CWND_DEBUG: Enable debugging for TCP congestion window.
 */
#ifndef TCP_CWND_DEBUG
#define TCP_CWND_DEBUG                  LWIP_DBG_OFF
#endif

/**
 * TCP_WND_DEBUG: Enable debugging in tcp_in.c for window updating.
 */
#ifndef TCP_WND_DEBUG
#define TCP_WND_DEBUG                   LWIP_DBG_OFF
#endif

/**
 * TCP_OUTPUT_DEBUG: Enable debugging in tcp_out.c output functions.
 */
#ifndef TCP_OUTPUT_DEBUG
#define TCP_OUTPUT_DEBUG                LWIP_DBG_OFF
#endif

/**
 * TCP_RST_DEBUG: Enable debugging for TCP with the RST message.
 */
#ifndef TCP_RST_DEBUG
#define TCP_RST_DEBUG                   LWIP_DBG_OFF
#endif

/**
 * TCP_QLEN_DEBUG: Enable debugging for TCP queue lengths.
 */
#ifndef TCP_QLEN_DEBUG
#define TCP_QLEN_DEBUG                  LWIP_DBG_OFF
#endif

/**
 * UDP_DEBUG: Enable debugging in UDP.
 */
#ifndef UDP_DEBUG
#define UDP_DEBUG                       LWIP_DBG_OFF
#endif

/**
 * TCPIP_DEBUG: Enable debugging in tcpip.c.
 */
#ifndef TCPIP_DEBUG
#define TCPIP_DEBUG                     LWIP_DBG_OFF
#endif

/**
 * PPP_DEBUG: Enable debugging for PPP.
 */
#ifndef PPP_DEBUG
#define PPP_DEBUG                       LWIP_DBG_OFF
#endif

/**
 * SLIP_DEBUG: Enable debugging in slipif.c.
 */
#ifndef SLIP_DEBUG
#define SLIP_DEBUG                      LWIP_DBG_OFF
#endif

/**
 * DHCP_DEBUG: Enable debugging in dhcp.c.
 */
#ifndef DHCP_DEBUG
#define DHCP_DEBUG                      LWIP_DBG_OFF
#endif

/**
 * AUTOIP_DEBUG: Enable debugging in autoip.c.
 */
#ifndef AUTOIP_DEBUG
#define AUTOIP_DEBUG                    LWIP_DBG_OFF
#endif

/**
 * DNS_DEBUG: Enable debugging for DNS.
 */
#ifndef DNS_DEBUG
#define DNS_DEBUG                       LWIP_DBG_OFF
#endif

/**
 * IP6_DEBUG: Enable debugging for IPv6.
 */
#ifndef IP6_DEBUG
#define IP6_DEBUG                       LWIP_DBG_OFF
#endif

/*
   --------------------------------------------------
   ---------- Performance tracking options ----------
   --------------------------------------------------
*/
/**
 * LWIP_PERF: Enable performance testing for lwIP
 * (if enabled, arch/perf.h is included)
 */
#ifndef LWIP_PERF
#define LWIP_PERF                       0
#endif

/**
 * ESP_L2_TO_L3_COPY: enable memcpy when receiving packet from L2
 */
#ifndef ESP_L2_TO_L3_COPY
#define ESP_L2_TO_L3_COPY                   0
#endif

#ifndef ESP_THREAD_SAFE_DEBUG
#define ESP_THREAD_SAFE_DEBUG               0
#endif

#endif /* LWIP_HDR_OPT_H */
