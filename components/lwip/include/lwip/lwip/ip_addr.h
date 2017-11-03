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
#ifndef LWIP_HDR_IP_ADDR_H__
#define LWIP_HDR_IP_ADDR_H__

#include "lwip/opt.h"
#include "lwip/def.h"

#include "lwip/ip4_addr.h"
#include "lwip/ip6_addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/** These are the values for ip_addr_t.type */
#define IPADDR_TYPE_V4                0U
#define IPADDR_TYPE_V6                6U
#define IPADDR_TYPE_ANY               46U

#if LWIP_IPV4 && LWIP_IPV6
/** A union struct for both IP version's addresses.
 * ATTENTION: watch out for its size when adding IPv6 address scope!
 */
typedef struct _ip_addr {
  union {
    ip6_addr_t ip6;
    ip4_addr_t ip4;
  } u_addr;
  u8_t type;
} ip_addr_t;

extern const ip_addr_t ip_addr_any_type;

#define IPADDR4_INIT(u32val)          { { { { u32val, 0ul, 0ul, 0ul } } }, IPADDR_TYPE_V4 }
#define IPADDR6_INIT(a, b, c, d)      { { { { a, b, c, d } } }, IPADDR_TYPE_V6 }

#define IP_IS_ANY_TYPE_VAL(ipaddr)    (IP_GET_TYPE(&ipaddr) == IPADDR_TYPE_ANY)
#define IPADDR_ANY_TYPE_INIT          { { { { 0ul, 0ul, 0ul, 0ul } } }, IPADDR_TYPE_ANY }

#define IP_IS_V6_VAL(ipaddr)          (IP_GET_TYPE(&ipaddr) == IPADDR_TYPE_V6)
#define IP_IS_V6(ipaddr)              (((ipaddr) != NULL) && IP_IS_V6_VAL(*(ipaddr)))

#define IP_V6_EQ_PART(ipaddr, WORD, VAL) (ip_2_ip6(ipaddr)->addr[WORD] == htonl(VAL))
#define IP_IS_V4MAPPEDV6(ipaddr) (IP_IS_V6(ipaddr) && IP_V6_EQ_PART(ipaddr, 0, 0) && IP_V6_EQ_PART(ipaddr, 1, 0) && IP_V6_EQ_PART(ipaddr, 2, 0x0000FFFF))

#define IP_SET_TYPE_VAL(ipaddr, iptype) do { (ipaddr).type = (iptype); }while(0)
#define IP_SET_TYPE(ipaddr, iptype)     do { if((ipaddr) != NULL) { IP_SET_TYPE_VAL(*(ipaddr), iptype); }}while(0)
#define IP_GET_TYPE(ipaddr)           ((ipaddr)->type)

#define IP_ADDR_PCB_VERSION_MATCH_EXACT(pcb, ipaddr) (IP_GET_TYPE(&pcb->local_ip) == IP_GET_TYPE(ipaddr))
#define IP_ADDR_PCB_VERSION_MATCH(pcb, ipaddr) (IP_IS_ANY_TYPE_VAL(pcb->local_ip) || IP_ADDR_PCB_VERSION_MATCH_EXACT(pcb, ipaddr))

/* Convert generic ip address to specific protocol version */
#define ip_2_ip6(ipaddr)   (&((ipaddr)->u_addr.ip6))
#define ip_2_ip4(ipaddr)   (&((ipaddr)->u_addr.ip4))

#define IP_ADDR4(ipaddr,a,b,c,d)      do { IP4_ADDR(ip_2_ip4(ipaddr),a,b,c,d); \
                                           IP_SET_TYPE_VAL(*(ipaddr), IPADDR_TYPE_V4); } while(0)
#define IP_ADDR6(ipaddr,i0,i1,i2,i3)  do { IP6_ADDR(ip_2_ip6(ipaddr),i0,i1,i2,i3); \
                                           IP_SET_TYPE_VAL(*(ipaddr), IPADDR_TYPE_V6); } while(0)

#define ip_addr_copy(dest, src)      do{ IP_SET_TYPE_VAL(dest, IP_GET_TYPE(&src)); if(IP_IS_V6_VAL(src)){ \
  ip6_addr_copy(*ip_2_ip6(&(dest)), *ip_2_ip6(&(src))); }else{ \
  ip4_addr_copy(*ip_2_ip4(&(dest)), *ip_2_ip4(&(src))); }}while(0)
#define ip_addr_copy_from_ip6(dest, src)      do{ \
  ip6_addr_copy(*ip_2_ip6(&(dest)), src); IP_SET_TYPE_VAL(dest, IPADDR_TYPE_V6); }while(0)
#define ip_addr_copy_from_ip4(dest, src)      do{ \
  ip4_addr_copy(*ip_2_ip4(&(dest)), src); IP_SET_TYPE_VAL(dest, IPADDR_TYPE_V4); }while(0)
#define ip_addr_set_ip4_u32(ipaddr, val)  do{if(ipaddr){ip4_addr_set_u32(ip_2_ip4(ipaddr), val); \
  IP_SET_TYPE(ipaddr, IPADDR_TYPE_V4); }}while(0)
#define ip_addr_get_ip4_u32(ipaddr)  (((ipaddr) && !IP_IS_V6(ipaddr)) ? \
  ip4_addr_get_u32(ip_2_ip4(ipaddr)) : 0)

#define ip_addr_set(dest, src) do{ IP_SET_TYPE(dest, IP_GET_TYPE(src)); if(IP_IS_V6(src)){ \
  ip6_addr_set(ip_2_ip6(dest), ip_2_ip6(src)); }else{ \
  ip4_addr_set(ip_2_ip4(dest), ip_2_ip4(src)); }}while(0)
  
#define ip_addr_set_ipaddr(dest, src) ip_addr_set(dest, src)
#define ip_addr_set_zero(ipaddr)     do{ \
  ip6_addr_set_zero(ip_2_ip6(ipaddr)); IP_SET_TYPE(ipaddr, 0); }while(0)
#define ip_addr_set_zero_ip4(ipaddr)     do{ \
  ip6_addr_set_zero(ip_2_ip6(ipaddr)); IP_SET_TYPE(ipaddr, IPADDR_TYPE_V4); }while(0)
#define ip_addr_set_zero_ip6(ipaddr)     do{ \
  ip6_addr_set_zero(ip_2_ip6(ipaddr)); IP_SET_TYPE(ipaddr, IPADDR_TYPE_V6); }while(0)
#define ip_addr_set_any(is_ipv6, ipaddr)      do{if(is_ipv6){ \
  ip6_addr_set_any(ip_2_ip6(ipaddr)); IP_SET_TYPE(ipaddr, IPADDR_TYPE_V6); }else{ \
  ip4_addr_set_any(ip_2_ip4(ipaddr)); IP_SET_TYPE(ipaddr, IPADDR_TYPE_V4); }}while(0)
#define ip_addr_set_loopback(is_ipv6, ipaddr) do{if(is_ipv6){ \
  ip6_addr_set_loopback(ip_2_ip6(ipaddr)); IP_SET_TYPE(ipaddr, IPADDR_TYPE_V6); }else{ \
  ip4_addr_set_loopback(ip_2_ip4(ipaddr)); IP_SET_TYPE(ipaddr, IPADDR_TYPE_V4); }}while(0)
#define ip_addr_set_hton(dest, src)  do{if(IP_IS_V6(src)){ \
  ip6_addr_set_hton(ip_2_ip6(ipaddr), (src)); IP_SET_TYPE(dest, IPADDR_TYPE_V6); }else{ \
  ip4_addr_set_hton(ip_2_ip4(ipaddr), (src)); IP_SET_TYPE(dest, IPADDR_TYPE_V4); }}while(0)
#define ip_addr_get_network(target, host, netmask) do{if(IP_IS_V6(host)){ \
  ip4_addr_set_zero(ip_2_ip4(target)); IP_SET_TYPE(target, IPADDR_TYPE_V6); } else { \
  ip4_addr_get_network(ip_2_ip4(target), ip_2_ip4(host), ip_2_ip4(netmask)); IP_SET_TYPE(target, IPADDR_TYPE_V4); }}while(0)
#define ip_addr_netcmp(addr1, addr2, mask) ((IP_IS_V6(addr1) && IP_IS_V6(addr2)) ? \
  0 : \
  ip4_addr_netcmp(ip_2_ip4(addr1), ip_2_ip4(addr2), mask))
#define ip_addr_cmp(addr1, addr2)    ((IP_GET_TYPE(addr1) != IP_GET_TYPE(addr2)) ? 0 : (IP_IS_V6_VAL(*(addr1)) ? \
  ip6_addr_cmp(ip_2_ip6(addr1), ip_2_ip6(addr2)) : \
  ip4_addr_cmp(ip_2_ip4(addr1), ip_2_ip4(addr2))))
#define ip_addr_isany(ipaddr)        ((IP_IS_V6(ipaddr)) ? \
  ip6_addr_isany(ip_2_ip6(ipaddr)) : \
  ip4_addr_isany(ip_2_ip4(ipaddr)))
#define ip_addr_isany_val(ipaddr)        ((IP_IS_V6_VAL(ipaddr)) ? \
  ip6_addr_isany_val(*ip_2_ip6(&(ipaddr))) : \
  ip4_addr_isany_val(*ip_2_ip4(&(ipaddr))))
#define ip_addr_isbroadcast(ipaddr, netif) ((IP_IS_V6(ipaddr)) ? \
  0 : \
  ip4_addr_isbroadcast(ip_2_ip4(ipaddr), netif))
#define ip_addr_ismulticast(ipaddr)  ((IP_IS_V6(ipaddr)) ? \
  ip6_addr_ismulticast(ip_2_ip6(ipaddr)) : \
  ip4_addr_ismulticast(ip_2_ip4(ipaddr)))
#define ip_addr_isloopback(ipaddr)  ((IP_IS_V6(ipaddr)) ? \
  ip6_addr_isloopback(ip_2_ip6(ipaddr)) : \
  ip4_addr_isloopback(ip_2_ip4(ipaddr)))
#define ip_addr_islinklocal(ipaddr)  ((IP_IS_V6(ipaddr)) ? \
  ip6_addr_islinklocal(ip_2_ip6(ipaddr)) : \
  ip4_addr_islinklocal(ip_2_ip4(ipaddr)))
#define ip_addr_debug_print(debug, ipaddr) do { if(IP_IS_V6(ipaddr)) { \
  ip6_addr_debug_print(debug, ip_2_ip6(ipaddr)); } else { \
  ip4_addr_debug_print(debug, ip_2_ip4(ipaddr)); }}while(0)
#define ip_addr_debug_print_val(debug, ipaddr) do { if(IP_IS_V6_VAL(ipaddr)) { \
  ip6_addr_debug_print_val(debug, *ip_2_ip6(&(ipaddr))); } else { \
  ip4_addr_debug_print_val(debug, *ip_2_ip4(&(ipaddr))); }}while(0)
#define ipaddr_ntoa(addr)   (((addr) == NULL) ? "NULL" : \
  ((IP_IS_V6(addr)) ? ip6addr_ntoa(ip_2_ip6(addr)) : ip4addr_ntoa(ip_2_ip4(addr))))
#define ipaddr_ntoa_r(addr, buf, buflen)   (((addr) == NULL) ? "NULL" : \
  ((IP_IS_V6(addr)) ? ip6addr_ntoa_r(ip_2_ip6(addr), buf, buflen) : ip4addr_ntoa_r(ip_2_ip4(addr), buf, buflen)))
int ipaddr_aton(const char *cp, ip_addr_t *addr);

/* Map an IPv4 ip_addr into an IPV6 ip_addr, using format
   defined in RFC4291 2.5.5.2.

   Safe to call when dest==src.
*/
#define ip_addr_make_ip4_mapped_ip6(dest, src) do {       \
        u32_t tmp = ip_2_ip4(src)->addr;                    \
        IP_ADDR6((dest), 0x0, 0x0, htonl(0x0000FFFF), tmp); \
    } while(0)

/* Convert an IPv4 mapped V6 address to an IPV4 address.

   Check IP_IS_V4MAPPEDV6(src) before using this.

   Safe to call when dest == src.
*/
#define ip_addr_ip4_from_mapped_ip6(dest, src) do {     \
        ip_2_ip4(dest)->addr = ip_2_ip6(src)->addr[3];  \
        IP_SET_TYPE(dest, IPADDR_TYPE_V4);         \
    } while(0)

#else /* LWIP_IPV4 && LWIP_IPV6 */

#define IP_ADDR_PCB_VERSION_MATCH(addr, pcb)         1
#define IP_ADDR_PCB_VERSION_MATCH_EXACT(pcb, ipaddr) 1

#if LWIP_IPV4

typedef ip4_addr_t ip_addr_t;
#define IPADDR4_INIT(u32val)                    { u32val }
#define IP_IS_V6_VAL(ipaddr)                    0
#define IP_IS_V6(ipaddr)                        0
#define IP_IS_ANY_TYPE_VAL(ipaddr)              0
#define IP_SET_TYPE_VAL(ipaddr, iptype)
#define IP_SET_TYPE(ipaddr, iptype)
#define IP_GET_TYPE(ipaddr)                     IPADDR_TYPE_V4
#define ip_2_ip4(ipaddr)                        (ipaddr)
#define IP_ADDR4(ipaddr,a,b,c,d)                IP4_ADDR(ipaddr,a,b,c,d)

#define ip_addr_copy(dest, src)                 ip4_addr_copy(dest, src)
#define ip_addr_copy_from_ip4(dest, src)        ip4_addr_copy(dest, src)
#define ip_addr_set_ip4_u32(ipaddr, val)        ip4_addr_set_u32(ip_2_ip4(ipaddr), val)
#define ip_addr_get_ip4_u32(ipaddr)             ip4_addr_get_u32(ip_2_ip4(ipaddr))
#define ip_addr_set(dest, src)                  ip4_addr_set(dest, src)
#define ip_addr_set_ipaddr(dest, src)           ip4_addr_set(dest, src)
#define ip_addr_set_zero(ipaddr)                ip4_addr_set_zero(ipaddr)
#define ip_addr_set_zero_ip4(ipaddr)            ip4_addr_set_zero(ipaddr)
#define ip_addr_set_any(is_ipv6, ipaddr)        ip4_addr_set_any(ipaddr)
#define ip_addr_set_loopback(is_ipv6, ipaddr)   ip4_addr_set_loopback(ipaddr)
#define ip_addr_set_hton(dest, src)             ip4_addr_set_hton(dest, src)
#define ip_addr_get_network(target, host, mask) ip4_addr_get_network(target, host, mask)
#define ip_addr_netcmp(addr1, addr2, mask)      ip4_addr_netcmp(addr1, addr2, mask)
#define ip_addr_cmp(addr1, addr2)               ip4_addr_cmp(addr1, addr2)
#define ip_addr_isany(ipaddr)                   ip4_addr_isany(ipaddr)
#define ip_addr_isany_val(ipaddr)               ip4_addr_isany_val(ipaddr)
#define ip_addr_isloopback(ipaddr)              ip4_addr_isloopback(ipaddr)
#define ip_addr_islinklocal(ipaddr)             ip4_addr_islinklocal(ipaddr)
#define ip_addr_isbroadcast(addr, netif)        ip4_addr_isbroadcast(addr, netif)
#define ip_addr_ismulticast(ipaddr)             ip4_addr_ismulticast(ipaddr)
#define ip_addr_debug_print(debug, ipaddr)      ip4_addr_debug_print(debug, ipaddr)
#define ip_addr_debug_print_val(debug, ipaddr)  ip4_addr_debug_print_val(debug, ipaddr)
#define ipaddr_ntoa(ipaddr)                     ip4addr_ntoa(ipaddr)
#define ipaddr_ntoa_r(ipaddr, buf, buflen)      ip4addr_ntoa_r(ipaddr, buf, buflen)
#define ipaddr_aton(cp, addr)                   ip4addr_aton(cp, addr)

#else /* LWIP_IPV4 */

typedef ip6_addr_t ip_addr_t;
#define IPADDR6_INIT(a, b, c, d)                { { a, b, c, d } }
#define IP_IS_V6_VAL(ipaddr)                    1
#define IP_IS_V6(ipaddr)                        1
#define IP_IS_ANY_TYPE_VAL(ipaddr)              0
#define IP_SET_TYPE_VAL(ipaddr, iptype)
#define IP_SET_TYPE(ipaddr, iptype)
#define IP_GET_TYPE(ipaddr)                     IPADDR_TYPE_V6
#define ip_2_ip6(ipaddr)                        (ipaddr)
#define IP_ADDR6(ipaddr,i0,i1,i2,i3)            IP6_ADDR(ipaddr,i0,i1,i2,i3)

#define ip_addr_copy(dest, src)                 ip6_addr_copy(dest, src)
#define ip_addr_copy_from_ip6(dest, src)        ip6_addr_copy(dest, src)
#define ip_addr_set(dest, src)                  ip6_addr_set(dest, src)
#define ip_addr_set_ipaddr(dest, src)           ip6_addr_set(dest, src)
#define ip_addr_set_zero(ipaddr)                ip6_addr_set_zero(ipaddr)
#define ip_addr_set_zero_ip6(ipaddr)            ip6_addr_set_zero(ipaddr)
#define ip_addr_set_any(is_ipv6, ipaddr)        ip6_addr_set_any(ipaddr)
#define ip_addr_set_loopback(is_ipv6, ipaddr)   ip6_addr_set_loopback(ipaddr)
#define ip_addr_set_hton(dest, src)             ip6_addr_set_hton(dest, src)
#define ip_addr_get_network(target, host, mask) ip6_addr_set_zero(target)
#define ip_addr_netcmp(addr1, addr2, mask)      0
#define ip_addr_cmp(addr1, addr2)               ip6_addr_cmp(addr1, addr2)
#define ip_addr_isany(ipaddr)                   ip6_addr_isany(ipaddr)
#define ip_addr_isany_val(ipaddr)               ip6_addr_isany_val(ipaddr)
#define ip_addr_isloopback(ipaddr)              ip6_addr_isloopback(ipaddr)
#define ip_addr_islinklocal(ipaddr)             ip6_addr_islinklocal(ipaddr)
#define ip_addr_isbroadcast(addr, netif)        0
#define ip_addr_ismulticast(ipaddr)             ip6_addr_ismulticast(ipaddr)
#define ip_addr_debug_print(debug, ipaddr)      ip6_addr_debug_print(debug, ipaddr)
#define ip_addr_debug_print_val(debug, ipaddr)  ip6_addr_debug_print_val(debug, ipaddr)
#define ipaddr_ntoa(ipaddr)                     ip6addr_ntoa(ipaddr)
#define ipaddr_ntoa_r(ipaddr, buf, buflen)      ip6addr_ntoa_r(ipaddr, buf, buflen)
#define ipaddr_aton(cp, addr)                   ip6addr_aton(cp, addr)

#endif /* LWIP_IPV4 */
#endif /* LWIP_IPV4 && LWIP_IPV6 */

#if LWIP_IPV4

extern const ip_addr_t ip_addr_any;
extern const ip_addr_t ip_addr_broadcast;

/** IP_ADDR_ can be used as a fixed/const ip_addr_t
 *  for the IPv4 wildcard and the broadcast address
 */
#define IP_ADDR_ANY         (&ip_addr_any)
#define IP_ADDR_BROADCAST   (&ip_addr_broadcast)
/** IP4_ADDR_ can be used as a fixed/const ip4_addr_t
 *  for the wildcard and the broadcast address
 */
#define IP4_ADDR_ANY        (ip_2_ip4(&ip_addr_any))
#define IP4_ADDR_BROADCAST  (ip_2_ip4(&ip_addr_broadcast))

#endif /* LWIP_IPV4*/

#if LWIP_IPV6

extern const ip_addr_t ip6_addr_any;

/** IP6_ADDR_ANY can be used as a fixed ip_addr_t
 *  for the IPv6 wildcard address
 */
#define IP6_ADDR_ANY   (&ip6_addr_any)
/** IP6_ADDR_ANY6 can be used as a fixed ip6_addr_t
 *  for the IPv6 wildcard address
 */
#define IP6_ADDR_ANY6  (ip_2_ip6(&ip6_addr_any))

#if !LWIP_IPV4
/** Just a little upgrade-helper for IPv6-only configurations: */
#define IP_ADDR_ANY IP6_ADDR_ANY
#endif /* !LWIP_IPV4 */

#endif

#if LWIP_IPV4 && LWIP_IPV6
#define IP_ANY_TYPE    (&ip_addr_any_type)
#else
#define IP_ANY_TYPE    IP_ADDR_ANY
#endif

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_IP_ADDR_H__ */
