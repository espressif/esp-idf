/*
 * SPDX-FileCopyrightText: 2001-2004 Swedish Institute of Computer Science
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * SPDX-FileContributor: 2015-2022 Espressif Systems (Shanghai) CO LTD
 */
/**
 * @file
 * Ethernet Interface Skeleton
 *
 */

#include <stdio.h>
#include <string.h>
#include "lwip/opt.h"
#include "lwip/pbuf.h"
#include "lwip/ethip6.h"
#include "netif/etharp.h"

#include "esp_compiler.h"
#include "esp_netif.h"
#include "esp_netif_net_stack.h"
#include "lwip/esp_netif_net_stack.h"
#include "lwip/esp_pbuf_ref.h"

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

/**
 * In this function, the hardware should be initialized.
 * Invoked by ethernetif_init().
 *
 * @param netif lwip network interface which has already been initialized
 */
static void ethernet_low_level_init(struct netif *netif)
{
    /* set MAC hardware address length */
    netif->hwaddr_len = ETH_HWADDR_LEN;

    /* maximum transfer unit */
    netif->mtu = 1500;

    /* device capabilities */
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET;

#if ESP_LWIP
#if LWIP_IGMP
    netif->flags |= NETIF_FLAG_IGMP;
#endif
#endif

#if ESP_IPV6
#if LWIP_IPV6 && LWIP_IPV6_MLD
    netif->flags |= NETIF_FLAG_MLD6;
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
            pbuf_copy(q, p);
        } else {
            return ERR_MEM;
        }
        ret = esp_netif_transmit(esp_netif, q->payload, q->len);
        /* content in payload has been copied to DMA buffer, it's safe to free pbuf now */
        pbuf_free(q);
    }
    /* Check error */
    if (likely(ret == ESP_OK)) {
        return ERR_OK;
    }
    if (ret == ESP_ERR_NO_MEM) {
        return ERR_MEM;
    }
    return ERR_IF;
}

/**
 * @brief This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param h lwip network interface structure (struct netif) for this ethernetif
 * @param buffer ethernet buffer
 * @param len length of buffer
 * @param l2_buff Placeholder for a separate L2 buffer. Unused for ethernet interface
 */
esp_netif_recv_ret_t ethernetif_input(void *h, void *buffer, size_t len, void *l2_buff)
{
    struct netif *netif = h;
    esp_netif_t *esp_netif = esp_netif_get_handle_from_netif_impl(netif);
    struct pbuf *p;

    if (unlikely(buffer == NULL || !netif_is_up(netif))) {
        if (buffer) {
            esp_netif_free_rx_buffer(esp_netif, buffer);
        }
        return ESP_NETIF_OPTIONAL_RETURN_CODE(ESP_FAIL);
    }

    /* allocate custom pbuf to hold  */
    p = esp_pbuf_allocate(esp_netif, buffer, len, buffer);
    if (p == NULL) {
        esp_netif_free_rx_buffer(esp_netif, buffer);
        return ESP_NETIF_OPTIONAL_RETURN_CODE(ESP_ERR_NO_MEM);
    }
    /* full packet send to tcpip_thread to process */
    if (unlikely(netif->input(p, netif) != ERR_OK)) {
        LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
        pbuf_free(p);
        return ESP_NETIF_OPTIONAL_RETURN_CODE(ESP_FAIL);
    }
    /* the pbuf will be free in upper layer, eg: ethernet_input */
    return ESP_NETIF_OPTIONAL_RETURN_CODE(ESP_OK);
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
    esp_netif_t *esp_netif = netif->state;
    /* Initialize interface hostname */
#if LWIP_NETIF_HOSTNAME
#if ESP_LWIP
    if (esp_netif_get_hostname(esp_netif, &netif->hostname) != ESP_OK) {
        netif->hostname = CONFIG_LWIP_LOCAL_HOSTNAME;
    }
#else
    netif->hostname = "lwip";
#endif

#endif /* LWIP_NETIF_HOSTNAME */

    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;
#if LWIP_IPV4
    netif->output = etharp_output;
#endif
#if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */
    netif->linkoutput = ethernet_low_level_output;

    ethernet_low_level_init(netif);

    return ERR_OK;
}
