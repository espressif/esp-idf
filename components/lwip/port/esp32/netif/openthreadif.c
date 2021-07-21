// Copyright 2021 Espressif Systems (Shanghai) CO LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License

#include <string.h>

#include "esp_netif.h"
#include "esp_netif_net_stack.h"
#include "lwip/netif.h"
#include "lwip/pbuf.h"
#include "netif/openthreadif.h"
#include "openthread/error.h"
#include "openthread/ip6.h"
#include "openthread/link.h"
#include "openthread/message.h"

#define OPENTHREAD_IP6_MTU 1280

static err_t openthread_output_ip6(struct netif *netif, struct pbuf *p, const struct ip6_addr *peer_addr)
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
        /* content in payload has been copied to OpenThread queue, it's safe to free pbuf now */
        pbuf_free(q);
    }
    /* Check error */
    if (unlikely(ret != ESP_OK)) {
        return ERR_ABRT;
    } else {
        return ERR_OK;
    }
}

void openthread_netif_input(void *h, void *buffer, size_t len, void *eb)
{
    struct netif *netif = h;
    struct pbuf *p;
    otMessage *message = (otMessage *)buffer;

    if (unlikely(buffer == NULL || !netif_is_up(netif))) {
        return;
    }

    /* Allocate LINK buffer in case it's forwarded to WiFi/ETH */
    p = pbuf_alloc(PBUF_LINK, len, PBUF_POOL);
    if (p == NULL) {
        LWIP_DEBUGF(NETIF_DEBUG, ("Failed to allocate input pbuf for OpenThread netif\n"));
        return;
    }

    if (unlikely(otMessageRead(message, 0, p->payload, len) != OT_ERROR_NONE)) {
        LWIP_DEBUGF(NETIF_DEBUG, ("Failed to read OpenThread message\n"));
    }

#if ESP_LWIP
    p->l2_owner = NULL;
    p->l2_buf = NULL;
#endif
    /* full packet send to tcpip_thread to process */
    if (unlikely(netif->input(p, netif) != ERR_OK)) {
        LWIP_DEBUGF(NETIF_DEBUG, ("openthread_netif_input: IP input error\n"));
        pbuf_free(p);
    }
    /* the pbuf will be free in upper layer, eg: tcpip_input */
}

err_t openthread_netif_init(struct netif *netif)
{
    netif->name[0] = 'o';
    netif->name[1] = 't';
    netif->hwaddr_len = sizeof(otExtAddress);
    memset(netif->hwaddr, 0, sizeof(netif->hwaddr));
    netif->mtu = OPENTHREAD_IP6_MTU;
    netif->flags = NETIF_FLAG_BROADCAST;
    netif->output = NULL;
    netif->output_ip6 = openthread_output_ip6;
    netif->l2_buffer_free_notify = NULL;
    netif_set_link_up(netif);

    return ERR_OK;
}
