#include <stdio.h>
#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "tcpip_adapter.h"
#include <string.h>

u16_t lwip_htons(u16_t n)
{
    return 0;
}

u32_t lwip_htonl(u32_t n)
{
    return 0;
}

esp_err_t tcpip_adapter_get_ip_info(tcpip_adapter_if_t tcpip_if, tcpip_adapter_ip_info_t *ip_info)
{
 return ESP_OK;
}

struct pbuf * pbuf_alloc(pbuf_layer layer, u16_t length, pbuf_type type)
{
    struct pbuf * p;
    p = (struct pbuf *)malloc(MEMP_PBUF_POOL);
    p->tot_len = length;
    p->next = NULL;
    p->type = PBUF_POOL;
    p->len = length;
    p->payload = malloc(length);
    return p;
}

u8_t pbuf_free(struct pbuf *p)
{
    if (p) {
        if (p->payload) {
            free(p->payload);
            p->payload = NULL;
        }
        free (p);
        p = NULL;
    }
    return 1;
}

err_t udp_sendto(struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *dst_ip, u16_t dst_port)
{
    return ESP_OK;
}

void udp_remove(struct udp_pcb *pcb)
{
}

struct udp_pcb *udp_new(void)
{
    return NULL;
}

err_t udp_bind(struct udp_pcb *pcb, const ip_addr_t *ipaddr, u16_t port)
{
    return ESP_OK;
}

void udp_recv(struct udp_pcb *pcb, udp_recv_fn recv, void *recv_arg)
{
}

void udp_disconnect(struct udp_pcb *pcb)
{
}
