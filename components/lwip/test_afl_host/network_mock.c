#include "no_warn_host.h"
#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "tcpip_adapter.h"
#include <string.h>
#include <stdio.h>

const ip_addr_t ip_addr_any;
const ip_addr_t ip_addr_broadcast;
struct ip_globals ip_data;
struct netif *netif_list;


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
    if (pcb == NULL)
    {
        free(pcb);
    }
}

struct udp_pcb *udp_new(void)
{
    return malloc(sizeof(struct udp_pcb));
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

void dns_setserver(u8_t numdns, const ip_addr_t *dnsserver)
{
}

uint32_t esp_random(void)
{
    return 0;
}

err_t etharp_query(struct netif *netif, const ip4_addr_t *ipaddr, struct pbuf *q)
{
    return ESP_OK;
}

u32_t lwip_ntohl(u32_t x)
{
    return 0;
}

void netif_set_addr(struct netif *netif, const ip4_addr_t *ipaddr, const ip4_addr_t *netmask,
                    const ip4_addr_t *gw)
{
}

void pbuf_realloc(struct pbuf *p, u16_t size)
{
    if (p != NULL)
    {
        uint8_t *buf = malloc(size);
        free(p->payload);
        p->payload = buf;
        p->len = size;
        p->tot_len = size;
    }
}

u16_t pbuf_copy_partial(struct pbuf *p, void *dataptr, u16_t len, u16_t offset)
{
    return 0;
}
err_t udp_connect(struct udp_pcb *pcb, const ip_addr_t *ipaddr, u16_t port)
{
    return ESP_OK;
}

err_t udp_sendto_if(struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *dst_ip, u16_t dst_port, struct netif *netif)
{
    return ESP_OK;
}

err_t udp_sendto_if_src(struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *dst_ip, u16_t dst_port, struct netif *netif, const ip_addr_t *src_ip)
{
    return ESP_OK;
}