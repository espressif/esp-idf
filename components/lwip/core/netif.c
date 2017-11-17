/**
 * @file
 * lwIP network interface abstraction
 *
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

#include "lwip/opt.h"

#include "lwip/def.h"
#include "lwip/ip_addr.h"
#include "lwip/ip6_addr.h"
#include "lwip/netif.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/udp.h"
#include "lwip/snmp.h"
#include "lwip/igmp.h"
#include "netif/etharp.h"
#include "lwip/stats.h"
#include "lwip/sys.h"
#if ENABLE_LOOPBACK
#include "lwip/sys.h"
#if LWIP_NETIF_LOOPBACK_MULTITHREADING
#include "lwip/tcpip.h"
#endif /* LWIP_NETIF_LOOPBACK_MULTITHREADING */
#endif /* ENABLE_LOOPBACK */

#if LWIP_AUTOIP
#include "lwip/autoip.h"
#endif /* LWIP_AUTOIP */
#if LWIP_DHCP
#include "lwip/dhcp.h"
#endif /* LWIP_DHCP */
#if LWIP_IPV6_DHCP6
#include "lwip/dhcp6.h"
#endif /* LWIP_IPV6_DHCP6 */
#if LWIP_IPV6_MLD
#include "lwip/mld6.h"
#endif /* LWIP_IPV6_MLD */

#if LWIP_NETIF_STATUS_CALLBACK
#define NETIF_STATUS_CALLBACK(n) do{ if (n->status_callback) { (n->status_callback)(n); }}while(0)
#else
#define NETIF_STATUS_CALLBACK(n)
#endif /* LWIP_NETIF_STATUS_CALLBACK */

#if LWIP_NETIF_LINK_CALLBACK
#define NETIF_LINK_CALLBACK(n) do{ if (n->link_callback) { (n->link_callback)(n); }}while(0)
#else
#define NETIF_LINK_CALLBACK(n)
#endif /* LWIP_NETIF_LINK_CALLBACK */

struct netif *netif_list;
struct netif *netif_default;

static u8_t netif_num;

#define NETIF_REPORT_TYPE_IPV4  0x01
#define NETIF_REPORT_TYPE_IPV6  0x02
static void netif_issue_reports(struct netif* netif, u8_t report_type);

#if LWIP_IPV6
static err_t netif_null_output_ip6(struct netif *netif, struct pbuf *p, const ip6_addr_t *ipaddr);
#endif /* LWIP_IPV6 */

#if LWIP_HAVE_LOOPIF
#if LWIP_IPV4
static err_t netif_loop_output_ipv4(struct netif *netif, struct pbuf *p, const ip4_addr_t* addr);
#endif
#if LWIP_IPV6
static err_t netif_loop_output_ipv6(struct netif *netif, struct pbuf *p, const ip6_addr_t* addr);
#endif


static struct netif loop_netif;

/**
 * Initialize a lwip network interface structure for a loopback interface
 *
 * @param netif the lwip network interface structure for this loopif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 */
static err_t
netif_loopif_init(struct netif *netif)
{
  /* initialize the snmp variables and counters inside the struct netif
   * ifSpeed: no assumption can be made!
   */
  MIB2_INIT_NETIF(netif, snmp_ifType_softwareLoopback, 0);

  netif->name[0] = 'l';
  netif->name[1] = 'o';
#if LWIP_IPV4
  netif->output = netif_loop_output_ipv4;
#endif
#if LWIP_IPV6
  netif->output_ip6 = netif_loop_output_ipv6;
#endif
#if LWIP_LOOPIF_MULTICAST
  netif->flags |= NETIF_FLAG_IGMP;
#endif
  return ERR_OK;
}
#endif /* LWIP_HAVE_LOOPIF */

void
netif_init(void)
{
#if LWIP_HAVE_LOOPIF
#if LWIP_IPV4
#define LOOPIF_ADDRINIT &loop_ipaddr, &loop_netmask, &loop_gw,
  ip4_addr_t loop_ipaddr, loop_netmask, loop_gw;
  IP4_ADDR(&loop_gw, 127,0,0,1);
  IP4_ADDR(&loop_ipaddr, 127,0,0,1);
  IP4_ADDR(&loop_netmask, 255,0,0,0);
#else /* LWIP_IPV4 */
#define LOOPIF_ADDRINIT
#endif /* LWIP_IPV4 */

#if NO_SYS
  netif_add(&loop_netif, LOOPIF_ADDRINIT NULL, netif_loopif_init, ip_input);
#else  /* NO_SYS */
  netif_add(&loop_netif, LOOPIF_ADDRINIT NULL, netif_loopif_init, tcpip_input);
#endif /* NO_SYS */

#if LWIP_IPV6
  IP_ADDR6(loop_netif.ip6_addr, 0, 0, 0, PP_HTONL(0x00000001UL));
  loop_netif.ip6_addr_state[0] = IP6_ADDR_VALID;
#endif /* LWIP_IPV6 */

  netif_set_link_up(&loop_netif);
  netif_set_up(&loop_netif);

#endif /* LWIP_HAVE_LOOPIF */
}

/**
 * Add a network interface to the list of lwIP netifs.
 *
 * @param netif a pre-allocated netif structure
 * @param ipaddr IP address for the new netif
 * @param netmask network mask for the new netif
 * @param gw default gateway IP address for the new netif
 * @param state opaque data passed to the new netif
 * @param init callback function that initializes the interface
 * @param input callback function that is called to pass
 * ingress packets up in the protocol layer stack.
 *
 * @return netif, or NULL if failed.
 */
struct netif *
netif_add(struct netif *netif,
#if LWIP_IPV4
          const ip4_addr_t *ipaddr, const ip4_addr_t *netmask, const ip4_addr_t *gw,
#endif /* LWIP_IPV4 */
          void *state, netif_init_fn init, netif_input_fn input)
{
#if LWIP_IPV6
  u32_t i;
#endif

  LWIP_ASSERT("No init function given", init != NULL);

  /* reset new interface configuration state */
#if LWIP_IPV4
  ip_addr_set_zero_ip4(&netif->ip_addr);
  ip_addr_set_zero_ip4(&netif->netmask);
  ip_addr_set_zero_ip4(&netif->gw);
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
  for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
    ip_addr_set_zero_ip6(&netif->ip6_addr[i]);
    netif_ip6_addr_set_state(netif, i, IP6_ADDR_INVALID);
  }
  netif->output_ip6 = netif_null_output_ip6;
#endif /* LWIP_IPV6 */


  NETIF_SET_CHECKSUM_CTRL(netif, NETIF_CHECKSUM_ENABLE_ALL);
  netif->flags = 0;
  
#if LWIP_DHCP
  /* netif not under DHCP control by default */
  netif->dhcp = NULL;

#if ESP_DHCP
  netif->dhcps_pcb = NULL;
#endif

#endif /* LWIP_DHCP */


#if LWIP_AUTOIP
  /* netif not under AutoIP control by default */
  netif->autoip = NULL;
#endif /* LWIP_AUTOIP */
#if LWIP_IPV6_AUTOCONFIG

#if ESP_IPV6_AUTOCONFIG
  netif->ip6_autoconfig_enabled = 1;
#else
  /* IPv6 address autoconfiguration not enabled by default */
  netif->ip6_autoconfig_enabled = 0;
#endif

#endif /* LWIP_IPV6_AUTOCONFIG */


#if LWIP_IPV6_SEND_ROUTER_SOLICIT
  netif->rs_count = LWIP_ND6_MAX_MULTICAST_SOLICIT;
#endif /* LWIP_IPV6_SEND_ROUTER_SOLICIT */
#if LWIP_IPV6_DHCP6
  /* netif not under DHCPv6 control by default */
  netif->dhcp6 = NULL;
#endif /* LWIP_IPV6_DHCP6 */
#if LWIP_NETIF_STATUS_CALLBACK
  netif->status_callback = NULL;
#endif /* LWIP_NETIF_STATUS_CALLBACK */
#if LWIP_NETIF_LINK_CALLBACK
  netif->link_callback = NULL;
#endif /* LWIP_NETIF_LINK_CALLBACK */
#if LWIP_IGMP
  netif->igmp_mac_filter = NULL;
#endif /* LWIP_IGMP */
#if LWIP_IPV6 && LWIP_IPV6_MLD
  netif->mld_mac_filter = NULL;
#endif /* LWIP_IPV6 && LWIP_IPV6_MLD */
#if ENABLE_LOOPBACK
  netif->loop_first = NULL;
  netif->loop_last = NULL;
#endif /* ENABLE_LOOPBACK */


  /* remember netif specific state information data */
  netif->state = state;
  netif->num = netif_num++;
  netif->input = input;
  NETIF_SET_HWADDRHINT(netif, NULL);
#if ENABLE_LOOPBACK && LWIP_LOOPBACK_MAX_PBUFS
  netif->loop_cnt_current = 0;
#endif /* ENABLE_LOOPBACK && LWIP_LOOPBACK_MAX_PBUFS */

#if LWIP_IPV4
  netif_set_addr(netif, ipaddr, netmask, gw);
#endif /* LWIP_IPV4 */


  /* call user specified initialization function for netif */
  if (init(netif) != ERR_OK) {
    return NULL;
  }

  /* add this netif to the list */
  netif->next = netif_list;
  netif_list = netif;
  mib2_netif_added(netif);

#if LWIP_IGMP
  /* start IGMP processing */
  if (netif->flags & NETIF_FLAG_IGMP) {
    igmp_start(netif);
  }
#endif /* LWIP_IGMP */

  LWIP_DEBUGF(NETIF_DEBUG, ("netif: added interface %c%c IP",
    netif->name[0], netif->name[1]));
#if LWIP_IPV4
  LWIP_DEBUGF(NETIF_DEBUG, (" addr "));
  ip4_addr_debug_print(NETIF_DEBUG, ipaddr);
  LWIP_DEBUGF(NETIF_DEBUG, (" netmask "));
  ip4_addr_debug_print(NETIF_DEBUG, netmask);
  LWIP_DEBUGF(NETIF_DEBUG, (" gw "));
  ip4_addr_debug_print(NETIF_DEBUG, gw);
#endif /* LWIP_IPV4 */
  LWIP_DEBUGF(NETIF_DEBUG, ("\n"));

  return netif;
}

#if LWIP_IPV4
/**
 * Change IP address configuration for a network interface (including netmask
 * and default gateway).
 *
 * @param netif the network interface to change
 * @param ipaddr the new IP address
 * @param netmask the new netmask
 * @param gw the new default gateway
 */
void
netif_set_addr(struct netif *netif, const ip4_addr_t *ipaddr, const ip4_addr_t *netmask,
    const ip4_addr_t *gw)
{
  netif_set_netmask(netif, netmask);
  netif_set_gw(netif, gw);
  /* set ipaddr last to ensure netmask/gw have been set when status callback is called */
  netif_set_ipaddr(netif, ipaddr);
}

#endif /* LWIP_IPV4*/

/**
 * Remove a network interface from the list of lwIP netifs.
 *
 * @param netif the network interface to remove
 */
void
netif_remove(struct netif *netif)
{
  if (netif == NULL) {
    return;
  }

#if LWIP_IPV4
  if (!ip4_addr_isany_val(*netif_ip4_addr(netif))) {
#if LWIP_TCP
    tcp_netif_ipv4_addr_changed(netif_ip4_addr(netif), NULL);
#endif /* LWIP_TCP */
    /* cannot do this for UDP, as there is no 'err' callback in udp pcbs */
  }

#if LWIP_IGMP
  /* stop IGMP processing */
  if (netif->flags & NETIF_FLAG_IGMP) {
    igmp_stop(netif);
  }
#endif /* LWIP_IGMP */
#endif /* LWIP_IPV4*/

#if LWIP_IPV6 && LWIP_IPV6_MLD
  /* stop MLD processing */
  mld6_stop(netif);
#endif /* LWIP_IPV6 && LWIP_IPV6_MLD */
  if (netif_is_up(netif)) {
    /* set netif down before removing (call callback function) */
    netif_set_down(netif);
  }

#if LWIP_DHCP
  /* netif not under DHCP control by default */
  if (netif->dhcp) {
      free(netif->dhcp);
      netif->dhcp = NULL;
  }
#endif /* LWIP_DHCP */

  mib2_remove_ip4(netif);

  /* this netif is default? */
  if (netif_default == netif) {
    /* reset default netif */
    netif_set_default(NULL);
  }
  /*  is it the first netif? */
  if (netif_list == netif) {
    netif_list = netif->next;
  } else {
    /*  look for netif further down the list */
    struct netif * tmp_netif;
    for (tmp_netif = netif_list; tmp_netif != NULL; tmp_netif = tmp_netif->next) {
      if (tmp_netif->next == netif) {
        tmp_netif->next = netif->next;
        break;
      }
    }
    if (tmp_netif == NULL) {
      return; /* netif is not on the list */
    }
  }
  mib2_netif_removed(netif);
#if LWIP_NETIF_REMOVE_CALLBACK
  if (netif->remove_callback) {
    netif->remove_callback(netif);
  }
#endif /* LWIP_NETIF_REMOVE_CALLBACK */
  LWIP_DEBUGF( NETIF_DEBUG, ("netif_remove: removed netif\n") );
}

/**
 * Find a network interface by searching for its name
 *
 * @param name the name of the netif (like netif->name) plus concatenated number
 * in ascii representation (e.g. 'en0')
 */
struct netif *
netif_find(const char *name)
{
  struct netif *netif;
  u8_t num;

  if (name == NULL) {
    return NULL;
  }

  num = name[2] - '0';

  for (netif = netif_list; netif != NULL; netif = netif->next) {
    if (num == netif->num &&
       name[0] == netif->name[0] &&
       name[1] == netif->name[1]) {
      LWIP_DEBUGF(NETIF_DEBUG, ("netif_find: found %c%c\n", name[0], name[1]));
      return netif;
    }
  }
  LWIP_DEBUGF(NETIF_DEBUG, ("netif_find: didn't find %c%c\n", name[0], name[1]));
  return NULL;
}

#if LWIP_IPV4
/**
 * Change the IP address of a network interface
 *
 * @param netif the network interface to change
 * @param ipaddr the new IP address
 *
 * @note call netif_set_addr() if you also want to change netmask and
 * default gateway
 */
void
netif_set_ipaddr(struct netif *netif, const ip4_addr_t *ipaddr)
{
  ip4_addr_t new_addr = (ipaddr ? *ipaddr : *IP4_ADDR_ANY);
#if ESP_LWIP
  ip4_addr_t *last_addr = ip_2_ip4(&netif->last_ip_addr);
#else
  ip4_addr_t *last_addr = netif_ip4_addr(netif);
#endif

  /* address is actually being changed? */
  if (ip4_addr_cmp(&new_addr, netif_ip4_addr(netif)) == 0) {
    LWIP_DEBUGF(NETIF_DEBUG | LWIP_DBG_STATE, ("netif_set_ipaddr: netif address being changed\n"));
#if LWIP_TCP
    tcp_netif_ipv4_addr_changed(last_addr, ipaddr);
#endif /* LWIP_TCP */
#if LWIP_UDP
    udp_netif_ipv4_addr_changed(last_addr, ipaddr);
#endif /* LWIP_UDP */

    mib2_remove_ip4(netif);
    mib2_remove_route_ip4(0, netif);
    /* set new IP address to netif */
    ip4_addr_set(ip_2_ip4(&netif->ip_addr), ipaddr);
    IP_SET_TYPE_VAL(netif->ip_addr, IPADDR_TYPE_V4);
    mib2_add_ip4(netif);
    mib2_add_route_ip4(0, netif);

    netif_issue_reports(netif, NETIF_REPORT_TYPE_IPV4);

    NETIF_STATUS_CALLBACK(netif);
  }

  LWIP_DEBUGF(NETIF_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("netif: IP address of interface %c%c set to %"U16_F".%"U16_F".%"U16_F".%"U16_F"\n",
    netif->name[0], netif->name[1],
    ip4_addr1_16(netif_ip4_addr(netif)),
    ip4_addr2_16(netif_ip4_addr(netif)),
    ip4_addr3_16(netif_ip4_addr(netif)),
    ip4_addr4_16(netif_ip4_addr(netif))));

  if (ipaddr && !ip4_addr_isany(ipaddr)) {
    ip4_addr_set(ip_2_ip4(&netif->last_ip_addr), ipaddr);
  }
}

/**
 * Change the default gateway for a network interface
 *
 * @param netif the network interface to change
 * @param gw the new default gateway
 *
 * @note call netif_set_addr() if you also want to change ip address and netmask
 */
void
netif_set_gw(struct netif *netif, const ip4_addr_t *gw)
{
  ip4_addr_set(ip_2_ip4(&netif->gw), gw);
  IP_SET_TYPE_VAL(netif->gw, IPADDR_TYPE_V4);
  LWIP_DEBUGF(NETIF_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("netif: GW address of interface %c%c set to %"U16_F".%"U16_F".%"U16_F".%"U16_F"\n",
    netif->name[0], netif->name[1],
    ip4_addr1_16(netif_ip4_gw(netif)),
    ip4_addr2_16(netif_ip4_gw(netif)),
    ip4_addr3_16(netif_ip4_gw(netif)),
    ip4_addr4_16(netif_ip4_gw(netif))));
}

/**
 * Change the netmask of a network interface
 *
 * @param netif the network interface to change
 * @param netmask the new netmask
 *
 * @note call netif_set_addr() if you also want to change ip address and
 * default gateway
 */
void
netif_set_netmask(struct netif *netif, const ip4_addr_t *netmask)
{
  mib2_remove_route_ip4(0, netif);
  /* set new netmask to netif */
  ip4_addr_set(ip_2_ip4(&netif->netmask), netmask);
  IP_SET_TYPE_VAL(netif->netmask, IPADDR_TYPE_V4);
  mib2_add_route_ip4(0, netif);
  LWIP_DEBUGF(NETIF_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("netif: netmask of interface %c%c set to %"U16_F".%"U16_F".%"U16_F".%"U16_F"\n",
    netif->name[0], netif->name[1],
    ip4_addr1_16(netif_ip4_netmask(netif)),
    ip4_addr2_16(netif_ip4_netmask(netif)),
    ip4_addr3_16(netif_ip4_netmask(netif)),
    ip4_addr4_16(netif_ip4_netmask(netif))));
}
#endif /* LWIP_IPV4 */

/**
 * Set a network interface as the default network interface
 * (used to output all packets for which no specific route is found)
 *
 * @param netif the default network interface
 */
void
netif_set_default(struct netif *netif)
{
  if (netif == NULL) {
    /* remove default route */
    mib2_remove_route_ip4(1, netif);
  } else {
    /* install default route */
    mib2_add_route_ip4(1, netif);
  }
  netif_default = netif;
  LWIP_DEBUGF(NETIF_DEBUG, ("netif: setting default interface %c%c\n",
           netif ? netif->name[0] : '\'', netif ? netif->name[1] : '\''));
}

/**
 * Bring an interface up, available for processing
 * traffic.
 */
void
netif_set_up(struct netif *netif)
{
  if (!(netif->flags & NETIF_FLAG_UP)) {
    netif->flags |= NETIF_FLAG_UP;

    MIB2_COPY_SYSUPTIME_TO(&netif->ts);

    NETIF_STATUS_CALLBACK(netif);

    if (netif->flags & NETIF_FLAG_LINK_UP) {
      netif_issue_reports(netif, NETIF_REPORT_TYPE_IPV4|NETIF_REPORT_TYPE_IPV6);
    }
  }
}

/** Send ARP/IGMP/MLD/RS events, e.g. on link-up/netif-up or addr-change
 */
static void
netif_issue_reports(struct netif* netif, u8_t report_type)
{
#if LWIP_IPV4
  if ((report_type & NETIF_REPORT_TYPE_IPV4) &&
      !ip4_addr_isany_val(*netif_ip4_addr(netif))) {
#if LWIP_ARP
    /* For Ethernet network interfaces, we would like to send a "gratuitous ARP" */
    if (netif->flags & (NETIF_FLAG_ETHARP)) {
      etharp_gratuitous(netif);
    }
#endif /* LWIP_ARP */

#if LWIP_IGMP
    /* resend IGMP memberships */
    if (netif->flags & NETIF_FLAG_IGMP) {
      igmp_report_groups(netif);
    }
#endif /* LWIP_IGMP */
  }
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
  if (report_type & NETIF_REPORT_TYPE_IPV6) {
#if LWIP_IPV6_MLD
    /* send mld memberships */
    mld6_report_groups(netif);
#endif /* LWIP_IPV6_MLD */
#if LWIP_IPV6_SEND_ROUTER_SOLICIT
    /* Send Router Solicitation messages. */
    netif->rs_count = LWIP_ND6_MAX_MULTICAST_SOLICIT;
#endif /* LWIP_IPV6_SEND_ROUTER_SOLICIT */
  }
#endif /* LWIP_IPV6 */
}

/**
 * Bring an interface down, disabling any traffic processing.
 */
void
netif_set_down(struct netif *netif)
{
  if (netif->flags & NETIF_FLAG_UP) {
    netif->flags &= ~NETIF_FLAG_UP;
    MIB2_COPY_SYSUPTIME_TO(&netif->ts);

#if LWIP_IPV4 && LWIP_ARP
    if (netif->flags & NETIF_FLAG_ETHARP) {
      etharp_cleanup_netif(netif);
    }
#endif /* LWIP_IPV4 && LWIP_ARP */
    NETIF_STATUS_CALLBACK(netif);
  }
}

#if LWIP_NETIF_STATUS_CALLBACK
/**
 * Set callback to be called when interface is brought up/down or address is changed while up
 */
void
netif_set_status_callback(struct netif *netif, netif_status_callback_fn status_callback)
{
  if (netif) {
    netif->status_callback = status_callback;
  }
}
#endif /* LWIP_NETIF_STATUS_CALLBACK */

#if LWIP_NETIF_REMOVE_CALLBACK
/**
 * Set callback to be called when the interface has been removed
 */
void
netif_set_remove_callback(struct netif *netif, netif_status_callback_fn remove_callback)
{
  if (netif) {
    netif->remove_callback = remove_callback;
  }
}
#endif /* LWIP_NETIF_REMOVE_CALLBACK */

/**
 * Called by a driver when its link goes up
 */
void
netif_set_link_up(struct netif *netif)
{
  if (!(netif->flags & NETIF_FLAG_LINK_UP)) {
    netif->flags |= NETIF_FLAG_LINK_UP;

#if LWIP_DHCP
    if (netif->dhcp) {
      dhcp_network_changed(netif);
    }
#endif /* LWIP_DHCP */

#if LWIP_AUTOIP
    if (netif->autoip) {
      autoip_network_changed(netif);
    }
#endif /* LWIP_AUTOIP */

    if (netif->flags & NETIF_FLAG_UP) {
      netif_issue_reports(netif, NETIF_REPORT_TYPE_IPV4|NETIF_REPORT_TYPE_IPV6);
    }
    NETIF_LINK_CALLBACK(netif);
  }
}

/**
 * Called by a driver when its link goes down
 */
void
netif_set_link_down(struct netif *netif )
{
  if (netif->flags & NETIF_FLAG_LINK_UP) {
    netif->flags &= ~NETIF_FLAG_LINK_UP;
    NETIF_LINK_CALLBACK(netif);
  }
}

#if LWIP_NETIF_LINK_CALLBACK
/**
 * Set callback to be called when link is brought up/down
 */
void
netif_set_link_callback(struct netif *netif, netif_status_callback_fn link_callback)
{
  if (netif) {
    netif->link_callback = link_callback;
  }
}
#endif /* LWIP_NETIF_LINK_CALLBACK */

#if ENABLE_LOOPBACK
/**
 * Send an IP packet to be received on the same netif (loopif-like).
 * The pbuf is simply copied and handed back to netif->input.
 * In multithreaded mode, this is done directly since netif->input must put
 * the packet on a queue.
 * In callback mode, the packet is put on an internal queue and is fed to
 * netif->input by netif_poll().
 *
 * @param netif the lwip network interface structure
 * @param p the (IP) packet to 'send'
 * @return ERR_OK if the packet has been sent
 *         ERR_MEM if the pbuf used to copy the packet couldn't be allocated
 */
err_t
netif_loop_output(struct netif *netif, struct pbuf *p)
{
  struct pbuf *r;
  err_t err;
  struct pbuf *last;
#if LWIP_LOOPBACK_MAX_PBUFS
  u8_t clen = 0;
#endif /* LWIP_LOOPBACK_MAX_PBUFS */
  /* If we have a loopif, SNMP counters are adjusted for it,
   * if not they are adjusted for 'netif'. */
#if MIB2_STATS
#if LWIP_HAVE_LOOPIF
  struct netif *stats_if = &loop_netif;
#else /* LWIP_HAVE_LOOPIF */
  struct netif *stats_if = netif;
#endif /* LWIP_HAVE_LOOPIF */
#endif /* MIB2_STATS */
  SYS_ARCH_DECL_PROTECT(lev);

  /* Allocate a new pbuf */
  r = pbuf_alloc(PBUF_LINK, p->tot_len, PBUF_RAM);
  if (r == NULL) {
    LINK_STATS_INC(link.memerr);
    LINK_STATS_INC(link.drop);
    MIB2_STATS_NETIF_INC(stats_if, ifoutdiscards);
    return ERR_MEM;
  }
#if LWIP_LOOPBACK_MAX_PBUFS
  clen = pbuf_clen(r);
  /* check for overflow or too many pbuf on queue */
  if (((netif->loop_cnt_current + clen) < netif->loop_cnt_current) ||
     ((netif->loop_cnt_current + clen) > LWIP_LOOPBACK_MAX_PBUFS)) {
    pbuf_free(r);
    LINK_STATS_INC(link.memerr);
    LINK_STATS_INC(link.drop);
    MIB2_STATS_NETIF_INC(stats_if, ifoutdiscards);
    return ERR_MEM;
  }
  netif->loop_cnt_current += clen;
#endif /* LWIP_LOOPBACK_MAX_PBUFS */

  /* Copy the whole pbuf queue p into the single pbuf r */
  if ((err = pbuf_copy(r, p)) != ERR_OK) {
    pbuf_free(r);
    LINK_STATS_INC(link.memerr);
    LINK_STATS_INC(link.drop);
    MIB2_STATS_NETIF_INC(stats_if, ifoutdiscards);
    return err;
  }

  /* Put the packet on a linked list which gets emptied through calling
     netif_poll(). */

  /* let last point to the last pbuf in chain r */
  for (last = r; last->next != NULL; last = last->next);

  SYS_ARCH_PROTECT(lev);
  if (netif->loop_first != NULL) {
    LWIP_ASSERT("if first != NULL, last must also be != NULL", netif->loop_last != NULL);
    netif->loop_last->next = r;
    netif->loop_last = last;
  } else {
    netif->loop_first = r;
    netif->loop_last = last;
  }
  SYS_ARCH_UNPROTECT(lev);

  LINK_STATS_INC(link.xmit);
  MIB2_STATS_NETIF_ADD(stats_if, ifoutoctets, p->tot_len);
  MIB2_STATS_NETIF_INC(stats_if, ifoutucastpkts);

#if LWIP_NETIF_LOOPBACK_MULTITHREADING
  /* For multithreading environment, schedule a call to netif_poll */
  tcpip_callback_with_block((tcpip_callback_fn)netif_poll, netif, 0);
#endif /* LWIP_NETIF_LOOPBACK_MULTITHREADING */

  return ERR_OK;
}

#if LWIP_HAVE_LOOPIF
#if LWIP_IPV4
static err_t
netif_loop_output_ipv4(struct netif *netif, struct pbuf *p, const ip4_addr_t* addr)
{
  LWIP_UNUSED_ARG(addr);
  return netif_loop_output(netif, p);
}
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
static err_t
netif_loop_output_ipv6(struct netif *netif, struct pbuf *p, const ip6_addr_t* addr)
{
  LWIP_UNUSED_ARG(addr);
  return netif_loop_output(netif, p);
}
#endif /* LWIP_IPV6 */
#endif /* LWIP_HAVE_LOOPIF */


/**
 * Call netif_poll() in the main loop of your application. This is to prevent
 * reentering non-reentrant functions like tcp_input(). Packets passed to
 * netif_loop_output() are put on a list that is passed to netif->input() by
 * netif_poll().
 */
void
netif_poll(struct netif *netif)
{
  struct pbuf *in;
  /* If we have a loopif, SNMP counters are adjusted for it,
   * if not they are adjusted for 'netif'. */
#if MIB2_STATS
#if LWIP_HAVE_LOOPIF
  struct netif *stats_if = &loop_netif;
#else /* LWIP_HAVE_LOOPIF */
  struct netif *stats_if = netif;
#endif /* LWIP_HAVE_LOOPIF */
#endif /* MIB2_STATS */
  SYS_ARCH_DECL_PROTECT(lev);

  do {
    /* Get a packet from the list. With SYS_LIGHTWEIGHT_PROT=1, this is protected */
    SYS_ARCH_PROTECT(lev);
    in = netif->loop_first;
    if (in != NULL) {
      struct pbuf *in_end = in;
#if LWIP_LOOPBACK_MAX_PBUFS
      u8_t clen = 1;
#endif /* LWIP_LOOPBACK_MAX_PBUFS */
      while (in_end->len != in_end->tot_len) {
        LWIP_ASSERT("bogus pbuf: len != tot_len but next == NULL!", in_end->next != NULL);
        in_end = in_end->next;
#if LWIP_LOOPBACK_MAX_PBUFS
        clen++;
#endif /* LWIP_LOOPBACK_MAX_PBUFS */
      }
#if LWIP_LOOPBACK_MAX_PBUFS
      /* adjust the number of pbufs on queue */
      LWIP_ASSERT("netif->loop_cnt_current underflow",
        ((netif->loop_cnt_current - clen) < netif->loop_cnt_current));
      netif->loop_cnt_current -= clen;
#endif /* LWIP_LOOPBACK_MAX_PBUFS */

      /* 'in_end' now points to the last pbuf from 'in' */
      if (in_end == netif->loop_last) {
        /* this was the last pbuf in the list */
        netif->loop_first = netif->loop_last = NULL;
      } else {
        /* pop the pbuf off the list */
        netif->loop_first = in_end->next;
        LWIP_ASSERT("should not be null since first != last!", netif->loop_first != NULL);
      }
      /* De-queue the pbuf from its successors on the 'loop_' list. */
      in_end->next = NULL;
    }
    SYS_ARCH_UNPROTECT(lev);

    if (in != NULL) {
      LINK_STATS_INC(link.recv);
      MIB2_STATS_NETIF_ADD(stats_if, ifinoctets, in->tot_len);
      MIB2_STATS_NETIF_INC(stats_if, ifinucastpkts);
      /* loopback packets are always IP packets! */
      if (ip_input(in, netif) != ERR_OK) {
        pbuf_free(in);
      }
      /* Don't reference the packet any more! */
      in = NULL;
    }
  /* go on while there is a packet on the list */
  } while (netif->loop_first != NULL);
}

#if !LWIP_NETIF_LOOPBACK_MULTITHREADING
/**
 * Calls netif_poll() for every netif on the netif_list.
 */
void
netif_poll_all(void)
{
  struct netif *netif = netif_list;
  /* loop through netifs */
  while (netif != NULL) {
    netif_poll(netif);
    /* proceed to next network interface */
    netif = netif->next;
  }
}
#endif /* !LWIP_NETIF_LOOPBACK_MULTITHREADING */
#endif /* ENABLE_LOOPBACK */

#if LWIP_IPV6
/** Checks if a specific address is assigned to the netif and returns its
 * index.
 *
 * @param netif the netif to check
 * @param ip6addr the IPv6 address to find
 * @return >= 0: address found, this is its index
 *         -1: address not found on this netif
 */
s8_t
netif_get_ip6_addr_match(struct netif *netif, const ip6_addr_t *ip6addr)
{
  s8_t i;
  for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
    if (!ip6_addr_isinvalid(netif_ip6_addr_state(netif, i)) &&
        ip6_addr_cmp(netif_ip6_addr(netif, i), ip6addr)) {
      return i;
    }
  }
  return -1;
}

/** Create a link-local IPv6 address on a netif (stored in slot 0)
 *
 * @param netif the netif to create the address on
 * @param from_mac_48bit if != 0, assume hwadr is a 48-bit MAC address (std conversion)
 *                       if == 0, use hwaddr directly as interface ID
 */
void
netif_create_ip6_linklocal_address(struct netif *netif, u8_t from_mac_48bit)
{
  u8_t i, addr_index;

  /* Link-local prefix. */
  ip_2_ip6(&netif->ip6_addr[0])->addr[0] = PP_HTONL(0xfe800000ul);
  ip_2_ip6(&netif->ip6_addr[0])->addr[1] = 0;

  /* Generate interface ID. */
  if (from_mac_48bit) {
    /* Assume hwaddr is a 48-bit IEEE 802 MAC. Convert to EUI-64 address. Complement Group bit. */
    ip_2_ip6(&netif->ip6_addr[0])->addr[2] = htonl((((u32_t)(netif->hwaddr[0] ^ 0x02)) << 24) |
        ((u32_t)(netif->hwaddr[1]) << 16) |
        ((u32_t)(netif->hwaddr[2]) << 8) |
        (0xff));
    ip_2_ip6(&netif->ip6_addr[0])->addr[3] = htonl((0xfeul << 24) |
        ((u32_t)(netif->hwaddr[3]) << 16) |
        ((u32_t)(netif->hwaddr[4]) << 8) |
        (netif->hwaddr[5]));
  } else {
    /* Use hwaddr directly as interface ID. */
    ip_2_ip6(&netif->ip6_addr[0])->addr[2] = 0;
    ip_2_ip6(&netif->ip6_addr[0])->addr[3] = 0;

    addr_index = 3;
    for (i = 0; (i < 8) && (i < netif->hwaddr_len); i++) {
      if (i == 4) {
        addr_index--;
      }
      ip_2_ip6(&netif->ip6_addr[0])->addr[addr_index] |= ((u32_t)(netif->hwaddr[netif->hwaddr_len - i - 1])) << (8 * (i & 0x03));
    }
  }
  

  /* Set address state. */
#if LWIP_IPV6_DUP_DETECT_ATTEMPTS
  /* Will perform duplicate address detection (DAD). */
  netif->ip6_addr_state[0] = IP6_ADDR_TENTATIVE;
#else
  /* Consider address valid. */
  netif->ip6_addr_state[0] = IP6_ADDR_PREFERRED;
#endif /* LWIP_IPV6_AUTOCONFIG */
}

/** This function allows for the easy addition of a new IPv6 address to an interface.
 * It takes care of finding an empty slot and then sets the address tentative
 * (to make sure that all the subsequent processing happens).
 *
 * @param netif netif to add the address on
 * @param ip6addr address to add
 * @param chosen_idx if != NULL, the chosen IPv6 address index will be stored here
 */
err_t
netif_add_ip6_address(struct netif *netif, const ip6_addr_t *ip6addr, s8_t *chosen_idx)
{
  s8_t i;

  i = netif_get_ip6_addr_match(netif, ip6addr);
  if (i >= 0) {
    /* Address already added */
    if (chosen_idx != NULL) {
      *chosen_idx = i;
    }
    return ERR_OK;
  }

  /* Find a free slot -- musn't be the first one (reserved for link local) */
  for (i = 1; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
    if (!ip6_addr_isvalid(netif->ip6_addr_state[i])) {
      ip_addr_copy_from_ip6(netif->ip6_addr[i], *ip6addr);
      netif_ip6_addr_set_state(netif, i, IP6_ADDR_TENTATIVE);
      if (chosen_idx != NULL) {
        *chosen_idx = i;
      }
      return ERR_OK;
    }
  }

  if (chosen_idx != NULL) {
    *chosen_idx = -1;
  }
  return ERR_VAL;
}

/** Dummy IPv6 output function for netifs not supporting IPv6
 */
static err_t
netif_null_output_ip6(struct netif *netif, struct pbuf *p, const ip6_addr_t *ipaddr)
{
  LWIP_UNUSED_ARG(netif);
  LWIP_UNUSED_ARG(p);
  LWIP_UNUSED_ARG(ipaddr);

  return ERR_IF;
}
#endif /* LWIP_IPV6 */
