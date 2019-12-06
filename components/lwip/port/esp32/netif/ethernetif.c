/**
 * @file
 * Ethernet Interface Skeleton
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
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/ethip6.h"
#include "netif/etharp.h"
#include <stdio.h>
#include <string.h>

#include "esp_eth.h"
#include "esp_netif.h"
#include "esp_netif_net_stack.h"

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

/**
 * @brief Free resources allocated in L2 layer
 *
 * @param buf memory alloc in L2 layer
 * @note this function is also the callback when invoke pbuf_free
 */
static void ethernet_free_rx_buf_l2(struct netif *netif, void *buf)
{
    free(buf);
}

/**
 * In this function, the hardware should be initialized.
 * Invoked by ethernetif_init().
 *
 * @param netif lwip network interface which has already been initialized
 */
static void ethernet_low_level_init(struct netif *netif)
{
    /* set MAC hardware address length */
    netif->hwaddr_len = ETHARP_HWADDR_LEN;

    /* maximum transfer unit */
    netif->mtu = 1500;

    /* device capabilities */
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

#if ESP_LWIP
#if LWIP_IGMP
    netif->flags |= NETIF_FLAG_IGMP;
#endif
#endif
}

/**
 * @brief This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf might be chained.
 *
 * @param netif lwip network interface structure for this ethernetif
 * @param p MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return err_t ERR_OK if the packet has been sent to Ethernet DMA buffer successfully
 *               ERR_MEM if private data couldn't be allocated
 *               ERR_IF if netif is not supported
 *               ERR_ABORT if there's some wrong when send pbuf payload to DMA buffer
 */
static err_t ethernet_low_level_output(struct netif *netif, struct pbuf *p)
{
    struct pbuf *q = p;
    esp_netif_t *esp_netif = esp_netif_get_handle_from_netif_impl(netif);
    esp_err_t ret = ESP_FAIL;
    if (!esp_netif) {
        LWIP_DEBUGF(NETIF_DEBUG, ("corresponding esp-netif is NULL: netif=%p pbuf=%p len=%d\n", netif, p, p->len));
        return ERR_IF;
    }

    if (q->next == NULL) {
        ret = esp_netif_transmit(esp_netif, q->payload, q->len);
    } else {
        LWIP_DEBUGF(PBUF_DEBUG, ("low_level_output: pbuf is a list, application may has bug"));
        q = pbuf_alloc(PBUF_RAW_TX, p->tot_len, PBUF_RAM);
        if (q != NULL) {
#if ESP_LWIP
            /* This pbuf RAM was not allocated on layer2, no extra free operation needed in pbuf_free */
            q->l2_owner = NULL;
            q->l2_buf = NULL;
#endif
            pbuf_copy(q, p);
        } else {
            return ERR_MEM;
        }
        ret = esp_netif_transmit(esp_netif, q->payload, q->len);
        /* content in payload has been copied to DMA buffer, it's safe to free pbuf now */
        pbuf_free(q);
    }
    /* Check error */
    if (ret != ESP_OK) {
        return ERR_ABRT;
    } else {
        return ERR_OK;
    }
}

/**
 * @brief This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif lwip network interface structure for this ethernetif
 * @param buffer ethernet buffer
 * @param len length of buffer
 */
void ethernetif_input(struct netif *netif, void *buffer, size_t len, void *eb)
{
    struct pbuf *p;

    if (buffer == NULL || !netif_is_up(netif)) {
        if (buffer) {
            ethernet_free_rx_buf_l2(netif, buffer);
        }
        return;
    }

    /* acquire new pbuf, type: PBUF_REF */
    p = pbuf_alloc(PBUF_RAW, len, PBUF_REF);
    if (p == NULL) {
        ethernet_free_rx_buf_l2(netif, buffer);
        return;
    }
    p->payload = buffer;
#if ESP_LWIP
    p->l2_owner = netif;
    p->l2_buf = buffer;
#endif
    /* full packet send to tcpip_thread to process */
    if (netif->input(p, netif) != ERR_OK) {
        LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
        pbuf_free(p);
    }
    /* the pbuf will be free in upper layer, eg: ethernet_input */
}

/**
 * Set up the network interface. It calls the function low_level_init() to do the
 * actual init work of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the ethernetif is initialized
 */
err_t ethernetif_init(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));
    /* Have to get the esp-netif handle from netif first and then driver==ethernet handle from there */
    esp_eth_handle_t eth_handle = esp_netif_get_io_driver(esp_netif_get_handle_from_netif_impl(netif));
    /* Initialize interface hostname */
#if LWIP_NETIF_HOSTNAME
#if ESP_LWIP
    netif->hostname = CONFIG_LWIP_LOCAL_HOSTNAME;
#else
    netif->hostname = "lwip";
#endif

#endif /* LWIP_NETIF_HOSTNAME */

    /* Initialize the snmp variables and counters inside the struct netif. */
    eth_speed_t speed;

    esp_eth_ioctl(eth_handle, ETH_CMD_G_SPEED, &speed);
    if (speed == ETH_SPEED_100M) {
        NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, 100000000);
    } else {
        NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, 10000000);
    }

    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;
    netif->output = etharp_output;
#if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */
    netif->linkoutput = ethernet_low_level_output;
    netif->l2_buffer_free_notify = ethernet_free_rx_buf_l2;

    ethernet_low_level_init(netif);

    return ERR_OK;
}
