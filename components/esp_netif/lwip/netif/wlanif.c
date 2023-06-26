/*
 * SPDX-FileCopyrightText: 2001-2004 Swedish Institute of Computer Science
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * SPDX-FileContributor: 2015-2022 Espressif Systems (Shanghai) CO LTD
 */
/**
 * @file
 * Ethernet Interface Skeleton used for WiFi
 *
 */

#include <stdio.h>
#include <string.h>
#include "lwip/opt.h"
#include "lwip/pbuf.h"
#include "lwip/snmp.h"
#include "lwip/ethip6.h"
#include "netif/etharp.h"

#include "esp_netif.h"
#include "esp_netif_net_stack.h"
#include "lwip/esp_netif_net_stack.h"
#include "esp_compiler.h"
#include "lwip/esp_pbuf_ref.h"

/**
 * In this function, the hardware should be initialized.
 * Called from wlanif_input().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this wlanif
 */
static void
low_level_init(struct netif *netif)
{
  /* set MAC hardware address length */
  netif->hwaddr_len = ETH_HWADDR_LEN;

  /* set MAC hardware address */

  /* maximum transfer unit */
  netif->mtu = 1500;

  /* device capabilities */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

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
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this wlanif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become available since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */
static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    esp_netif_t *esp_netif = netif->state;
    if (esp_netif == NULL) {
        return ERR_IF;
    }

    struct pbuf *q = p;
    esp_err_t ret;

    if(q->next == NULL) {
        ret = esp_netif_transmit_wrap(esp_netif, q->payload, q->len, q);

    } else {
        LWIP_DEBUGF(PBUF_DEBUG, ("low_level_output: pbuf is a list, application may has bug"));
        q = pbuf_alloc(PBUF_RAW_TX, p->tot_len, PBUF_RAM);
        if (q != NULL) {
            pbuf_copy(q, p);
        } else {
            return ERR_MEM;
        }
        ret = esp_netif_transmit_wrap(esp_netif, q->payload, q->len, q);

        pbuf_free(q);
    }

    if (ret == ESP_OK) {
        return ERR_OK;
    }
    if (ret == ESP_ERR_NO_MEM) {
        return ERR_MEM;
    }
    if (ret == ESP_ERR_INVALID_ARG) {
        return ERR_ARG;
    }
    return ERR_IF;
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param h lwip network interface structure (struct netif) for this ethernetif
 * @param buffer wlan buffer
 * @param len length of buffer
 * @param l2_buff wlan's L2 buffer pointer
 */
esp_netif_recv_ret_t wlanif_input(void *h, void *buffer, size_t len, void* l2_buff)
{
    struct netif * netif = h;
    esp_netif_t *esp_netif = netif->state;
    struct pbuf *p;

    if(unlikely(!buffer || !netif_is_up(netif))) {
        if (l2_buff) {
            esp_netif_free_rx_buffer(esp_netif, l2_buff);
        }
        return ESP_NETIF_OPTIONAL_RETURN_CODE(ESP_FAIL);
    }

#ifdef CONFIG_LWIP_L2_TO_L3_COPY
    p = pbuf_alloc(PBUF_RAW, len, PBUF_RAM);
    if (p == NULL) {
        esp_netif_free_rx_buffer(esp_netif, l2_buff);
        return ESP_NETIF_OPTIONAL_RETURN_CODE(ESP_ERR_NO_MEM);
    }
    memcpy(p->payload, buffer, len);
    esp_netif_free_rx_buffer(esp_netif, l2_buff);
#else
    p = esp_pbuf_allocate(esp_netif, buffer, len, l2_buff);
    if (p == NULL) {
        esp_netif_free_rx_buffer(esp_netif, l2_buff);
        return ESP_NETIF_OPTIONAL_RETURN_CODE(ESP_ERR_NO_MEM);
    }

#endif

    /* full packet send to tcpip_thread to process */
    if (unlikely(netif->input(p, netif) != ERR_OK)) {
        LWIP_DEBUGF(NETIF_DEBUG, ("wlanif_input: IP input error\n"));
        pbuf_free(p);
        return ESP_NETIF_OPTIONAL_RETURN_CODE(ESP_FAIL);
    }
    return ESP_NETIF_OPTIONAL_RETURN_CODE(ESP_OK);
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this wlanif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t
wlanif_init(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */

#if ESP_LWIP
    if (esp_netif_get_hostname(netif->state, &netif->hostname) != ESP_OK) {
        netif->hostname = CONFIG_LWIP_LOCAL_HOSTNAME;
    }
#else
    netif->hostname = "lwip";
#endif

#endif /* LWIP_NETIF_HOSTNAME */

    /*
     * Initialize the snmp variables and counters inside the struct netif.
     * The last argument should be replaced with your link speed, in units
     * of bits per second.
     */
    NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, 100);

    /* We directly use etharp_output() here to save a function call.
     * You can instead declare your own function an call etharp_output()
     * from it if you have to do some checks before sending (e.g. if link
     * is available...) */
#if LWIP_IPV4
    netif->output = etharp_output;
#endif
#if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */
    netif->linkoutput = low_level_output;

    /* initialize the hardware */
    low_level_init(netif);

    return ERR_OK;
}

err_t wlanif_init_sta(struct netif *netif) {
    netif->name[0] = 's';
    netif->name[1] = 't';
    return wlanif_init(netif);
}

err_t wlanif_init_ap(struct netif *netif) {
    netif->name[0] = 'a';
    netif->name[1] = 'p';
    return wlanif_init(netif);
}

err_t wlanif_init_nan(struct netif *netif) {
    netif->name[0] = 'n';
    netif->name[1] = 'a';
    return wlanif_init(netif);
}
