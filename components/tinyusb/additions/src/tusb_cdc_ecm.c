/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Peter Lawrence
 * Modifications Copyright © 2021 Ci4Rail GmbH
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

#include <stdio.h>
#include "esp_log.h"
#include "lwip/tcpip.h"
#include "lwip/init.h"
#include "lwip/ip.h"
#include "class/net/net_device.h"
#include "netif/etharp.h"
#include "tusb_cdc_ecm.h"
#include "descriptors_control.h"

const static char *TAG = "CDC-ECM Driver";

#define FRAME_BUFFER_SIZE  100

struct frame_buffer_t {
  uint8_t read_idx;
  uint8_t write_idx;
  struct pbuf* frame[FRAME_BUFFER_SIZE];
};

/* lwip context */
static struct netif netif_data;
/* shared between tud_network_recv_cb() and service_traffic() */
struct frame_buffer_t rx_frame_buffer = 
{
  .read_idx = 0,
  .write_idx = 0,
  .frame = {}
};
/* semaphore as receive event, is intended to notify service_traffic thread in case a new frame was received */
SemaphoreHandle_t rx_event = NULL;
/* Dummy, not really used. Only required for the usage of RNDIS protocol, but we are using CDC-ECM. Is only declared, that there will be no compile error. */
const uint8_t tud_network_mac_address[6];


static void service_traffic(void)
{
  /* handle any packet received by tud_network_recv_cb() */
  while(rx_frame_buffer.frame[rx_frame_buffer.read_idx]) {
    ethernet_input(rx_frame_buffer.frame[rx_frame_buffer.read_idx], &netif_data);

    rx_frame_buffer.frame[rx_frame_buffer.read_idx] = NULL;
    rx_frame_buffer.read_idx++;
    if(rx_frame_buffer.read_idx >= FRAME_BUFFER_SIZE) {
      rx_frame_buffer.read_idx = 0;
    }
    tud_network_recv_renew();
  }
}

static void service_traffic_task(void* param)
{
  while(1) {
    /* do only service traffic if event was received */
    if(xSemaphoreTake(rx_event, portMAX_DELAY))
    {
      service_traffic();
    }
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
  for(uint8_t idx = 0; idx < FRAME_BUFFER_SIZE; idx++) {
    if(rx_frame_buffer.frame[idx] != NULL) {
      pbuf_free(rx_frame_buffer.frame[idx]);
    }
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
    char mac_str[17];

    /* initialize tcp ip stack */
    tcpip_init(NULL, NULL);

    /* the lwip virtual MAC address must be different from the host's; to ensure this, we toggle the LSbit */
    netif->hwaddr_len = sizeof(cfg.mac_address);
    memcpy(netif->hwaddr, cfg.mac_address, sizeof(cfg.mac_address));
    netif->hwaddr[5] ^= 0x01;
    /* set host MAC address */
    tusb_set_mac_address(cfg.mac_address);
    
    /* print host MAC address */
    for(int i = 0; i < 6; i++) {
      snprintf(mac_str + (i*3), 4,"%02x%s", cfg.mac_address[i], i<5 ? ":" : "");
    }
    ESP_LOGI(TAG,"Host NIC MAC Address: %s", mac_str);

    netif = netif_add(netif, &(cfg.ipaddr), &(cfg.netmask), &(cfg.gateway), NULL, netif_init_cb, ip_input);
    netif_set_default(netif);
    if(!netif_is_up(&netif_data)) {
      return ESP_FAIL;
    }

    /* create receive event */
    rx_event = xSemaphoreCreateBinary();
    if(rx_event == NULL) {
      return ESP_FAIL;
    }

    xTaskCreate( service_traffic_task, "service_traffic", 4096, NULL, 6, NULL);

    return ESP_OK;
}

bool tud_network_recv_cb(const uint8_t *src, uint16_t size)
{
  /* check if buffer run full */
  if(rx_frame_buffer.frame[rx_frame_buffer.write_idx] != NULL) {
    /* signal the inability to accept it */
    return false;
  }

  if (size) {
    struct pbuf *p = pbuf_alloc(PBUF_RAW, size, PBUF_POOL);

    if (p) {
      /* pbuf_alloc() has already initialized struct; all we need to do is copy the data */
      memcpy(p->payload, src, size);

      /* store away the pointer for service_traffic() to later handle */
      rx_frame_buffer.frame[rx_frame_buffer.write_idx] = p;

      rx_frame_buffer.write_idx++;
      if(rx_frame_buffer.write_idx >= FRAME_BUFFER_SIZE) {
        rx_frame_buffer.write_idx = 0;
      }

      /* new frame received -> set rx event */
      xSemaphoreGive(rx_event);
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
