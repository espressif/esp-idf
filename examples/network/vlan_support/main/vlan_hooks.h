/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "lwip/netif.h"
#include "lwip/pbuf.h"
#include "lwip/prot/ethernet.h"

/**
 * @brief Returns the custom data stored in the supplied `netif` struct representing its vlan tag
 *
 * @param netif The lwIP network interface on which to send the packet
 * @param p The packet to send. pbuf layer must be @ref PBUF_LINK.
 * @param src The source MAC address to be copied into the ethernet header
 * @param dst The destination MAC address to be copied into the ethernet header
 * @param eth_type Ethernet type (@ref lwip_ieee_eth_type)
 *
 * @return Returns the vlan tag of the interface or -1.
 */
static inline int lwip_vlan_set(struct netif *netif, struct pbuf *p,
                                const struct eth_addr *src, const struct eth_addr *dst,
                                u16_t eth_type)
{
    u16_t vlan_id =  *((uint16_t *)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_MAX + 1));

    if (0xFFF == vlan_id) {
        return -1;
    } else {
        return vlan_id;
    }
}

/**
 * @brief Checks if the vlan tag in the frame matches the vlan tag of the interface.
 *
 * @param netif: struct netif on which the packet has been received
 * @param eth_hdr: struct eth_hdr of the packet
 * @param vlan_hdr: struct eth_vlan_hdr of the packet
 *
 * @return Returns true if the vlan tag in the frame matches the vlan tag of the interface, else returns false.
 */
static inline bool lwip_vlan_check(struct netif *netif, struct eth_hdr *eth_hdr, struct eth_vlan_hdr *vlan_hdr)
{
    u16_t vlan_id =  *((uint16_t *)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_MAX + 1));

    if (vlan_id == VLAN_ID(vlan_hdr)) {
        return true;
    }

    return false;
}
