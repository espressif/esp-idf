/*
 * SPDX-FileCopyrightText: 2001-2003 Swedish Institute of Computer Science
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * SPDX-FileContributor: 2022-2023 Espressif Systems (Shanghai) CO LTD
 */
#include <string.h>
#include "lwip/opt.h"
#include "lwip/pbuf.h"
#include "lwip/snmp.h"
#include "lwip/ethip6.h"
#include "netif/etharp.h"

#include "esp_netif.h"
#include "esp_netif_net_stack.h"

#define IFNAME0 't'
#define IFNAME1 'p'

static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
    esp_netif_t *esp_netif = esp_netif_get_handle_from_netif_impl(netif);
    char buf[1518]; /* max packet size including VLAN excluding CRC */

    if (p->tot_len > sizeof(buf)) {
        MIB2_STATS_NETIF_INC(netif, ifoutdiscards);
        LWIP_DEBUGF(NETIF_DEBUG, ("tapif: packet too large"));
        return ERR_IF;
    }

    /* initiate transfer(); */
    pbuf_copy_partial(p, buf, p->tot_len, 0);

    int ret = esp_netif_transmit(esp_netif, buf, p->tot_len);
    /* Check error */
    if (likely(ret == ESP_OK)) {
        return ERR_OK;
    }
    if (ret == ESP_ERR_NO_MEM) {
        return ERR_MEM;
    }
    return ERR_IF;
}

static void
low_level_init(struct netif *netif)
{

    /* Obtain MAC address from network interface. */
    netif->hwaddr[0] = 0x02;
    netif->hwaddr[1] = 0x12;
    netif->hwaddr[2] = 0x34;
    netif->hwaddr[3] = 0x56;
    netif->hwaddr[4] = 0x78;
    netif->hwaddr[5] = 0xab;
    netif->hwaddr_len = 6;

    /* device capabilities */
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP;
}

err_t lwip_tapif_init(struct netif *netif)
{
    LWIP_ASSERT("Tried to initialize tapif with NULL netif", netif != NULL);
    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;
#if LWIP_IPV4
    netif->output = etharp_output;
#endif /* LWIP_IPV4 */
#if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */
    netif->linkoutput = low_level_output;
    netif->mtu = 1500;

    low_level_init(netif);
    netif_set_link_up(netif);
    return ERR_OK;
}

void lwip_tapif_input(void *h, void *buffer, size_t len, void *l2_buff)
{
    struct netif *netif = h;
    struct pbuf *p;
    LWIP_ASSERT("running tapif input with NULL netif", netif != NULL);

    p = pbuf_alloc(PBUF_RAW, len, PBUF_RAM);
    if (p == NULL) {
        return;
    }
    memcpy(p->payload, buffer, len);

    /* full packet send to tcpip_thread to process */
    if (unlikely(netif->input(p, netif) != ERR_OK)) {
        LWIP_DEBUGF(NETIF_DEBUG, ("tapif_input: IP input error\n"));
        pbuf_free(p);
    }
}
