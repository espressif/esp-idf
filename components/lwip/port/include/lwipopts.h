/*
 * SPDX-FileCopyrightText: 2001-2003 Swedish Institute of Computer Science
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * SPDX-FileContributor: 2015-2024 Espressif Systems (Shanghai) CO LTD
 */
#ifndef LWIP_HDR_ESP_LWIPOPTS_H
#define LWIP_HDR_ESP_LWIPOPTS_H

#include "sdkconfig.h"
#include <string.h>     // For memcpy
#include <stdlib.h>     // For malloc/free
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/poll.h>
#include "esp_task.h"
#include "esp_random.h"
#include "sdkconfig.h"
#include "sntp/sntp_get_set_time.h"
#include "sockets_ext.h"
#include "arch/sys_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
   -----------------------------------------------
   ---------- Platform specific locking ----------
   -----------------------------------------------
*/
/**
 * LWIP_TCPIP_CORE_LOCKING
 * Creates a global mutex that is held during TCPIP thread operations.
 * Can be locked by client code to perform lwIP operations without changing
 * into TCPIP thread using callbacks. See LOCK_TCPIP_CORE() and
 * UNLOCK_TCPIP_CORE().
 * Your system should provide mutexes supporting priority inversion to use this.
 */
#ifdef CONFIG_LWIP_TCPIP_CORE_LOCKING
#define LWIP_TCPIP_CORE_LOCKING         1
#ifdef CONFIG_LWIP_TCPIP_CORE_LOCKING_INPUT
#define LWIP_TCPIP_CORE_LOCKING_INPUT   1
#else
#define LWIP_TCPIP_CORE_LOCKING_INPUT   0
#endif
#define LOCK_TCPIP_CORE()     do { sys_mutex_lock(&lock_tcpip_core); sys_thread_tcpip(LWIP_CORE_LOCK_MARK_HOLDER); } while(0)
#define UNLOCK_TCPIP_CORE()   do { sys_thread_tcpip(LWIP_CORE_LOCK_UNMARK_HOLDER); sys_mutex_unlock(&lock_tcpip_core);  } while(0)
#ifdef CONFIG_LWIP_CHECK_THREAD_SAFETY
#define LWIP_ASSERT_CORE_LOCKED() do { LWIP_ASSERT("Required to lock TCPIP core functionality!", sys_thread_tcpip(LWIP_CORE_LOCK_QUERY_HOLDER)); } while(0)
#endif /* CONFIG_LWIP_CHECK_THREAD_SAFETY */

#else
#define LWIP_TCPIP_CORE_LOCKING         0
#define LWIP_TCPIP_CORE_LOCKING_INPUT   0
#ifdef CONFIG_LWIP_CHECK_THREAD_SAFETY
#define LWIP_ASSERT_CORE_LOCKED()     do { LWIP_ASSERT("Required to run in TCPIP context!", sys_thread_tcpip(LWIP_CORE_LOCK_QUERY_HOLDER)); } while(0)
#endif /* CONFIG_LWIP_CHECK_THREAD_SAFETY */
#endif /* CONFIG_LWIP_TCPIP_CORE_LOCKING */

#define LWIP_MARK_TCPIP_THREAD() sys_thread_tcpip(LWIP_CORE_MARK_TCPIP_TASK)

/**
 * SYS_LIGHTWEIGHT_PROT==1: if you want inter-task protection for certain
 * critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#define SYS_LIGHTWEIGHT_PROT        1

/**
 * MEMCPY: override this if you have a faster implementation at hand than the
 * one included in your C library
 */
#define MEMCPY(dst,src,len)             memcpy(dst,src,len)

/**
 * SMEMCPY: override this with care! Some compilers (e.g. gcc) can inline a
 * call to memcpy() if the length is known at compile time and is small.
 */
#define SMEMCPY(dst,src,len)            memcpy(dst,src,len)

#define LWIP_RAND       esp_random

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
#define MEM_LIBC_MALLOC                 1

/**
* MEMP_MEM_MALLOC==1: Use mem_malloc/mem_free instead of the lwip pool allocator.
* Especially useful with MEM_LIBC_MALLOC but handle with care regarding execution
* speed and usage from interrupts!
*/
#define MEMP_MEM_MALLOC                 1

/**
 * MEM_ALIGNMENT: should be set to the alignment of the CPU
 *    4 byte alignment -> #define MEM_ALIGNMENT 4
 *    2 byte alignment -> #define MEM_ALIGNMENT 2
 */
#define MEM_ALIGNMENT                   4

/*
   ------------------------------------------------
   ---------- Internal Memory Pool Sizes ----------
   ------------------------------------------------
*/

/**
 * MEMP_NUM_NETCONN: the number of struct netconns.
 * (only needed if you use the sequential API, like api_lib.c)
 */
#define MEMP_NUM_NETCONN                CONFIG_LWIP_MAX_SOCKETS

/**
 * MEMP_NUM_RAW_PCB: Number of raw connection PCBs
 * (requires the LWIP_RAW option)
 */
#define MEMP_NUM_RAW_PCB                CONFIG_LWIP_MAX_RAW_PCBS

/**
 * MEMP_NUM_TCP_PCB: the number of simultaneously active TCP connections.
 * (requires the LWIP_TCP option)
 */
#define MEMP_NUM_TCP_PCB                CONFIG_LWIP_MAX_ACTIVE_TCP

/**
 * MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP connections.
 * (requires the LWIP_TCP option)
 */
#define MEMP_NUM_TCP_PCB_LISTEN         CONFIG_LWIP_MAX_LISTENING_TCP

/**
 * MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
 * per active UDP "connection".
 * (requires the LWIP_UDP option)
 */
#define MEMP_NUM_UDP_PCB                CONFIG_LWIP_MAX_UDP_PCBS

/*
   --------------------------------
   ---------- ARP options -------
   --------------------------------
*/
/**
 * ARP_QUEUEING==1: Multiple outgoing packets are queued during hardware address
 * resolution. By default, only the most recent packet is queued per IP address.
 * This is sufficient for most protocols and mainly reduces TCP connection
 * startup time. Set this to 1 if you know your application sends more than one
 * packet in a row to an IP address that is not in the ARP cache.
 */
#define ARP_QUEUEING                    1

#ifdef CONFIG_LWIP_DHCPS_STATIC_ENTRIES
#define ETHARP_SUPPORT_STATIC_ENTRIES   1
#else
#define ETHARP_SUPPORT_STATIC_ENTRIES   0
#endif

/*
   --------------------------------
   ---------- IP options ----------
   --------------------------------
*/
/**
 * LWIP_IPV4==1: Enable IPv4
 */
#ifdef CONFIG_LWIP_IPV4
#define LWIP_IPV4                       1
#else
#define LWIP_IPV4                       0
#endif

/**
 * IP_REASSEMBLY==1: Reassemble incoming fragmented IP4 packets. Note that
 * this option does not affect outgoing packet sizes, which can be controlled
 * via IP_FRAG.
 */
#ifdef CONFIG_LWIP_IP4_REASSEMBLY
#define IP_REASSEMBLY                   1
#else
#define IP_REASSEMBLY                   0
#endif

/**
 * IP_FRAG==1: Fragment outgoing IP4 packets if their size exceeds MTU. Note
 * that this option does not affect incoming packet sizes, which can be
 * controlled via IP_REASSEMBLY.
 */
#ifdef CONFIG_LWIP_IP4_FRAG
#define IP_FRAG                         1
#else
#define IP_FRAG                         0
#endif

/**
 * IP_FORWARD==1: Enables the ability to forward IP packets across network
 * interfaces. If you are going to run lwIP on a device with only one network
 * interface, define this to 0.
 */
#ifdef CONFIG_LWIP_IP_FORWARD
#define IP_FORWARD                      1
#else
#define IP_FORWARD                      0
#endif

/**
 * IP_NAPT==1: Enables IPv4 Network Address and Port Translation.
 * Note that CONFIG_LWIP_IP_FORWARD option need to be enabled in
 * system configuration for the NAPT to work on ESP platform
 */
#ifdef CONFIG_LWIP_IPV4_NAPT
#define IP_NAPT                         1

#ifdef CONFIG_LWIP_IPV4_NAPT_PORTMAP
#define IP_NAPT_PORTMAP                 1
#else
#define IP_NAPT_PORTMAP                 0
#endif

#else
#define IP_NAPT                         0
#endif

/**
 * IP_REASS_MAXAGE: Maximum time (in multiples of IP_TMR_INTERVAL - so seconds, normally)
 * a fragmented IP packet waits for all fragments to arrive. If not all fragments arrived
 * in this time, the whole packet is discarded.
 */
#define IP_REASS_MAXAGE                 3

/**
 * IP_REASS_MAX_PBUFS: Total maximum amount of pbufs waiting to be reassembled.
 * Since the received pbufs are enqueued, be sure to configure
 * PBUF_POOL_SIZE > IP_REASS_MAX_PBUFS so that the stack is still able to receive
 * packets even if the maximum amount of fragments is enqueued for reassembly!
 */
#define IP_REASS_MAX_PBUFS              CONFIG_LWIP_IP_REASS_MAX_PBUFS

/**
 * IP_DEFAULT_TTL: Default value for Time-To-Live used by transport layers.
 */
#define IP_DEFAULT_TTL                   CONFIG_LWIP_IP_DEFAULT_TTL

/*
   ----------------------------------
   ---------- ICMP options ----------
   ----------------------------------
*/
/**
 * LWIP_ICMP==1: Enable ICMP module inside the IP stack.
 * Be careful, disable that make your product non-compliant to RFC1122
 */
#ifdef CONFIG_LWIP_ICMP
#define LWIP_ICMP                       1
#else
#define LWIP_ICMP                       0
#endif

/**
 * LWIP_BROADCAST_PING==1: respond to broadcast pings (default is unicast only)
 */
#ifdef CONFIG_LWIP_BROADCAST_PING
#define LWIP_BROADCAST_PING             1
#else
#define LWIP_BROADCAST_PING             0
#endif

/**
 * LWIP_MULTICAST_PING==1: respond to multicast pings (default is unicast only)
 */
#ifdef CONFIG_LWIP_MULTICAST_PING
#define LWIP_MULTICAST_PING             1
#else
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
#define LWIP_RAW                        1

/*
   ----------------------------------
   ---------- DHCP options ----------
   ----------------------------------
*/
#if CONFIG_LWIP_IPV4
/**
 * LWIP_DHCP==1: Enable DHCP module.
 */
#define LWIP_DHCP                       1

/**
 * DHCP_DOES_ARP_CHECK==1: Do an ARP check on the offered address.
 */
#ifdef CONFIG_LWIP_DHCP_DOES_ARP_CHECK
#define DHCP_DOES_ARP_CHECK             1
#else
#define DHCP_DOES_ARP_CHECK             0
#endif

/**
 * LWIP_DHCP_GETS_NTP==1: Request NTP servers with discover/select. For each
 * response packet, an callback is called, which has to be provided by the port:
 * void dhcp_set_ntp_servers(u8_t num_ntp_servers, ip_addr_t* ntp_server_addrs);
*/
#ifdef CONFIG_LWIP_DHCP_GET_NTP_SRV
#define LWIP_DHCP_GET_NTP_SRV           1
#else
#define LWIP_DHCP_GET_NTP_SRV           0
#endif

/**
 * ESP specific option only applicable if ESP_DHCP=1
 * LWIP_DHCP_DISABLE_CLIENT_ID==1: Do not add option 61 (client-id) to DHCP packets
 *
 */
#ifdef CONFIG_LWIP_DHCP_DISABLE_CLIENT_ID
#define ESP_DHCP_DISABLE_CLIENT_ID      1
#else
#define ESP_DHCP_DISABLE_CLIENT_ID      0
#endif

/**
 * ESP specific option only applicable if ESP_DHCP=1
 * CONFIG_LWIP_DHCP_RESTORE_LAST_IP==1: Last valid IP address obtained from DHCP server
 * is restored after reset/power-up.
 */
#ifdef CONFIG_LWIP_DHCP_RESTORE_LAST_IP
/*
 * Make the post-init hook check if we could restore the previously bound address
 * - if yes reset the state to bound and mark result as ERR_OK (which skips discovery state)
 * - if no, return false to continue normally to the discovery state
 */
#define LWIP_HOOK_DHCP_POST_INIT(netif, result) \
    (dhcp_ip_addr_restore(netif) ? ( dhcp_set_state(dhcp, DHCP_STATE_BOUND), \
                                     dhcp_network_changed(netif), \
                                     (result) = ERR_OK , \
        true ) : \
        false)
#else
#define LWIP_HOOK_DHCP_PRE_DISCOVERY(netif, result) (false)
#endif /* CONFIG_LWIP_DHCP_RESTORE_LAST_IP */

/**
 * The maximum of NTP servers requested
 */
#define LWIP_DHCP_MAX_NTP_SERVERS       CONFIG_LWIP_DHCP_MAX_NTP_SERVERS

/**
 * CONFIG_LWIP_DHCP_OPTIONS_LEN: The total length of outgoing DHCP option msg. If you have many options
 * and options value is too long, you can configure the length according to your requirements
 */
#define DHCP_OPTIONS_LEN                CONFIG_LWIP_DHCP_OPTIONS_LEN

/**
 * LWIP_DHCP_DISABLE_VENDOR_CLASS_ID==1: Do not add option 60 (Vendor Class Identifier) to DHCP packets
 */
#define ESP_DHCP_DISABLE_VENDOR_CLASS_IDENTIFIER       CONFIG_LWIP_DHCP_DISABLE_VENDOR_CLASS_ID

#define DHCP_DEFINE_CUSTOM_TIMEOUTS     1
#define DHCP_COARSE_TIMER_SECS          CONFIG_LWIP_DHCP_COARSE_TIMER_SECS
#define DHCP_NEXT_TIMEOUT_THRESHOLD     (3)
/* Since for embedded devices it's not that hard to miss a discover packet, so lower
 * the discover and request retry backoff time from (2,4,8,16,32,60,60)s to (500m,1,2,4,4,4,4)s.
 */
#define DHCP_REQUEST_TIMEOUT_SEQUENCE(tries)   ((uint16_t)(((tries) < 5 ? 1 << (tries) : 16) * 250))

static inline uint32_t timeout_from_offered(uint32_t lease, uint32_t min)
{
    uint32_t timeout = lease;
    if (timeout == 0) {
        timeout = min;
    }
    timeout = (timeout + DHCP_COARSE_TIMER_SECS - 1) / DHCP_COARSE_TIMER_SECS;
    return timeout;
}

#define DHCP_CALC_TIMEOUT_FROM_OFFERED_T0_LEASE(dhcp)  \
        timeout_from_offered((dhcp)->offered_t0_lease, 120)
#define DHCP_CALC_TIMEOUT_FROM_OFFERED_T1_RENEW(dhcp)  \
        timeout_from_offered((dhcp)->offered_t1_renew, (dhcp)->t0_timeout>>1 /* 50% */ )
#define DHCP_CALC_TIMEOUT_FROM_OFFERED_T2_REBIND(dhcp) \
        timeout_from_offered((dhcp)->offered_t2_rebind, ((dhcp)->t0_timeout/8)*7 /* 87.5% */ )

#define LWIP_HOOK_DHCP_PARSE_OPTION(netif, dhcp, state, msg, msg_type, option, len, pbuf, offset)   \
        do {    LWIP_UNUSED_ARG(msg);                                           \
                dhcp_parse_extra_opts(dhcp, state, option, len, pbuf, offset);  \
            } while(0)

#define LWIP_HOOK_DHCP_APPEND_OPTIONS(netif, dhcp, state, msg, msg_type, options_len_ptr) \
        dhcp_append_extra_opts(netif, state, msg, options_len_ptr);

#endif /* CONFIG_LWIP_IPV4 */
/*
   ------------------------------------
   ---------- AUTOIP options ----------
   ------------------------------------
*/
/**
 * LWIP_AUTOIP==1: Enable AUTOIP module.
 */
#ifdef CONFIG_LWIP_AUTOIP
#define LWIP_AUTOIP                     1

/**
* LWIP_DHCP_AUTOIP_COOP==1: Allow DHCP and AUTOIP to be both enabled on
* the same interface at the same time.
*/
#define LWIP_DHCP_AUTOIP_COOP           1

/**
* LWIP_DHCP_AUTOIP_COOP_TRIES: Set to the number of DHCP DISCOVER probes
* that should be sent before falling back on AUTOIP. This can be set
* as low as 1 to get an AutoIP address very quickly, but you should
* be prepared to handle a changing IP address when DHCP overrides
* AutoIP.
*/
#define LWIP_DHCP_AUTOIP_COOP_TRIES     CONFIG_LWIP_AUTOIP_TRIES

/**
 * ESP specific option only applicable if ESP_DHCP=1
 * LWIP_AUTOIP_MAX_CONFLICTS: Defines max conflicts before rate limiting
 */
#define LWIP_AUTOIP_MAX_CONFLICTS CONFIG_LWIP_AUTOIP_MAX_CONFLICTS

/**
 * ESP specific option only applicable if ESP_DHCP=1
 * LWIP_AUTOIP_RATE_LIMIT_INTERVAL: Delay in seconds between successive attempts
 */
#define LWIP_AUTOIP_RATE_LIMIT_INTERVAL CONFIG_LWIP_AUTOIP_RATE_LIMIT_INTERVAL

#endif /* CONFIG_LWIP_AUTOIP */

/*
   ----------------------------------
   ---------- IGMP options ----------
   ----------------------------------
*/
/**
 * LWIP_IGMP==1: Turn on IGMP module.
 */
#define LWIP_IGMP                       1

/*
   ----------------------------------
   ---------- DNS options -----------
   ----------------------------------
*/
/**
 * LWIP_DNS==1: Turn on DNS module. UDP must be available for DNS
 * transport.
 */
#define LWIP_DNS                        1

/** The maximum of DNS servers
 */
#define DNS_MAX_SERVERS                 CONFIG_LWIP_DNS_MAX_SERVERS

/** ESP specific option only applicable if ESP_DNS=1
 *
 * The last server can be initialized automatically by defining
 * FALLBACK_DNS_SERVER_ADDRESS(ipaddr), where 'ipaddr' is an 'ip_addr_t*'
 */
#define DNS_FALLBACK_SERVER_INDEX       (DNS_MAX_SERVERS - 1)

#ifdef CONFIG_LWIP_FALLBACK_DNS_SERVER_SUPPORT
#define FALLBACK_DNS_SERVER_ADDRESS(address)                           \
        do {    ip_addr_t *server_dns = address;                            \
                char server_ip[] = CONFIG_LWIP_FALLBACK_DNS_SERVER_ADDRESS; \
                ipaddr_aton(server_ip, server_dns);                         \
        } while (0)
#endif /* CONFIG_LWIP_FALLBACK_DNS_SERVER_SUPPORT */

/**
 * LWIP_DNS_SUPPORT_MDNS_QUERIES==1: Enable mDNS queries in hostname resolution.
 * This option is set via menuconfig.
 */
#ifdef CONFIG_LWIP_DNS_SUPPORT_MDNS_QUERIES
#define LWIP_DNS_SUPPORT_MDNS_QUERIES   1
#else
#define LWIP_DNS_SUPPORT_MDNS_QUERIES   0
#endif

/*
   ---------------------------------
   ---------- UDP options ----------
   ---------------------------------
*/
/**
 * LWIP_UDP==1: Turn on UDP.
 */
#define LWIP_UDP                        1

/**
 * LWIP_NETBUF_RECVINFO==1: Enable IP_PKTINFO option.
 * This option is set via menuconfig.
 */
#ifdef CONFIG_LWIP_NETBUF_RECVINFO
#define LWIP_NETBUF_RECVINFO            1
#else
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
#define LWIP_TCP                        1

/**
 * TCP_WND: The size of a TCP window.  This must be at least
 * (2 * TCP_MSS) for things to work well.
 * ATTENTION: when using TCP_RCV_SCALE, TCP_WND is the total size
 * with scaling applied. Maximum window value in the TCP header
 * will be TCP_WND >> TCP_RCV_SCALE
 */
#define TCP_WND                         CONFIG_LWIP_TCP_WND_DEFAULT

/**
 * TCP_MAXRTX: Maximum number of retransmissions of data segments.
 */
#define TCP_MAXRTX                      CONFIG_LWIP_TCP_MAXRTX

/**
 * TCP_SYNMAXRTX: Maximum number of retransmissions of SYN segments.
 */
#define TCP_SYNMAXRTX                   CONFIG_LWIP_TCP_SYNMAXRTX

/**
 * TCP_QUEUE_OOSEQ==1: TCP will queue segments that arrive out of order.
 * Define to 0 if your device is low on memory.
 */
#ifdef CONFIG_LWIP_TCP_QUEUE_OOSEQ
#define TCP_QUEUE_OOSEQ                 1
#else
#define TCP_QUEUE_OOSEQ                 0
#endif

/**
 * TCP_OOSEQ_MAX_PBUFS: The maximum number of pbufs
 * queued on ooseq per pcb
 */
#if TCP_QUEUE_OOSEQ
#define TCP_OOSEQ_MAX_PBUFS             CONFIG_LWIP_TCP_OOSEQ_MAX_PBUFS
#endif

/**
 * TCP_OOSEQ_TIMEOUT: Timeout for each pbuf queued in TCP OOSEQ, in RTOs.
 */
#if TCP_QUEUE_OOSEQ
#define TCP_OOSEQ_TIMEOUT               CONFIG_LWIP_TCP_OOSEQ_TIMEOUT
#endif

/**
 * LWIP_TCP_SACK_OUT==1: TCP will support sending selective acknowledgements (SACKs).
 */
#ifdef CONFIG_LWIP_TCP_SACK_OUT
#define LWIP_TCP_SACK_OUT               1
#else
#define LWIP_TCP_SACK_OUT               0
#endif

/**
 * TCP_MSS: TCP Maximum segment size. (default is 536, a conservative default,
 * you might want to increase this.)
 * For the receive side, this MSS is advertised to the remote side
 * when opening a connection. For the transmit size, this MSS sets
 * an upper limit on the MSS advertised by the remote host.
 */
#define TCP_MSS                         CONFIG_LWIP_TCP_MSS

/**
 * TCP_SND_BUF: TCP sender buffer space (bytes).
 * To achieve good performance, this should be at least 2 * TCP_MSS.
 */
#define TCP_SND_BUF                     CONFIG_LWIP_TCP_SND_BUF_DEFAULT


/**
 * TCP_LISTEN_BACKLOG: Enable the backlog option for tcp listen pcb.
 */
#define TCP_LISTEN_BACKLOG              1

/**
 * TCP_OVERSIZE: The maximum number of bytes that tcp_write may
 * allocate ahead of time
 */
#ifdef CONFIG_LWIP_TCP_OVERSIZE_MSS
#define TCP_OVERSIZE                    TCP_MSS
#endif
#ifdef CONFIG_LWIP_TCP_OVERSIZE_QUARTER_MSS
#define TCP_OVERSIZE                    (TCP_MSS/4)
#endif
#ifdef CONFIG_LWIP_TCP_OVERSIZE_DISABLE
#define TCP_OVERSIZE                    0
#endif
#ifndef TCP_OVERSIZE
#error "One of CONFIG_TCP_OVERSIZE_xxx options should be set by sdkconfig"
#endif

/**
 * LWIP_EVENT_API and LWIP_CALLBACK_API: Only one of these should be set to 1.
 *     LWIP_EVENT_API==1: The user defines lwip_tcp_event() to receive all
 *         events (accept, sent, etc) that happen in the system.
 *     LWIP_CALLBACK_API==1: The PCB callback function is called directly
 *         for the event. This is the default.
 */
#define LWIP_EVENT_API                  0
#define LWIP_CALLBACK_API               1

/**
 * TCP_TMR_INTERVAL: TCP timer interval
 */
#define TCP_TMR_INTERVAL                CONFIG_LWIP_TCP_TMR_INTERVAL

/**
 * TCP_MSL: The maximum segment lifetime in milliseconds
 */
#define TCP_MSL                         CONFIG_LWIP_TCP_MSL

/**
 * TCP_FIN_WAIT_TIMEOUT: The maximum FIN segment lifetime in milliseconds
 */
#define TCP_FIN_WAIT_TIMEOUT            CONFIG_LWIP_TCP_FIN_WAIT_TIMEOUT

/**
 * LWIP_WND_SCALE and TCP_RCV_SCALE:
 * Set LWIP_WND_SCALE to 1 to enable window scaling.
 * Set TCP_RCV_SCALE to the desired scaling factor (shift count in the
 * range of [0..14]).
 * When LWIP_WND_SCALE is enabled but TCP_RCV_SCALE is 0, we can use a large
 * send window while having a small receive window only.
 */
#ifdef CONFIG_LWIP_WND_SCALE
#define LWIP_WND_SCALE                  1
#define TCP_RCV_SCALE                   CONFIG_LWIP_TCP_RCV_SCALE
#else
#define LWIP_WND_SCALE                  0
#define TCP_RCV_SCALE                   0
#endif /* CONFIG_LWIP_WND_SCALE */

/**
 * ESP specific option only applicable if ESP_LWIP=1
 * LWIP_TCP_RTO_TIME: TCP rto time.
 * Default is 3 second.
 */
#define LWIP_TCP_RTO_TIME             CONFIG_LWIP_TCP_RTO_TIME

/*
   ------------------------------------------------
   ---------- Network Interfaces options ----------
   ------------------------------------------------
*/

/**
 * LWIP_NETIF_HOSTNAME==1: use DHCP_OPTION_HOSTNAME with netif's hostname
 * field.
 */
#define LWIP_NETIF_HOSTNAME             1

/**
  * LWIP_NETIF_API==1: Support netif api (in netifapi.c)
 */
#ifdef CONFIG_LWIP_NETIF_API
#define LWIP_NETIF_API                  1
#else
#define LWIP_NETIF_API                  0
#endif

/**
 * LWIP_NETIF_STATUS_CALLBACK==1: Support a callback function whenever an interface
 * changes its up/down status (i.e., due to DHCP IP acquisition)
 */
#ifdef CONFIG_LWIP_NETIF_STATUS_CALLBACK
#define LWIP_NETIF_STATUS_CALLBACK      1
#else
#define LWIP_NETIF_STATUS_CALLBACK      0
#endif

/**
 * LWIP_NETIF_EXT_STATUS_CALLBACK==1: Support an extended callback function
 * for several netif related event that supports multiple subscribers.
 *
 * This ext-callback is used by ESP-NETIF with lwip-orig (upstream version)
 * to provide netif related events on IP4/IP6 address/status changes
 */
#define LWIP_NETIF_EXT_STATUS_CALLBACK  1

/**
 * LWIP_NETIF_TX_SINGLE_PBUF: if this is set to 1, lwIP *tries* to put all data
 * to be sent into one single pbuf. This is for compatibility with DMA-enabled
 * MACs that do not support scatter-gather.
 * Beware that this might involve CPU-memcpy before transmitting that would not
 * be needed without this flag! Use this only if you need to!
 *
 * ATTENTION: a driver should *NOT* rely on getting single pbufs but check TX
 * pbufs for being in one piece. If not, @ref pbuf_clone can be used to get
 * a single pbuf:
 *   if (p->next != NULL) {
 *     struct pbuf *q = pbuf_clone(PBUF_RAW, PBUF_RAM, p);
 *     if (q == NULL) {
 *       return ERR_MEM;
 *     }
 *     p = q; ATTENTION: do NOT free the old 'p' as the ref belongs to the caller!
 *   }
 */
#define LWIP_NETIF_TX_SINGLE_PBUF       1

/**
 * LWIP_NUM_NETIF_CLIENT_DATA: Number of clients that may store
 * data in client_data member array of struct netif (max. 256).
 */
#ifndef CONFIG_LWIP_NUM_NETIF_CLIENT_DATA
#define CONFIG_LWIP_NUM_NETIF_CLIENT_DATA 0
#endif
#if defined(CONFIG_ESP_NETIF_BRIDGE_EN) || defined(CONFIG_LWIP_PPP_SUPPORT)
/*
 * If special lwip interfaces (like bridge, ppp) enabled
 * `netif->state` is used internally and we must store esp-netif ptr
 * in `netif->client_data`
 */
#define LWIP_ESP_NETIF_DATA             (1)
#else
#define LWIP_ESP_NETIF_DATA             (0)
#endif

#define LWIP_NUM_NETIF_CLIENT_DATA      (LWIP_ESP_NETIF_DATA + CONFIG_LWIP_NUM_NETIF_CLIENT_DATA)

/**
 * BRIDGEIF_MAX_PORTS: this is used to create a typedef used for forwarding
 * bit-fields: the number of bits required is this + 1 (for the internal/cpu port)
 */
#ifdef CONFIG_LWIP_BRIDGEIF_MAX_PORTS
#define BRIDGEIF_MAX_PORTS       CONFIG_LWIP_BRIDGEIF_MAX_PORTS
#endif

/*
   ------------------------------------
   ---------- LOOPIF options ----------
   ------------------------------------
*/
/**
 * LWIP_HAVE_LOOPIF==1: Support loop interface (127.0.0.1).
 * This is only needed when no real netifs are available. If at least one other
*/
#ifdef CONFIG_LWIP_NETIF_LOOPBACK
#define LWIP_HAVE_LOOPIF                1

/**
 * LWIP_NETIF_LOOPBACK==1: Support sending packets with a destination IP
 * address equal to the netif IP address, looping them back up the stack.
 */
#define LWIP_NETIF_LOOPBACK             1

/**
 * LWIP_LOOPBACK_MAX_PBUFS: Maximum number of pbufs on queue for loopback
 * sending for each netif (0 = disabled)
 */
#define LWIP_LOOPBACK_MAX_PBUFS         CONFIG_LWIP_LOOPBACK_MAX_PBUFS
#else
#define LWIP_HAVE_LOOPIF                0
#endif

/*
   ------------------------------------
   ---------- SLIPIF options ----------
   ------------------------------------
*/

#ifdef CONFIG_LWIP_SLIP_SUPPORT
/**
 * Enable SLIP receive from ISR functions and disable Rx thread
 *
 * This is the only supported mode of lwIP SLIP interface, so that
 * - incoming packets are queued into pbufs
 * - no thread is created from lwIP
 * meaning it is the application responsibility to read data
 * from IO driver and feed them to the slip interface
 */
#define SLIP_RX_FROM_ISR                 1
#define SLIP_USE_RX_THREAD               0

/**
 * SLIP_DEBUG: Enable debugging for SLIP.
 */
#ifdef CONFIG_LWIP_SLIP_DEBUG_ON
#define SLIP_DEBUG                       LWIP_DBG_ON
#else
#define SLIP_DEBUG                       LWIP_DBG_OFF
#endif
#endif /* CONFIG_LWIP_SLIP_SUPPORT */

/*
   ------------------------------------
   ---------- Thread options ----------
   ------------------------------------
*/
/**
 * TCPIP_THREAD_NAME: The name assigned to the main tcpip thread.
 */
#define TCPIP_THREAD_NAME              "tiT"

/**
 * TCPIP_THREAD_STACKSIZE: The stack size used by the main tcpip thread.
 * The stack size value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#define TCPIP_THREAD_STACKSIZE          ESP_TASK_TCPIP_STACK

/**
 * TCPIP_THREAD_PRIO: The priority assigned to the main tcpip thread.
 * The priority value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#define TCPIP_THREAD_PRIO               ESP_TASK_TCPIP_PRIO

/**
 * TCPIP_MBOX_SIZE: The mailbox size for the tcpip thread messages
 * The queue size value itself is platform-dependent, but is passed to
 * sys_mbox_new() when tcpip_init is called.
 */
#define TCPIP_MBOX_SIZE                 CONFIG_LWIP_TCPIP_RECVMBOX_SIZE

/**
 * DEFAULT_UDP_RECVMBOX_SIZE: The mailbox size for the incoming packets on a
 * NETCONN_UDP. The queue size value itself is platform-dependent, but is passed
 * to sys_mbox_new() when the recvmbox is created.
 */
#define DEFAULT_UDP_RECVMBOX_SIZE       CONFIG_LWIP_UDP_RECVMBOX_SIZE

/**
 * DEFAULT_TCP_RECVMBOX_SIZE: The mailbox size for the incoming packets on a
 * NETCONN_TCP. The queue size value itself is platform-dependent, but is passed
 * to sys_mbox_new() when the recvmbox is created.
 */
#define DEFAULT_TCP_RECVMBOX_SIZE       CONFIG_LWIP_TCP_RECVMBOX_SIZE

/**
 * DEFAULT_ACCEPTMBOX_SIZE: The mailbox size for the incoming connections.
 * The queue size value itself is platform-dependent, but is passed to
 * sys_mbox_new() when the acceptmbox is created.
 */
#define DEFAULT_ACCEPTMBOX_SIZE         CONFIG_LWIP_TCP_ACCEPTMBOX_SIZE

/**
 * DEFAULT_THREAD_STACKSIZE: The stack size used by any other lwIP thread.
 * The stack size value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#define DEFAULT_THREAD_STACKSIZE        TCPIP_THREAD_STACKSIZE

/**
 * DEFAULT_THREAD_PRIO: The priority assigned to any other lwIP thread.
 * The priority value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#define DEFAULT_THREAD_PRIO             TCPIP_THREAD_PRIO

/**
 * DEFAULT_RAW_RECVMBOX_SIZE: The mailbox size for the incoming packets on a
 * NETCONN_RAW. The queue size value itself is platform-dependent, but is passed
 * to sys_mbox_new() when the recvmbox is created.
 */
#define DEFAULT_RAW_RECVMBOX_SIZE       6

/*
   ----------------------------------------------
   ---------- Sequential layer options ----------
   ----------------------------------------------
*/
/**
 * LWIP_NETCONN==1: Enable Netconn API (require to use api_lib.c)
 */
#define LWIP_NETCONN                    1

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
#define LWIP_NETCONN_SEM_PER_THREAD     1

/** LWIP_NETCONN_FULLDUPLEX==1: Enable code that allows reading from one thread,
 * writing from a 2nd thread and closing from a 3rd thread at the same time.
 * ATTENTION: This is currently really alpha! Some requirements:
 * - LWIP_NETCONN_SEM_PER_THREAD==1 is required to use one socket/netconn from
 *   multiple threads at once
 * - sys_mbox_free() has to unblock receive tasks waiting on recvmbox/acceptmbox
 *   and prevent a task pending on this during/after deletion
 */
#define LWIP_NETCONN_FULLDUPLEX         1

/*
   ------------------------------------
   ---------- Socket options ----------
   ------------------------------------
*/
/**
 * LWIP_SOCKET==1: Enable Socket API (require to use sockets.c)
 */
#define LWIP_SOCKET                     1

/**
 * LWIP_COMPAT_SOCKETS==1: Enable BSD-style sockets functions names through defines.
 * LWIP_COMPAT_SOCKETS==2: Same as ==1 but correctly named functions are created.
 * While this helps code completion, it might conflict with existing libraries.
 * (only used if you use sockets.c)
 */
#define LWIP_COMPAT_SOCKETS             0

/**
 * LWIP_POSIX_SOCKETS_IO_NAMES==1: Enable POSIX-style sockets functions names.
 * Disable this option if you use a POSIX operating system that uses the same
 * names (read, write & close). (only used if you use sockets.c)
 *
 * POSIX I/O functions are mapped to LWIP via the VFS layer
 * (see port/vfs_lwip.c)
 */
/**
 */
#define LWIP_POSIX_SOCKETS_IO_NAMES     0

/**
 * LWIP_SOCKET_OFFSET==n: Increases the file descriptor number created by LwIP with n.
 *
 * FD_SETSIZE from sys/types.h is the maximum number of supported file
 * descriptors and CONFIG_LWIP_MAX_SOCKETS defines the number of sockets;
 * LWIP_SOCKET_OFFSET is configured to use the largest numbers of file
 * descriptors for sockets. File descriptors from 0 to LWIP_SOCKET_OFFSET-1
 * are non-socket descriptors and from LWIP_SOCKET_OFFSET to FD_SETSIZE are
 * socket descriptors.
 */
#define LWIP_SOCKET_OFFSET              (FD_SETSIZE - CONFIG_LWIP_MAX_SOCKETS)

/**
 * LWIP_SO_SNDTIMEO==1: Enable send timeout for sockets/netconns and
 * SO_SNDTIMEO processing.
 */
#define LWIP_SO_SNDTIMEO                1

/**
 * LWIP_SO_RCVTIMEO==1: Enable receive timeout for sockets/netconns and
 * SO_RCVTIMEO processing.
 */
#define LWIP_SO_RCVTIMEO                1

/**
 * LWIP_TCP_KEEPALIVE==1: Enable TCP_KEEPIDLE, TCP_KEEPINTVL and TCP_KEEPCNT
 * options processing. Note that TCP_KEEPIDLE and TCP_KEEPINTVL have to be set
 * in seconds. (does not require sockets.c, and will affect tcp.c)
 */
#define LWIP_TCP_KEEPALIVE              1

/**
 * LWIP_SO_LINGER==1: Enable SO_LINGER processing.
 * This option is set via menuconfig.
 */
#ifdef CONFIG_LWIP_SO_LINGER
#define LWIP_SO_LINGER                  1
#else
#define LWIP_SO_LINGER                  0
#endif

/**
 * LWIP_SO_RCVBUF==1: Enable SO_RCVBUF processing.
 * This option is set via menuconfig.
 */
#ifdef CONFIG_LWIP_SO_RCVBUF
#define LWIP_SO_RCVBUF                  1
#else
#define LWIP_SO_RCVBUF                  0
#endif

/**
 * SO_REUSE==1: Enable SO_REUSEADDR option.
 * This option is set via menuconfig.
 */
#ifdef CONFIG_LWIP_SO_REUSE
#define SO_REUSE                        1
#else
#define SO_REUSE                        0
#endif

/**
 * SO_REUSE_RXTOALL==1: Pass a copy of incoming broadcast/multicast packets
 * to all local matches if SO_REUSEADDR is turned on.
 * WARNING: Adds a memcpy for every packet if passing to more than one pcb!
 */
#ifdef CONFIG_LWIP_SO_REUSE_RXTOALL
#define SO_REUSE_RXTOALL                1
#else
#define SO_REUSE_RXTOALL                0
#endif

/** LWIP_TIMEVAL_PRIVATE: if you want to use the struct timeval provided
 * by your system, set this to 0 and include <sys/time.h> in cc.h */
#define LWIP_TIMEVAL_PRIVATE            0

/*
   ----------------------------------------
   ---------- Statistics options ----------
   ----------------------------------------
*/

/**
 * LWIP_STATS==1: Enable statistics collection in lwip_stats.
 */
#ifdef CONFIG_LWIP_STATS
#define LWIP_STATS                      1
#define ESP_STATS_MEM                   1

/**
 * LWIP_STATS_DISPLAY==1: Compile in the statistics output functions.
 */
#define LWIP_STATS_DISPLAY              1
#else
#define LWIP_STATS                      0
#define ESP_STATS_MEM                   0
#endif /* CONFIG_LWIP_STATS */

/*
   ---------------------------------
   ---------- PPP options ----------
   ---------------------------------
*/

/**
 * PPP_SUPPORT==1: Enable PPP.
 */
#ifdef CONFIG_LWIP_PPP_SUPPORT
#define PPP_SUPPORT                     1

/**
 * PPP_IPV6_SUPPORT == 1: Enable IPV6 support for local link
 * between modem and lwIP stack.
 * Some modems do not support IPV6 addressing in local link and
 * the only option available is to disable IPV6 address negotiation.
 */
#define PPP_IPV6_SUPPORT                               CONFIG_LWIP_PPP_ENABLE_IPV6

/**
 * PPP_NOTIFY_PHASE==1: Support PPP notify phase.
 */
#define PPP_NOTIFY_PHASE                CONFIG_LWIP_PPP_NOTIFY_PHASE_SUPPORT

/**
 * PAP_SUPPORT==1: Support PAP.
 */
#define PAP_SUPPORT                     CONFIG_LWIP_PPP_PAP_SUPPORT

/**
 * CHAP_SUPPORT==1: Support CHAP.
 */
#define CHAP_SUPPORT                    CONFIG_LWIP_PPP_CHAP_SUPPORT

/**
 * MSCHAP_SUPPORT==1: Support MSCHAP.
 */
#define MSCHAP_SUPPORT                  CONFIG_LWIP_PPP_MSCHAP_SUPPORT

/**
 * CCP_SUPPORT==1: Support CCP.
 */
#define MPPE_SUPPORT                    CONFIG_LWIP_PPP_MPPE_SUPPORT

/**
 * PPP_SERVER==1: Enable PPP server support (waiting for incoming PPP session).
 */
#define PPP_SERVER                      CONFIG_LWIP_PPP_SERVER_SUPPORT

/**
 * VJ_SUPPORT==1: Support VJ header compression.
 */
#define VJ_SUPPORT                      CONFIG_LWIP_PPP_VJ_HEADER_COMPRESSION

/**
 * PPP_MAXIDLEFLAG: Max Xmit idle time (in ms) before resend flag char.
 * TODO: If PPP_MAXIDLEFLAG > 0 and next package is send during PPP_MAXIDLEFLAG time,
 *       then 0x7E is not added at the begining of PPP package but 0x7E termination
 *       is always at the end. This behaviour brokes PPP dial with GSM (PPPoS).
 *       The PPP package should always start and end with 0x7E.
 */

#define PPP_MAXIDLEFLAG                 0

#ifdef CONFIG_LWIP_ENABLE_LCP_ECHO
/**
 * LCP_ECHOINTERVAL: Interval in seconds between keepalive LCP echo requests, 0 to disable.
 */
#define LCP_ECHOINTERVAL                CONFIG_LWIP_LCP_ECHOINTERVAL

/**
 * LCP_MAXECHOFAILS: Number of consecutive unanswered echo requests before failure is indicated.
 */
#define LCP_MAXECHOFAILS                CONFIG_LWIP_LCP_MAXECHOFAILS
#endif /* CONFIG_LWIP_ENABLE_LCP_ECHO */

/**
 * PPP_DEBUG: Enable debugging for PPP.
 */
#ifdef CONFIG_LWIP_PPP_DEBUG_ON
#define PPP_DEBUG                       LWIP_DBG_ON
#define PRINTPKT_SUPPORT                1
#define PPP_PROTOCOLNAME                1
#else
#define PPP_DEBUG                       LWIP_DBG_OFF
#endif /* CONFIG_LWIP_PPP_DEBUG_ON */
#else
#define PPP_SUPPORT                     0
#endif  /* CONFIG_LWIP_PPP_SUPPORT */

/*
   --------------------------------------
   ---------- Checksum options ----------
   --------------------------------------
*/

/**
 * CHECKSUM_CHECK_IP==1: Check checksums in software for incoming IP packets.
 */
#ifdef CONFIG_LWIP_CHECKSUM_CHECK_IP
#define CHECKSUM_CHECK_IP               1
#else
#define CHECKSUM_CHECK_IP               0
#endif

/**
 * CHECKSUM_CHECK_UDP==1: Check checksums in software for incoming UDP packets.
 */
#ifdef CONFIG_LWIP_CHECKSUM_CHECK_UDP
#define CHECKSUM_CHECK_UDP              1
#else
#define CHECKSUM_CHECK_UDP              0
#endif

/**
 * CHECKSUM_CHECK_ICMP==1: Check checksums in software for incoming ICMP packets.
 */
#ifdef CONFIG_LWIP_CHECKSUM_CHECK_ICMP
#define CHECKSUM_CHECK_ICMP             1
#else
#define CHECKSUM_CHECK_ICMP             0
#endif

/*
   ---------------------------------------
   ---------- IPv6 options ---------------
   ---------------------------------------
*/
/**
 * LWIP_IPV6==1: Enable IPv6
 */
#ifdef CONFIG_LWIP_IPV6
#define LWIP_IPV6                       1
#else
#define LWIP_IPV6                       0
#endif

/**
 * LWIP_ND6==1: Enable ND6 protocol in IPv6
 */
#ifdef CONFIG_LWIP_ND6
#define LWIP_ND6                        1
#else
#define LWIP_ND6                        0
#endif

/**
 * LWIP_FORCE_ROUTER_FORWARDING==1: the router flag in NA packet will always set to 1,
 * otherwise, never set router flag for NA packets.
 */
#ifdef CONFIG_LWIP_FORCE_ROUTER_FORWARDING
#define LWIP_FORCE_ROUTER_FORWARDING    1
#else
#define LWIP_FORCE_ROUTER_FORWARDING    0
#endif

/**
 * LWIP_IPV6_NUM_ADDRESSES: Number of IPv6 addresses per netif.
 */
#define LWIP_IPV6_NUM_ADDRESSES         CONFIG_LWIP_IPV6_NUM_ADDRESSES

/**
 * LWIP_IPV6_FORWARD==1: Forward IPv6 packets across netifs
 */
#ifdef CONFIG_LWIP_IPV6_FORWARD
#define LWIP_IPV6_FORWARD               1
#else
#define LWIP_IPV6_FORWARD               0
#endif

/**
 * LWIP_IPV6_FRAG==1: Fragment outgoing IP6 packets if their size exceeds MTU. Note
 * that this option does not affect incoming packet sizes, which can be
 * controlled via IP6_REASSEMBLY.
 */
#ifdef CONFIG_LWIP_IP6_FRAG
#define LWIP_IPV6_FRAG                  1
#else
#define LWIP_IPV6_FRAG                  0
#endif

/**
 * LWIP_IPV6_REASS==1: reassemble incoming IP6 packets that fragmented. Note that
 * this option does not affect outgoing packet sizes, which can be controlled
 * via LWIP_IP6_FRAG.
 */
#ifdef CONFIG_LWIP_IP6_REASSEMBLY
#define LWIP_IPV6_REASS                 1
#else
#define LWIP_IPV6_REASS                 0
#endif

/**
 * LWIP_IPV6_AUTOCONFIG==1: Enable stateless address autoconfiguration as per RFC 4862.
 */
#ifdef CONFIG_LWIP_IPV6_AUTOCONFIG
#define ESP_IPV6_AUTOCONFIG             1
#else
#define ESP_IPV6_AUTOCONFIG             0
#endif

/**
 * LWIP_ND6_RDNSS_MAX_DNS_SERVERS > 0: Use IPv6 Router Advertisement Recursive
 * DNS Server Option (as per RFC 6106) to copy a defined maximum number of DNS
 * servers to the DNS module.
 */
#define LWIP_ND6_RDNSS_MAX_DNS_SERVERS  CONFIG_LWIP_IPV6_RDNSS_MAX_DNS_SERVERS

/**
 * LWIP_IPV6_DHCP6==1: enable DHCPv6 stateful/stateless address autoconfiguration.
 */
#ifdef CONFIG_LWIP_IPV6_DHCP6
#define LWIP_IPV6_DHCP6                 1
#else
#define LWIP_IPV6_DHCP6                 0
#endif

/**
 * MEMP_NUM_ND6_QUEUE: Max number of IPv6 packets to queue during MAC resolution.
 */
#define MEMP_NUM_ND6_QUEUE              CONFIG_LWIP_IPV6_MEMP_NUM_ND6_QUEUE

/**
 * LWIP_ND6_NUM_NEIGHBORS: Number of entries in IPv6 neighbor cache
 */
#define LWIP_ND6_NUM_NEIGHBORS          CONFIG_LWIP_IPV6_ND6_NUM_NEIGHBORS

/*
   ---------------------------------------
   ---------- Hook options ---------------
   ---------------------------------------
*/
#ifdef LWIP_HOOK_FILENAME
#warning LWIP_HOOK_FILENAME is used for IDF default hooks. Please use ESP_IDF_LWIP_HOOK_FILENAME to insert additional hook
#endif
#define LWIP_HOOK_FILENAME              "lwip_default_hooks.h"
#define LWIP_HOOK_IP4_ROUTE_SRC         ip4_route_src_hook
#if LWIP_NETCONN_FULLDUPLEX
#define LWIP_DONE_SOCK(sock)            done_socket(sock)
#else
#define LWIP_DONE_SOCK(sock)            ((void)1)
#endif /* LWIP_NETCONN_FULLDUPLEX */

#define LWIP_HOOK_SOCKETS_GETSOCKOPT(s, sock, level, optname, optval, optlen, err)    \
        lwip_getsockopt_impl_ext(sock, level, optname, optval, optlen, err)?(LWIP_DONE_SOCK(sock), true): false

#define LWIP_HOOK_SOCKETS_SETSOCKOPT(s, sock, level, optname, optval, optlen, err)    \
        lwip_setsockopt_impl_ext(sock, level, optname, optval, optlen, err)?(LWIP_DONE_SOCK(sock), true): false

/*
   ---------------------------------------
   ---------- Debugging options ----------
   ---------------------------------------
*/
/**
 * LWIP_DEBUG: Enable lwip debugging in other modules.
 */
#ifdef CONFIG_LWIP_DEBUG
#define LWIP_DEBUG                      LWIP_DBG_ON
#else
#undef LWIP_DEBUG
#endif

/**
 * ETHARP_DEBUG: Enable debugging in etharp.c.
 */
#ifdef CONFIG_LWIP_ETHARP_DEBUG
#define ETHARP_DEBUG                     LWIP_DBG_ON
#else
#define ETHARP_DEBUG                     LWIP_DBG_OFF
#endif

/**
 * NETIF_DEBUG: Enable debugging in netif.c.
 */
#ifdef CONFIG_LWIP_NETIF_DEBUG
#define NETIF_DEBUG                     LWIP_DBG_ON
#else
#define NETIF_DEBUG                     LWIP_DBG_OFF
#endif

/**
 * PBUF_DEBUG: Enable debugging in pbuf.c.
 */
#ifdef CONFIG_LWIP_PBUF_DEBUG
#define PBUF_DEBUG                     LWIP_DBG_ON
#else
#define PBUF_DEBUG                     LWIP_DBG_OFF
#endif

/**
 * API_LIB_DEBUG: Enable debugging in api_lib.c.
 */
#ifdef CONFIG_LWIP_API_LIB_DEBUG
#define API_LIB_DEBUG                     LWIP_DBG_ON
#else
#define API_LIB_DEBUG                     LWIP_DBG_OFF
#endif

/**
 * SOCKETS_DEBUG: Enable debugging in sockets.c.
 */
#ifdef CONFIG_LWIP_SOCKETS_DEBUG
#define SOCKETS_DEBUG                   LWIP_DBG_ON
#else
#define SOCKETS_DEBUG                   LWIP_DBG_OFF
#endif

/**
 * ICMP_DEBUG: Enable debugging in icmp.c.
 */
#ifdef CONFIG_LWIP_ICMP_DEBUG
#define ICMP_DEBUG                      LWIP_DBG_ON
#else
#define ICMP_DEBUG                      LWIP_DBG_OFF
#endif

#ifdef CONFIG_LWIP_ICMP6_DEBUG
#define ICMP6_DEBUG                      LWIP_DBG_ON
#else
#define ICMP6_DEBUG                      LWIP_DBG_OFF
#endif

/**
 * DHCP_DEBUG: Enable debugging in dhcp.c.
 */
#ifdef CONFIG_LWIP_DHCP_DEBUG
#define DHCP_DEBUG                      LWIP_DBG_ON
#else
#define DHCP_DEBUG                      LWIP_DBG_OFF
#endif

#ifdef CONFIG_LWIP_DHCP_STATE_DEBUG
#define ESP_DHCP_DEBUG                  LWIP_DBG_ON
#else
#define ESP_DHCP_DEBUG                  LWIP_DBG_OFF
#endif

/**
 * IP_DEBUG: Enable debugging for IP.
 */
#ifdef CONFIG_LWIP_IP_DEBUG
#define IP_DEBUG                        LWIP_DBG_ON
#else
#define IP_DEBUG                        LWIP_DBG_OFF
#endif

/**
 * IP6_DEBUG: Enable debugging for IP6.
 */
#ifdef CONFIG_LWIP_IP6_DEBUG
#define IP6_DEBUG                        LWIP_DBG_ON
#else
#define IP6_DEBUG                        LWIP_DBG_OFF
#endif

/**
 * TCP_DEBUG: Enable debugging for TCP.
 */
#ifdef CONFIG_LWIP_TCP_DEBUG
#define TCP_DEBUG                        LWIP_DBG_ON
#else
#define TCP_DEBUG                        LWIP_DBG_OFF
#endif

/**
 * UDP_DEBUG: Enable debugging for UDP.
 */
#ifdef CONFIG_LWIP_UDP_DEBUG
#define UDP_DEBUG                        LWIP_DBG_ON
#else
#define UDP_DEBUG                        LWIP_DBG_OFF
#endif

/**
 * SNTP_DEBUG: Enable debugging for SNTP.
 */
#ifdef CONFIG_LWIP_SNTP_DEBUG
#define SNTP_DEBUG                       LWIP_DBG_ON
#else
#define SNTP_DEBUG                       LWIP_DBG_OFF
#endif

/**
 * DNS_DEBUG: Enable debugging for DNS.
 */
#ifdef CONFIG_LWIP_DNS_DEBUG
#define DNS_DEBUG                       LWIP_DBG_ON
#else
#define DNS_DEBUG                       LWIP_DBG_OFF
#endif

/**
 * NAPT_DEBUG: Enable debugging for NAPT.
 */
#ifdef CONFIG_LWIP_NAPT_DEBUG
#define NAPT_DEBUG                      LWIP_DBG_ON
#else
#define NAPT_DEBUG                      LWIP_DBG_OFF
#endif

/**
 * MEMP_DEBUG: Enable debugging in memp.c.
 */
#define MEMP_DEBUG                      LWIP_DBG_OFF

/**
 * TCP_INPUT_DEBUG: Enable debugging in tcp_in.c for incoming debug.
 */
#define TCP_INPUT_DEBUG                 LWIP_DBG_OFF

/**
 * TCP_OUTPUT_DEBUG: Enable debugging in tcp_out.c output functions.
 */
#define TCP_OUTPUT_DEBUG                LWIP_DBG_OFF

/**
 * TCPIP_DEBUG: Enable debugging in tcpip.c.
 */
#define TCPIP_DEBUG                     LWIP_DBG_OFF

/**
 * TCP_OOSEQ_DEBUG: Enable debugging in tcpin.c for OOSEQ.
 */
#define TCP_OOSEQ_DEBUG                 LWIP_DBG_OFF

/**
 * BRIDGEIF_DEBUG: Enable generic debugging for bridge.
 */
#ifdef CONFIG_LWIP_BRIDGEIF_DEBUG
#define BRIDGEIF_DEBUG                  LWIP_DBG_ON
#endif

/**
 * BRIDGEIF_FDB_DEBUG: Enable debugging for bridge FDB.
 */
#ifdef CONFIG_LWIP_BRIDGEIF_FDB_DEBUG
#define BRIDGEIF_FDB_DEBUG              LWIP_DBG_ON
#endif

/**
 * BRIDGEIF_FW_DEBUG: Enable debugging for bridge forwarding.
 */
#ifdef CONFIG_LWIP_BRIDGEIF_FW_DEBUG
#define BRIDGEIF_FW_DEBUG               LWIP_DBG_ON
#endif

/*
   --------------------------------------
   ------------ SNTP options ------------
   --------------------------------------
*/
/**
 * Max number of SNTP servers handled (default equal to LWIP_DHCP_MAX_NTP_SERVERS)
 */
#ifdef CONFIG_LWIP_SNTP_MAX_SERVERS
#define SNTP_MAX_SERVERS                CONFIG_LWIP_SNTP_MAX_SERVERS
#endif /* CONFIG_LWIP_SNTP_MAX_SERVERS */

/** Set this to 1 to support DNS names (or IP address strings) to set sntp servers
 * One server address/name can be defined as default if SNTP_SERVER_DNS == 1:
 * \#define SNTP_SERVER_ADDRESS "pool.ntp.org"
 */
#define SNTP_SERVER_DNS            1

/**
 * It disables a check of SNTP_UPDATE_DELAY it is done in sntp_set_sync_interval
 */
#define SNTP_SUPPRESS_DELAY_CHECK

#define SNTP_UPDATE_DELAY                 (sntp_get_sync_interval())
#define SNTP_SET_SYSTEM_TIME_US(sec, us)  (sntp_set_system_time(sec, us))
#define SNTP_GET_SYSTEM_TIME(sec, us)     (sntp_get_system_time(&(sec), &(us)))

/**
 * Configuring SNTP startup delay
 */
#ifdef CONFIG_LWIP_SNTP_STARTUP_DELAY
#define SNTP_STARTUP_DELAY 1
#ifdef CONFIG_LWIP_SNTP_MAXIMUM_STARTUP_DELAY
#define SNTP_STARTUP_DELAY_FUNC     (LWIP_RAND() % CONFIG_LWIP_SNTP_MAXIMUM_STARTUP_DELAY)
#endif /* CONFIG_LWIP_SNTP_MAXIMUM_STARTUP_DELAY */
#else
#define SNTP_STARTUP_DELAY 0
#endif /* SNTP_STARTUP_DELAY */

/*
   ---------------------------------------
   --------- ESP specific options --------
   ---------------------------------------
*/
/**
 * ESP_LWIP_IGMP_TIMERS_ONDEMAND==1: Start IGMP timers only if used
 * ESP_LWIP_MLD6_TIMERS_ONDEMAND==1: Start MLD6 timers only if used
 * Timers will only be activated when joining groups/receiving queries
 */
#ifdef CONFIG_LWIP_TIMERS_ONDEMAND
#define ESP_LWIP_IGMP_TIMERS_ONDEMAND   1
#define ESP_LWIP_MLD6_TIMERS_ONDEMAND   1
#define ESP_LWIP_DHCP_FINE_TIMERS_ONDEMAND 1
#define ESP_LWIP_DNS_TIMERS_ONDEMAND       1
#if IP_REASSEMBLY
#define ESP_LWIP_IP4_REASSEMBLY_TIMERS_ONDEMAND 1
#endif /* IP_REASSEMBLY */
#if LWIP_IPV6_REASS
#define ESP_LWIP_IP6_REASSEMBLY_TIMERS_ONDEMAND 1
#endif /* LWIP_IPV6_REASS */
#else
#define ESP_LWIP_IGMP_TIMERS_ONDEMAND   0
#define ESP_LWIP_MLD6_TIMERS_ONDEMAND   0
#define ESP_LWIP_DHCP_FINE_TIMERS_ONDEMAND 0
#define ESP_LWIP_DNS_TIMERS_ONDEMAND    0
#if IP_REASSEMBLY
#define ESP_LWIP_IP4_REASSEMBLY_TIMERS_ONDEMAND 0
#endif /* IP_REASSEMBLY */
#if LWIP_IPV6_REASS
#define ESP_LWIP_IP6_REASSEMBLY_TIMERS_ONDEMAND 0
#endif /* LWIP_IPV6_REASS */
#endif

/**
 * ESP_GRATUITOUS_ARP==1: This option allows to send gratuitous ARP periodically.
 */
#ifdef CONFIG_LWIP_ESP_GRATUITOUS_ARP
#define ESP_GRATUITOUS_ARP              1
#define ESP_GRATUITOUS_ARP_INTERVAL     (CONFIG_LWIP_GARP_TMR_INTERVAL*1000UL)
#else
#define ESP_GRATUITOUS_ARP              0
#endif

/**
 * ESP_MLDV6_REPORT==1: This option allows to send mldv6 report periodically.
 */
#ifdef CONFIG_LWIP_ESP_MLDV6_REPORT
#define ESP_MLDV6_REPORT              1
#else
#define ESP_MLDV6_REPORT              0
#endif

#define ESP_LWIP                        1
#define ESP_LWIP_ARP                    1
#define ESP_PER_SOC_TCP_WND             0
#define ESP_THREAD_SAFE                 1
#define ESP_THREAD_SAFE_DEBUG           LWIP_DBG_OFF
#define ESP_DHCP                        1
#define ESP_DNS                         1
#define ESP_STATS_TCP                   0
#define ESP_LWIP_LOGI(...)              ESP_LOGI("lwip", __VA_ARGS__)
#define ESP_PING                        1
#define ESP_HAS_SELECT                  1
#define ESP_IP4_ROUTE                   1
#define ESP_AUTO_IP                     1
#define ESP_PBUF                        1
#define ESP_PPP                         1
#define ESP_IPV6                        LWIP_IPV6
#define ESP_SOCKET                      1
#define ESP_LWIP_SELECT                 1
#define ESP_LWIP_LOCK                   1
#define ESP_THREAD_PROTECTION           1
#define LWIP_SUPPORT_CUSTOM_PBUF        1
#define ESP_LWIP_FALLBACK_DNS_PREFER_IPV4 0

/*
   -----------------------------------------
   ---------- DHCP Server options ----------
   -----------------------------------------
*/
/**
 * ESP_DHCPS==1: Enable the DHCP server
 */
#ifdef CONFIG_LWIP_DHCPS
#define ESP_DHCPS                       1
#define ESP_DHCPS_TIMER                 0
#else
#define ESP_DHCPS                       0
#define ESP_DHCPS_TIMER                 0
#endif /* CONFIG_LWIP_DHCPS */


#if LWIP_NETCONN_SEM_PER_THREAD
#if ESP_THREAD_SAFE
#define LWIP_NETCONN_THREAD_SEM_GET() sys_thread_sem_get()
#define LWIP_NETCONN_THREAD_SEM_ALLOC() sys_thread_sem_init()
#define LWIP_NETCONN_THREAD_SEM_FREE() sys_thread_sem_deinit()
#endif
#endif

/**
 * If CONFIG_ALLOC_MEMORY_IN_SPIRAM_FIRST is enabled, Try to
 * allocate memory for lwip in SPIRAM firstly. If failed, try to allocate
 * internal memory then.
 */
#if CONFIG_SPIRAM_TRY_ALLOCATE_WIFI_LWIP
#define mem_clib_malloc(size)    heap_caps_malloc_prefer(size, 2, MALLOC_CAP_DEFAULT|MALLOC_CAP_SPIRAM, MALLOC_CAP_DEFAULT|MALLOC_CAP_INTERNAL)
#define mem_clib_calloc(n, size) heap_caps_calloc_prefer(n, size, 2, MALLOC_CAP_DEFAULT|MALLOC_CAP_SPIRAM, MALLOC_CAP_DEFAULT|MALLOC_CAP_INTERNAL)
#else /* !CONFIG_SPIRAM_TRY_ALLOCATE_WIFI_LWIP */
#define mem_clib_malloc malloc
#define mem_clib_calloc calloc
#endif /* CONFIG_SPIRAM_TRY_ALLOCATE_WIFI_LWIP */


/*
 * Check if the lwIP configuration is sane
 */
#if !LWIP_IPV4 && !LWIP_IPV6
#error "Please enable at least one IP stack (either IPv4 or IPv6 or both)"
#endif

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_ESP_LWIPOPTS_H */
