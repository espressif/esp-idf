/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "lwip/opt.h"

/* don't build if not configured for use in lwipopts.h */
#if LWIP_IPV4 && DHCP_DOES_ARP_CHECK

#include "lwip/acd.h"
#include "lwip/dhcp.h"
#include "lwip/prot/dhcp.h"
#include "lwip/timeouts.h"

#define ACD_DHCP_ARP_REPLY_TIMEOUT_MS 500

static void
acd_dhcp_check_timeout_cb(void *arg);

static void
acd_suspend(struct netif *netif)
{
    struct dhcp *dhcp;
    struct acd *acd;
    if (netif == NULL || (dhcp = netif_dhcp_data(netif)) == NULL) {
        return;
    }
    acd = &dhcp->acd;
    acd->state = ACD_STATE_OFF;
    sys_untimeout(acd_dhcp_check_timeout_cb, (void *)netif);
}

void
acd_remove(struct netif *netif, struct acd *acd)
{
    acd_suspend(netif);
    acd->acd_conflict_callback = NULL;
}

void
acd_netif_ip_addr_changed(struct netif *netif, const ip_addr_t *old_addr,
                          const ip_addr_t *new_addr)
{
    acd_suspend(netif);
}

void
acd_network_changed_link_down(struct netif *netif)
{
    acd_suspend(netif);
}

void
acd_arp_reply(struct netif *netif, struct etharp_hdr *hdr)
{
    struct dhcp *dhcp;
    ip4_addr_t sipaddr;
    struct eth_addr netifaddr;
    struct acd *acd;
    if (netif == NULL || (dhcp = netif_dhcp_data(netif)) == NULL) {
        return;
    }
    acd = &dhcp->acd;
    // Check that we're looking for ARP reply in ACD_PROBING state and DHCP_CHECKING state
    if (hdr->opcode != PP_HTONS(ARP_REPLY) || dhcp->state != DHCP_STATE_CHECKING ||
        acd->state != ACD_STATE_PROBING || acd->acd_conflict_callback == NULL) {
        return;
    }
    SMEMCPY(netifaddr.addr, netif->hwaddr, ETH_HWADDR_LEN);
    IPADDR_WORDALIGNED_COPY_TO_IP4_ADDR_T(&sipaddr, &hdr->sipaddr);
    LWIP_DEBUGF(ACD_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("acd_arp_reply(): CHECKING, arp reply for 0x%08"X32_F"\n",
            ip4_addr_get_u32(&sipaddr)));
    /* did another host (not our mac addr) respond with the address we were offered by the DHCP server? */
    if (ip4_addr_eq(&sipaddr, &dhcp->offered_ip_addr) &&
        !eth_addr_eq(&netifaddr, &hdr->shwaddr)) {
        LWIP_DEBUGF(ACD_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE | LWIP_DBG_LEVEL_WARNING,
                    ("acd_arp_reply(): arp reply matched with offered address, declining\n"));
        dhcp->acd.acd_conflict_callback(netif, ACD_DECLINE);
    }
}

err_t
acd_add(struct netif *netif, struct acd *acd,
        acd_conflict_callback_t acd_conflict_callback)
{
    acd->acd_conflict_callback = acd_conflict_callback;
    return ERR_OK;
}

static void send_probe_once(struct netif *netif)
{
    struct dhcp *dhcp = netif_dhcp_data(netif);
    if (etharp_query(netif, &dhcp->offered_ip_addr, NULL) != ERR_OK) {
        LWIP_DEBUGF(ACD_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING, ("acd_send_probe_once(): could not perform ARP query\n"));
        return;
    }
    if (dhcp->tries < 255) {
        dhcp->tries++;
    }
    LWIP_DEBUGF(ACD_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("acd_send_probe_once(): set request timeout %"U16_F" msecs\n", ACD_DHCP_ARP_REPLY_TIMEOUT_MS));
    sys_timeout(ACD_DHCP_ARP_REPLY_TIMEOUT_MS, acd_dhcp_check_timeout_cb, (void *)netif);
}

static void
acd_dhcp_check_timeout_cb(void *arg)
{
    struct netif *netif = (struct netif *)arg;
    struct dhcp *dhcp;
    struct acd *acd;
    if (netif == NULL || (dhcp = netif_dhcp_data(netif)) == NULL) {
        return;
    }
    acd = &dhcp->acd;
    if (acd->state != ACD_STATE_PROBING || acd->acd_conflict_callback == NULL || dhcp->state != DHCP_STATE_CHECKING) {
        return;
    }
    LWIP_DEBUGF(ACD_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_STATE, ("acd_dhcp_check_timeout_cb(): CHECKING, ARP request timed out\n"));
    if (dhcp->tries <= 1) {
        send_probe_once(netif);
    } else {
        // No conflict detected
        dhcp->acd.acd_conflict_callback(netif, ACD_IP_OK);
    }
}

err_t
acd_start(struct netif *netif, struct acd *acd, ip4_addr_t ipaddr)
{
    if (netif == NULL || netif_dhcp_data(netif) == NULL) {
        return ERR_ARG;
    }
    acd->state = ACD_STATE_PROBING;
    send_probe_once(netif);
    return ERR_OK;
}

#endif /* LWIP_IPV4 && DHCP_DOES_ARP_CHECK */
