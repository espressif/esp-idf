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
#include "tcpip_adapter.h"

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

static char hostname[16];

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void
ethernet_low_level_init(struct netif *netif)
{ 
  /* set MAC hardware address length */
  netif->hwaddr_len = ETHARP_HWADDR_LEN;

  /* set MAC hardware address */

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
  /* Do whatever else is needed to initialize interface. */  
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */
static err_t
ethernet_low_level_output(struct netif *netif, struct pbuf *p)
{
  struct pbuf *q;
  esp_interface_t eth_if = tcpip_adapter_get_esp_if(netif);

  if (eth_if != ESP_IF_ETH) {
    LWIP_DEBUGF(NETIF_DEBUG,("eth_if=%d netif=%p pbuf=%p len=%d\n", eth_if, netif, p, p->len)); 

    return ERR_IF;
  } 
  
#if ESP_LWIP
    q = p;
    u16_t pbuf_x_len = 0;
    pbuf_x_len = q->len;
    if(q->next !=NULL)
    {
        //char cnt = 0;
        struct pbuf *tmp = q->next;
        while(tmp != NULL)
        {
            memcpy( (u8_t *)( (u8_t *)(q->payload) + pbuf_x_len), (u8_t *)tmp->payload , tmp->len );
            pbuf_x_len += tmp->len;
            //cnt++;
            tmp = tmp->next;
        }
    }
   
    return esp_eth_tx(q->payload, pbuf_x_len);
#else
    for(q = p; q != NULL; q = q->next) {
        return esp_emac_tx(q->payload, q->len);
    }
  return ERR_OK;
#endif
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
void
ethernetif_input(struct netif *netif, void *buffer, uint16_t len)
{
  struct pbuf *p;
  
  if(buffer== NULL || netif == NULL)
    	goto _exit;
#if CONFIG_EMAC_L2_TO_L3_RX_BUF_MODE
  p = pbuf_alloc(PBUF_RAW, len, PBUF_RAM);
  if (p == NULL) {
    return;
  }
  memcpy(p->payload, buffer, len);

/* full packet send to tcpip_thread to process */
if (netif->input(p, netif) != ERR_OK) {
  LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
  pbuf_free(p);
}

#else
  p = pbuf_alloc(PBUF_RAW, len, PBUF_REF);
  if (p == NULL){
    return;
  }
  p->payload = buffer;
  p->user_flag = PBUF_USER_FLAG_OWNER_ETH;
  p->user_buf = buffer;
 
  /* full packet send to tcpip_thread to process */
if (netif->input(p, netif) != ERR_OK) {
  LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
  p->user_flag = PBUF_USER_FLAG_OWNER_NULL;
  pbuf_free(p);
}
#endif
_exit:
;	  
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t
ethernetif_init(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */

#if ESP_LWIP
  sprintf(hostname, "ESP_%02X%02X%02X", netif->hwaddr[3], netif->hwaddr[4], netif->hwaddr[5]);
  netif->hostname = hostname;
  
#else
  sprintf(hostname, "ESP_%02X%02X%02X", netif->hwaddr[3], netif->hwaddr[4], netif->hwaddr[5]);
  netif->hostname = hostname;
#endif
  
#endif /* LWIP_NETIF_HOSTNAME */

  /*
   * Initialize the snmp variables and counters inside the struct netif.
   * The last argument should be replaced with your link speed, in units
   * of bits per second.
   */
  NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
  netif->output = etharp_output;
#if LWIP_IPV6
  netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */
  netif->linkoutput = ethernet_low_level_output;
  
  /* initialize the hardware */
  ethernet_low_level_init(netif);

  return ERR_OK;
}
