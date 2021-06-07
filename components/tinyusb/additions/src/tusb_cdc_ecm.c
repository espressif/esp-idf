/*
Copyright © 2021 Ci4Rail GmbH
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
 http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Peter Lawrence
 *
 * influenced by lrndis https://github.com/fetisov/lrndis
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */


#include "lwip/tcpip.h"
#include "lwip/init.h"
#include "lwip/ip.h"
#include "class/net/net_device.h"
#include "netif/etharp.h"
#include "tusb_cdc_ecm.h"
#include "descriptors_control.h"

/* lwip context */
static struct netif netif_data;
/* shared between tud_network_recv_cb() and service_traffic() */
static struct pbuf *rx_frame;
/* Dummy, not really used. Only required for the usage of RNDIS protocol, but we are using CDC-ECM. Is only declared, that there will be no compile error. */
const uint8_t tud_network_mac_address[6];

static void service_traffic(void)
{
  /* handle any packet received by tud_network_recv_cb() */
  if (rx_frame) {
    ethernet_input(rx_frame, &netif_data);
    rx_frame = NULL;
    tud_network_recv_renew();
  }

  sys_check_timeouts();
}

static void service_traffic_task(void* param)
{
  while(1) {
    service_traffic();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

static err_t linkoutput_fn(struct netif *netif, struct pbuf *p)
{
  (void)netif;

  for (;;) {
    /* if TinyUSB isn't ready, we must signal back to lwip that there is nothing we can do */
    if (!tud_ready()) {
      return ERR_USE;
    }

    /* if the network driver can accept another packet, we make it happen */
    if (tud_network_can_xmit()) {
      tud_network_xmit(p, 0);
      return ERR_OK;
    }

    /* transfer execution to TinyUSB in the hopes that it will finish transmitting the prior packet */
    tud_task();
  }
}

static err_t output_fn(struct netif *netif, struct pbuf *p, const ip_addr_t *addr)
{
  return etharp_output(netif, p, addr);
}

void tud_network_init_cb(void)
{
  /* if the network is re-initializing and we have a leftover packet, we must do a cleanup */
  if (rx_frame) {
    pbuf_free(rx_frame);
    rx_frame = NULL;
  }
}

static err_t netif_init_cb(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));
  netif->mtu = CFG_TUD_NET_MTU;
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP | NETIF_FLAG_UP;
  netif->state = NULL;
  netif->name[0] = 'E';
  netif->name[1] = 'X';
  netif->linkoutput = linkoutput_fn;
  netif->output = output_fn;
  return ERR_OK;
}

esp_err_t tusb_ethernet_over_usb_init(tinyusb_config_ethernet_over_usb_t cfg)
{
    struct netif *netif = &netif_data;

    /* initialize tcp ip stack */
    tcpip_init(NULL, NULL);

    /* the lwip virtual MAC address must be different from the host's; to ensure this, we toggle the LSbit */
    netif->hwaddr_len = sizeof(cfg.mac_address);
    memcpy(netif->hwaddr, cfg.mac_address, sizeof(cfg.mac_address));
    netif->hwaddr[5] ^= 0x01;
    /* set host MAC address */
    tusb_set_mac_address(cfg.mac_address);

    printf("MAC: ");
    for(int i = 0; i < sizeof(cfg.mac_address)/sizeof(cfg.mac_address[0]); i++)
    {
      printf("%02x:", cfg.mac_address[i]);
    }
    printf("\n");

    netif = netif_add(netif, &(cfg.ipaddr), &(cfg.netmask), &(cfg.gateway), NULL, netif_init_cb, ip_input);
    netif_set_default(netif);
    while (!netif_is_up(&netif_data));

    xTaskCreate( service_traffic_task, "service_traffic", 4096, NULL, 5, NULL);

    return ESP_OK;
}

bool tud_network_recv_cb(const uint8_t *src, uint16_t size)
{
  /* this shouldn't happen, but if we get another packet before 
  parsing the previous, we must signal our inability to accept it */
  if (rx_frame) {
    return false;
  }

  if (size) {
    struct pbuf *p = pbuf_alloc(PBUF_RAW, size, PBUF_POOL);

    if (p) {
      /* pbuf_alloc() has already initialized struct; all we need to do is copy the data */
      memcpy(p->payload, src, size);

      /* store away the pointer for service_traffic() to later handle */
      rx_frame = p;
    }
  }

  return true;
}

uint16_t tud_network_xmit_cb(uint8_t *dst, void *ref, uint16_t arg)
{
  struct pbuf *p = (struct pbuf *)ref;
  struct pbuf *q;
  uint16_t len = 0;

  (void)arg;

  /* traverse the "pbuf chain"; see ./lwip/src/core/pbuf.c for more info */
  for(q = p; q != NULL; q = q->next) {
    memcpy(dst, (char *)q->payload, q->len);
    dst += q->len;
    len += q->len;
    if (q->len == q->tot_len) break;
  }

  return len;
}
