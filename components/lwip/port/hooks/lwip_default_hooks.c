/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "lwip_default_hooks.h"
#include "lwip/prot/dhcp.h"
#include "lwip/dhcp.h"
#include "lwip/prot/iana.h"
#include <string.h>

#define __weak __attribute__((weak))

#ifdef CONFIG_LWIP_HOOK_IP6_ROUTE_DEFAULT
struct netif *__weak
lwip_hook_ip6_route(const ip6_addr_t *src, const ip6_addr_t *dest)
{
    LWIP_UNUSED_ARG(src);
    LWIP_UNUSED_ARG(dest);

    return NULL;
}
#endif

#ifdef CONFIG_LWIP_HOOK_NETCONN_EXT_RESOLVE_DEFAULT
int __weak lwip_hook_netconn_external_resolve(const char *name, ip_addr_t *addr, u8_t addrtype, err_t *err)
{
    LWIP_UNUSED_ARG(name);
    LWIP_UNUSED_ARG(addr);
    LWIP_UNUSED_ARG(addrtype);
    LWIP_UNUSED_ARG(err);

    return 0;
}
#endif

#ifdef CONFIG_LWIP_HOOK_ND6_GET_GW_DEFAULT
const ip6_addr_t *__weak lwip_hook_nd6_get_gw(struct netif *netif, const ip6_addr_t *dest)
{
    LWIP_UNUSED_ARG(netif);
    LWIP_UNUSED_ARG(dest);

    return NULL;
}
#endif

#ifdef CONFIG_LWIP_HOOK_IP6_SELECT_SRC_ADDR_DEFAULT
const ip_addr_t *__weak lwip_hook_ip6_select_source_address(struct netif *netif, const ip6_addr_t *dest)
{
    LWIP_UNUSED_ARG(netif);
    LWIP_UNUSED_ARG(dest);

    return NULL;
}
#endif

#ifdef CONFIG_LWIP_HOOK_IP6_INPUT_DEFAULT
/**
 * @brief The default IPv6 input hook checks if we already have an IPv6 address (netif->ip6_addr[0] is link local),
 * so we drop all incoming IPv6 packets if the input netif has no LL address.
 *
 * LWIP accepts IPv6 multicast packets even if the ip6_addr[] for the given address wasn't set,
 * this may cause trouble if we enable IPv6 SLAAC (LWIP_IPV6_AUTOCONFIG), but have not created any LL address.
 * If the router sends a packet to all nodes 0xff01::1 with RDNSS servers, it would be accepted and rewrite
 * DNS server info with IPv6 values (which won't be routable without any IPv6 address assigned)
 */
int __weak lwip_hook_ip6_input(struct pbuf *p, struct netif *inp)
{
    /* Check if the first IPv6 address (link-local) is unassigned (all zeros).
     * If the address is empty, it indicates that no link-local address has been configured,
     * and the interface should not accept incoming IPv6 traffic. */
    if (ip6_addr_isany(ip_2_ip6(&inp->ip6_addr[0]))) {
        /* We don't have an LL address -> eat this packet here, so it won't get accepted on the input netif */
        pbuf_free(p);
        return 1;
    }
    return 0;
}
#endif

#ifdef CONFIG_LWIP_IPV4

#ifdef LWIP_HOOK_IP4_ROUTE_SRC
#if ESP_IP4_ROUTE
#include "lwip/netif.h"

bool ip4_netif_exist(const ip4_addr_t *src, const ip4_addr_t *dest)
{
    struct netif *netif = NULL;

    for (netif = netif_list; netif != NULL; netif = netif->next) {
        /* is the netif up, does it have a link and a valid address? */
        if (netif_is_up(netif) && netif_is_link_up(netif) && !ip4_addr_isany_val(*netif_ip4_addr(netif))) {
            /* source netif and dest netif match? */
            if (ip4_addr_netcmp(src, netif_ip4_addr(netif), netif_ip4_netmask(netif)) || ip4_addr_netcmp(dest, netif_ip4_addr(netif), netif_ip4_netmask(netif))) {
                /* return false when both netif don't match */
                return true;
            }
        }
    }

    return false;
}

/**
 * Source based IPv4 routing hook function.
 */
struct netif *
ip4_route_src_hook(const ip4_addr_t *src,const ip4_addr_t *dest)
{
    struct netif *netif = NULL;

    /* destination IP is broadcast IP? */
    if ((src != NULL) && !ip4_addr_isany(src)) {
        /* iterate through netifs */
        for (netif = netif_list; netif != NULL; netif = netif->next) {
            /* is the netif up, does it have a link and a valid address? */
            if (netif_is_up(netif) && netif_is_link_up(netif) && !ip4_addr_isany_val(*netif_ip4_addr(netif))) {
                /* source IP matches? */
                if (ip4_addr_cmp(src, netif_ip4_addr(netif))) {
                    /* return netif on which to forward IP packet */

                    return netif;
                }
            }
        }
    }
    return netif;
}
#endif
#endif /* LWIP_HOOK_IP4_ROUTE_SRC */

#define LWIP_DHCP_ENABLE_MTU_UPDATE         1
#define LWIP_DHCP_ENABLE_VENDOR_SPEC_IDS    !ESP_DHCP_DISABLE_VENDOR_CLASS_IDENTIFIER
#define LWIP_DHCP_ENABLE_CLIENT_ID          !ESP_DHCP_DISABLE_CLIENT_ID

#if LWIP_DHCP_ENABLE_VENDOR_SPEC_IDS
#define DHCP_OPTION_VSI             43
#define DHCP_OPTION_VCI             60
#define DHCP_OPTION_VSI_MAX         64

static u8_t vendor_class_len = 0;
static char *vendor_class_buf = NULL;
static u8_t dhcp_option_vsi[DHCP_OPTION_VSI_MAX];

void dhcp_free_vendor_class_identifier(void)
{
  mem_free(vendor_class_buf);
}

int dhcp_get_vendor_specific_information(uint8_t len, char * str)
{
  u8_t copy_len = 0;

  if (len == 0 || str == NULL) {
    return ERR_ARG;
  }

  copy_len = LWIP_MIN(len, sizeof(dhcp_option_vsi));

  memcpy(str, dhcp_option_vsi, copy_len);

  return ERR_OK;
}

int dhcp_set_vendor_class_identifier(uint8_t len, const char * str)
{
  if (len == 0 || str == NULL) {
    return ERR_ARG;
  }

  if (vendor_class_buf && vendor_class_len != len) {
    mem_free(vendor_class_buf);
    vendor_class_buf = NULL;
  }

  if (!vendor_class_buf) {
    vendor_class_buf = (char *)mem_malloc(len + 1);
    if (vendor_class_buf == NULL) {
      return ERR_MEM;
    }

    vendor_class_len = len;
  }

  memcpy(vendor_class_buf, str, len);
  return ERR_OK;
}
#endif /* LWIP_DHCP_ENABLE_VENDOR_SPEC_IDS */

void dhcp_parse_extra_opts(struct dhcp *dhcp, uint8_t state, uint8_t option, uint8_t len, struct pbuf* p, uint16_t offset)
{
    LWIP_UNUSED_ARG(dhcp);
    LWIP_UNUSED_ARG(state);
    LWIP_UNUSED_ARG(option);
    LWIP_UNUSED_ARG(len);
    LWIP_UNUSED_ARG(p);
    LWIP_UNUSED_ARG(offset);
#if LWIP_DHCP_ENABLE_MTU_UPDATE
    if ((option == DHCP_OPTION_MTU) &&
     (state == DHCP_STATE_REBOOTING || state == DHCP_STATE_REBINDING ||
      state == DHCP_STATE_RENEWING  || state == DHCP_STATE_REQUESTING)) {
    u32_t mtu = 0;
    struct netif *netif;
    LWIP_ERROR("dhcp_parse_extra_opts(): MTU option's len != 2", len == 2, return;);
    LWIP_ERROR("dhcp_parse_extra_opts(): extracting MTU option failed",
               pbuf_copy_partial(p, &mtu, 2, offset) == 2, return;);
    mtu = lwip_htons((u16_t)mtu);
    NETIF_FOREACH(netif) {
      /* find the netif related to this dhcp */
      if (dhcp == netif_dhcp_data(netif)) {
        if (mtu < netif->mtu) {
          netif->mtu = mtu;
          LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_parse_extra_opts(): Negotiated netif MTU is %d\n", netif->mtu));
        }
        return;
      }
    }
  } /* DHCP_OPTION_MTU */
#endif /* LWIP_DHCP_ENABLE_MTU_UPDATE */
#if LWIP_DHCP_ENABLE_VENDOR_SPEC_IDS
    if ((option == DHCP_OPTION_VSI) &&
             (state == DHCP_STATE_REBOOTING || state == DHCP_STATE_REBINDING ||
              state == DHCP_STATE_RENEWING  || state == DHCP_STATE_REQUESTING || state == DHCP_STATE_SELECTING)) {
      u16_t copy_len;
      copy_len = LWIP_MIN(len, sizeof(dhcp_option_vsi));
      LWIP_ERROR("dhcp_parse_extra_opts(): extracting VSI option failed",
         pbuf_copy_partial(p, &dhcp_option_vsi, copy_len, offset) == copy_len, return;);
    } /* DHCP_OPTION_VSI */
#endif /* LWIP_DHCP_ENABLE_VENDOR_SPEC_IDS */
}

void dhcp_append_extra_opts(struct netif *netif, uint8_t state, struct dhcp_msg *msg_out, uint16_t *options_out_len)
{
    LWIP_UNUSED_ARG(netif);
    LWIP_UNUSED_ARG(state);
    LWIP_UNUSED_ARG(msg_out);
    LWIP_UNUSED_ARG(options_out_len);
#if LWIP_DHCP_ENABLE_CLIENT_ID
    if (state == DHCP_STATE_RENEWING || state == DHCP_STATE_REBINDING ||
      state == DHCP_STATE_REBOOTING || state == DHCP_STATE_OFF ||
      state == DHCP_STATE_REQUESTING || state == DHCP_STATE_BACKING_OFF || state == DHCP_STATE_SELECTING) {
    size_t i;
    u8_t *options = msg_out->options + *options_out_len;
    LWIP_ERROR("dhcp_append(client_id): options_out_len + 3 + netif->hwaddr_len <= DHCP_OPTIONS_LEN",
               *options_out_len + 3U + netif->hwaddr_len <= DHCP_OPTIONS_LEN, return;);
    *options_out_len = *options_out_len + netif->hwaddr_len + 3;
    *options++ = DHCP_OPTION_CLIENT_ID;
    *options++ = netif->hwaddr_len + 1; /* option size */
    *options++ = LWIP_IANA_HWTYPE_ETHERNET;
    for (i = 0; i < netif->hwaddr_len; i++) {
      *options++ = netif->hwaddr[i];
    }
  }
#endif /* LWIP_DHCP_ENABLE_CLIENT_ID */
#if LWIP_DHCP_ENABLE_VENDOR_SPEC_IDS
    if (state == DHCP_STATE_RENEWING || state == DHCP_STATE_REBINDING ||
      state == DHCP_STATE_REBOOTING || state == DHCP_STATE_OFF ||
      state == DHCP_STATE_REQUESTING || state == DHCP_STATE_BACKING_OFF || state == DHCP_STATE_SELECTING) {
    size_t i;
    const char *p = NULL;
    u8_t len = 0;

    if (vendor_class_buf && vendor_class_len) {
      p = vendor_class_buf;
      len = vendor_class_len;
    } else {
#if LWIP_NETIF_HOSTNAME
      size_t namelen;
      if (netif->hostname != NULL && (namelen = strlen(netif->hostname)) < 0xff) {
        p = netif->hostname;
        len = (u8_t)namelen;
      }
#endif /* LWIP_NETIF_HOSTNAME */
    }
    LWIP_ERROR("dhcp_append(vci): options_out_len + 3 + vci_size <= DHCP_OPTIONS_LEN",
               *options_out_len + 3U + len <= DHCP_OPTIONS_LEN, return;);
    if (p) {
      u8_t *options = msg_out->options + *options_out_len;
      *options_out_len = *options_out_len + len + 3;
      *options++ = DHCP_OPTION_VCI;
      *options++ = len;
      for (i = 0; i < len; i ++) {
        *options++ = p[i];
      }
    }
    return;
  }
#endif /* LWIP_DHCP_ENABLE_VENDOR_SPEC_IDS */

}

#endif /* CONFIG_LWIP_IPV4 */
